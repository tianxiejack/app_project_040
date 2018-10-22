/*
 * Arachne_Message.c
 *
 *  Created on: Dec 8, 2017
 *      Author: xz
 */

/*Arachne_Message.c*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h> 	/* add this: exit返回,不会报提示信息 */
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>  /* ture false 有效*/
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include "Arachne_Ipc_Message.h"
#include "Serial_port.h"
#include <semaphore.h>

#include "osa_sem.h"

OSA_SemHndl sendmsgSem;
/*extern "C"
 {
 }*/


#define PTHREAD_NAME    "Receive Theater"
#define PTHREAD_NAME1   "Send Theater"

int ipc_port = 125;
key_t ipc_key;
int ipc_qid;
int fd;
IPC_msg g_MSG = { 0 };
Return_pack return_pack;
unsigned char send_buf[15];
static unsigned int Cycle_time = 33;
pthread_mutex_t Mutex[2] = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	char serial_tty[64]; //串口设备节点
	int baud_rate; //波特率
} SerialSet; //串口设置
SerialSet Serial[1] = { "/dev/ttyTHS2", 115200 };
char IPC_ftok_path[64] = "/home/"; //IPC引用路径

void UartSet() {
	int err;
	/*打开串口，返回文件描述符并初始化*/
	if (-1 == (fd = UART0_Open(fd, (char*) &Serial[0].serial_tty))) {
		printf("Controloanel:fail to UART0_Open\n");
	}
	if (-1 == (err = UART0_Init(fd, Serial[0].baud_rate, 0, 8, 1, 'N'))) {
		printf("Controloanel:fail to UART0_Init\n");
	}
}

void IPC_pthread_create(char *pthread_name, pthread_t *thread,
		const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
	int ret;
	ret = pthread_create(thread, NULL, start_routine, NULL);
	if (ret != 0) {
		printf("Creat %s thread failed!\n", pthread_name);
		exit(ret);
	}
}

/*接收线程*/
void *Recv_ipc(void *arg) {
	int i;
	int ret_value;
	IPC_msg msg;
	memset(&msg, 0, sizeof(IPC_msg));
	printf("\n%x\n", ipc_qid);
	while (1) {
		ret_value = msgrcv(ipc_qid, &msg, sizeof(msg), IPC_MSG_TYPE_STE_TO_AVT,
				0);
		if (ret_value < 0) {
			printf("Receive IPC msg failed,errno=%d !!!\n",
			errno);
		}
		pthread_mutex_lock(&Mutex[0]);
		g_MSG = msg;
		pthread_mutex_unlock(&Mutex[0]);
	}
}

void SetReturnPack(Return_pack* value) {
	int i;
	pthread_mutex_lock(&Mutex[1]);
	return_pack = *value;
	Cycle_time = return_pack.CycleTime;
	send_buf[0] = 0xeb;
	send_buf[1] = return_pack.payload.message_id;
	send_buf[2] = return_pack.payload.Track_Deviation_X & 0xff;
	send_buf[3] = (return_pack.payload.Track_Deviation_X >> 8) & 0xff;
	send_buf[4] = return_pack.payload.Track_Deviation_Y & 0xff;
	send_buf[5] = (return_pack.payload.Track_Deviation_Y >> 8) & 0xff;
	send_buf[6] = ((return_pack.payload.Track_Thermal & 0x3) << 3)
			| ((return_pack.payload.Track_Tv & 0x3) << 1)
			| (return_pack.payload.current_video & 0x1);
	send_buf[7] = return_pack.payload.Senser_Aim_CurX & 0xff;
	send_buf[8] = (return_pack.payload.Senser_Aim_CurX >> 8) & 0xff;
	send_buf[9] = return_pack.payload.Senser_Aim_CurY & 0xff;
	send_buf[10] = (return_pack.payload.Senser_Aim_CurY >> 8) & 0xff;
	send_buf[11] = ((return_pack.payload.Self_Tr_S.self_state & 0x1) << 7)
			| ((return_pack.payload.Self_Tr_S.video_input & 0x3) << 5)
			| ((return_pack.payload.Self_Tr_S.roam_boot & 0x1) << 4)
			| ((return_pack.payload.Self_Tr_S.mul_target_AVT & 0x3) << 2)
			| (return_pack.payload.Self_Tr_S.track_state & 0x3);
	send_buf[12] = ((return_pack.payload.Compress_Transfer & 0x3) << 5)
			| ((return_pack.payload.Power_On & 0x1) << 4)
			| (return_pack.payload.r_interface_rate & 0x3);
	send_buf[13] = ((return_pack.payload.r_thermal_frame & 0x3) << 2)
			| (return_pack.payload.r_tv_frame & 0x3);
	send_buf[14] = 0;
	for (i = 0; i < 14; i++) {
		send_buf[14] ^= send_buf[i];
	}
	pthread_mutex_unlock(&Mutex[1]);
} //设置返回包的数据，以及周期发送时间

/*周期发送发送线程*/

void sendMsg_Sem()
{

	OSA_semSignal(&sendmsgSem);
}

void *SendMessage(void *arg) {
	int i;
	int len;
	struct timeval last = { 0 }, curent;
	unsigned int timer;

	OSA_semCreate(&sendmsgSem, 1, 0);
	
	while (1) {
		
		OSA_semWait(&sendmsgSem, OSA_TIMEOUT_FOREVER);
		UART0_Send(fd, (char *)send_buf, sizeof(send_buf));
		/*
		gettimeofday(&curent, NULL);
		timer = (curent.tv_sec - last.tv_sec) * 1000
				+ (curent.tv_usec - last.tv_usec) / 1000;
		if (timer > Cycle_time) {
			len = UART0_Send(fd, send_buf, sizeof(send_buf));
			last = curent;
		}*/
	}
}

/*消息队列模块：包括初始化和开接收线程*/
void InitIPCModule() {
	pthread_t thread[6];
	UartSet();
	/*创建消息队列*/
	ipc_key = ftok(IPC_ftok_path, ipc_port);
	if (ipc_key == -1) {
		printf(" port %d ftok key failed erro:  \n", ipc_port);
		perror(IPC_ftok_path);
		exit(-1);
	}
	ipc_qid = msgget(ipc_key, IPC_EXCL); /*检查消息队列是否存在*/
	if (ipc_qid < 0) {
		ipc_qid = msgget(ipc_key, IPC_CREAT | 0666);/*创建消息队列*/
		if (ipc_qid < 0) {
			printf("failed to create msq | errno=%d \n", errno);
			perror(" ");
			exit(-1);
		}
	} else {
		printf("port %d  create success ! \n", ipc_port);
	}
	/*创建线程*/
	IPC_pthread_create((char *)PTHREAD_NAME, &thread[0], NULL, Recv_ipc, NULL);
	IPC_pthread_create((char *)PTHREAD_NAME1, &thread[1], NULL, SendMessage, NULL);
}

IPC_msg GetMessage() {
	return g_MSG;
}

void IPC_Destroy() {
	msgctl(ipc_qid, IPC_RMID, 0); //删除消息队列
	UART0_Close(fd); //关闭串口
}
