#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

//extern "C"{
#include"Arachne_Ipc_Message.h";
//}

#include "msgProc.hpp"
#include "cr_timer/setTimer.h"
#include "globalData.h"
#include "cr_gpio/gpio040.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include "crCore.hpp"
#include "cr_osd/app_proj_xgs040.h"

ICore_1001 *send_core = NULL;

CGlobalData *psendOsd = CGlobalData::getInstance();

OSA_SemHndl recvmsgSem;
OSA_SemHndl sendthrSem;
Return_pack send_msg;

static bool b_trkStat = false;
unsigned int itrkTime = 0;

static Void * uart_dataRecv(Void * prm);
static Void * uart_dataSend(Void * prm);


void recvmsg_040(IPC_msg data)
{
	static bool b_secondTrk=false;
	static bool b_saveAxis = false;
	unsigned char TimeSelect = 0;
	unsigned char Time_Hour,Time_Minute,Time_Second;
	
	CORE_CMD inCtrl, *pMsg = NULL;
	pMsg = &inCtrl;
	
	memset(pMsg,0,sizeof(CORE_CMD));

	
	if(data.payload.sen_mes.video_switch==1)
	{
		psendOsd->osd_core.chId = pMsg->chId=0;		
	}
	else if(data.payload.sen_mes.video_switch==2)
	{
		psendOsd->osd_core.chId = pMsg->chId=1;		
	}
	
	app_ctrl_setSensor(pMsg);

	psendOsd->osd_core.osd_enEnh = pMsg->enENH[pMsg->chId] = data.payload.sen_mes.Image_Enhance;
	app_ctrl_setEnhance(pMsg);

	pMsg->Color = data.payload.sen_mes.character_color;
	app_ctrl_setDispColor(pMsg);


	psendOsd->osd_core.osd_enDisp = pMsg->DispLever = data.payload.sen_mes.character_state;
	app_ctrl_setDispGrade(pMsg);

	//printf("jet +++ enMMT=%d\n",data.payload.Track_Search.MulTarget_Detect);
	pMsg->enMTD = data.payload.Track_Search.MulTarget_Detect;
	app_ctrl_setMMT(pMsg);

	psendOsd->osd_core.osd_enZoom = pMsg->enZoomx = data.payload.MulTarget_Num.TV_SizeChage;
	app_ctrl_setZoom(pMsg);

	//printf("jet +++ TrkWin=%d\n",data.payload.MulTarget_Num.bomen_size);
	
	psendOsd->osd_core.osd_WorkMode = data.payload.Work_Pattern.cur_mode;
	switch(data.payload.Work_Pattern.cur_mode)
	{
		case Current_Mode_Lock:
			pMsg->enTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Collect:
			pMsg->enTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Manual:

			pMsg->enTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Scan:
			pMsg->enTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Tracking:	
			pMsg->enTrk = true;
			
			if(b_secondTrk)
			{
				psendOsd->osd_core.osd_enSecondTrk = false;
				b_secondTrk = false;

				pMsg->secondTrk_X = (data.payload.Track_Search.HighOrder_Hor <<8)|data.payload.Track_Search.LowOrder_Hor;
				pMsg->secondTrk_Y = (data.payload.Track_Search.HighOrder_Ver <<8)|data.payload.Track_Search.LowOrder_Ver;
				
				app_ctrl_setSearchTrk(pMsg);
			}
			
			
			app_ctrl_setTrkStat(pMsg);

			if(data.payload.MulTarget_Num.bomen_size==Bomen_Size_Big)
				pMsg->TrkWinSize = Bomen_Size_Big;
			else if(data.payload.MulTarget_Num.bomen_size==Bomen_Size_Middle)
				pMsg->TrkWinSize = Bomen_Size_Middle;
			else if(data.payload.MulTarget_Num.bomen_size==Bomen_Size_Small)
				pMsg->TrkWinSize = Bomen_Size_Small;
			app_ctrl_setTrkBomen(pMsg);
			//printf("jet +++ xref=%d,yref=%d\n", data.payload.Track_Trim.move_track_hor, data.payload.Track_Trim.move_track_ver);
			switch(data.payload.Track_Trim.move_track_hor)
			{
				case Move_Track_Hor_Left:
					
					app_ctrl_setAimPos(eTrk_ref_left, 1);
					break;
				case Move_Track_Hor_Right:

					app_ctrl_setAimPos(eTrk_ref_right, 1);
					break;
				default:
			
					break;
			}
			switch(data.payload.Track_Trim.move_track_ver)
			{
				case Move_Track_Ver_Up:

					app_ctrl_setAimPos(eTrk_ref_up, 1);
					break;
				case Move_Track_Ver_Down:

					app_ctrl_setAimPos(eTrk_ref_down, 1);
					break;
				default:
			
					break;
			}
			
			break;
		case Current_Mode_Track_Search:
			b_secondTrk = true;
			psendOsd->osd_core.osd_enSecondTrk = true;

			psendOsd->osd_core.osd_secondTrk_X = (data.payload.Track_Search.HighOrder_Hor <<8)|data.payload.Track_Search.LowOrder_Hor;
			psendOsd->osd_core.osd_secondTrk_Y = (data.payload.Track_Search.HighOrder_Ver <<8)|data.payload.Track_Search.LowOrder_Ver;
			
			//printf("jet +++ H=%02x\n", (data.payload.Track_Search.HighOrder_Hor <<8)|data.payload.Track_Search.LowOrder_Hor);
			break;
		case Current_Mode_Geografia_Search:
			pMsg->enTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;		
		case Current_Mode_Serve:
			pMsg->enTrk = false;
			app_ctrl_setTrkStat(pMsg);
			
			if(data.payload.Work_Pattern.Shaft_Start == 1)
			{
				psendOsd->osd_core.osd_AxisMode = true;
				b_saveAxis = true;
			}

			if(psendOsd->osd_core.osd_AxisMode)
			{
				switch(data.payload.Track_Trim.move_cross_hor)
				{
					case Move_CrossCurve_Hor_Left:
						
						app_ctrl_setAxisPos(eAxis_ref_left, 1);
						break;
					case Move_CrossCurve_Hor_Right:

						app_ctrl_setAxisPos(eAxis_ref_right, 1);
						break;
					default:
				
						break;
				}
				switch(data.payload.Track_Trim.move_cross_ver)
				{
					case Move_CrossCurve_Ver_Up:

						app_ctrl_setAxisPos(eAxis_ref_up, 1);
						break;
					case Move_CrossCurve_Ver_Down:

						app_ctrl_setAxisPos(eAxis_ref_down, 1);
						break;
					default:
				
						break;
				}
			}

			if((data.payload.Work_Pattern.Shart_Exit == 1)&&(b_saveAxis))
			{
				psendOsd->osd_core.osd_AxisMode = false;
				b_saveAxis = false;
				app_ctrl_SaveAxisPos();
			}
			break;
		case Current_Mode_System_SelfInspection:
			pMsg->enTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		default:
				
			break;
	}

	//printf("jet +++++ enlaser=%d,stat=%d\n",data.payload.sen_mes.Laser_state,data.payload.Laser_Mes.laser_state);
	if(data.payload.sen_mes.Laser_state==0)
		psendOsd->osd_core.osd_enLaser = false;
	else
	{
		psendOsd->osd_core.osd_enLaser = true;

		switch(data.payload.Laser_Mes.laser_state)
		{
			case Laser_State_Prepare:
				psendOsd->osd_core.osd_laserStat = Laser_State_Prepare;
				break;
			case Laser_State_Standby:				
				psendOsd->osd_core.osd_laserStat = Laser_State_Standby;
				break;
			case Laser_State_Once_Range:
				psendOsd->osd_core.osd_laserStat = Laser_State_Once_Range;
				break;
			case Laser_State_Continuous_Range_1Hz:
				psendOsd->osd_core.osd_laserStat = Laser_State_Continuous_Range_1Hz;
				break;
			case Laser_State_Continuous_Range_5Hz:
				psendOsd->osd_core.osd_laserStat = Laser_State_Continuous_Range_5Hz;
				break;
			case Laser_State_Once_Irradiate:
				psendOsd->osd_core.osd_laserStat = Laser_State_Once_Irradiate;
				break;
			case Laser_State_Continuous_Irradiate:
				psendOsd->osd_core.osd_laserStat = Laser_State_Continuous_Irradiate;
				break;				
			default:		
				break;
		}

	}

	//printf("jet +++ osd_enLA=%d,laser_code=%d\n",psendOsd->osd_core.osd_enLA,data.payload.Laser_Mes.laser_code);
	if(data.payload.Laser_Mes.laser_mes_d==Laser_Mes_D_Show)
		psendOsd->osd_core.osd_enLA = true;
	else
		psendOsd->osd_core.osd_enLA = false;

	psendOsd->osd_core.osd_laserCode = data.payload.Laser_Mes.laser_code;

	//printf("jet +++ tvFov=%d,frFov=%d\n",data.payload.Sensor_Tv_Angle,data.payload.Sensor_Thermal_Angle);
	psendOsd->osd_core.osd_TVFovAngle = data.payload.Sensor_Tv_Angle;
	psendOsd->osd_core.osd_FRFovAngle = data.payload.Sensor_Thermal_Angle;
	psendOsd->osd_core.osd_LMC = data.payload.Work_Pattern.LMC;
	
	psendOsd->osd_core.osd_panAngle = data.payload.TurretAngle_Hor;
	//if(0 == (data.payload.TurretAngle_Ver & 0x80000000))
		//psendOsd->osd_core.osd_tiltAngle = data.payload.TurretAngle_Ver & 0x7fffffff;
	//else
		//psendOsd->osd_core.osd_tiltAngle = ((~(abs(data.payload.TurretAngle_Ver & 0x7fffffff)))+1) | 0x80000000;

	psendOsd->osd_core.osd_tiltAngle = data.payload.TurretAngle_Ver;

	//printf("jet +++ pan=%d,tilt=%x,recv=%d\n",psendOsd->osd_core.osd_panAngle,psendOsd->osd_core.osd_tiltAngle,data.payload.TurretAngle_Ver);

	TimeSelect = data.payload.Td.time_flag;
	
	//time hour minute second
	if(TimeSelect==0x01)
		Time_Hour=data.payload.Td.time_date%24;
	else if(TimeSelect==0x02)
		Time_Minute=data.payload.Td.time_date%60;
	else if(TimeSelect==0x03)
		Time_Second=data.payload.Td.time_date%60;

	psendOsd->osd_core.osd_Time = Time_Hour<<16|Time_Minute<<8|Time_Second;
	//printf("jet +++ timeSelect=%d,time=%d\n", data.payload.Td.time_flag, data.payload.Td.time_date);

	psendOsd->osd_core.osd_Distance = data.payload.Target_Distance;

	psendOsd->osd_core.osd_dispZoom = data.payload.Sensor_Image_EN.Infrared_Change;
	psendOsd->osd_core.osd_dispFREnh = data.payload.Sensor_Image_EN.Sensor_Thermal;
	psendOsd->osd_core.osd_dispTVEnh = data.payload.Sensor_Image_EN.Sensor_Tv_Enhance;
	
}

void app_getvideostatus()
{
	unsigned int unitFaultStat = 0;
	if(Hard_getccdstatus() == 0)
		unitFaultStat |= (1<<0);
	else if(Hard_getccdstatus() == 1)
		unitFaultStat |= (0<<0);
	if(Hard_getfirstatus() == 0)
		unitFaultStat |= (1<<1);
	else if(Hard_getfirstatus() == 1)
		unitFaultStat |= (0<<1);

	if(unitFaultStat==0x00)
	{
		send_msg.payload.Self_Tr_S.video_input = Video_Input_Normal;
		send_msg.payload.Self_Tr_S.self_state = Self_State_Normal;
	}
	else if(unitFaultStat==0x01)
	{
		send_msg.payload.Self_Tr_S.video_input = Video_Input_TV_Off;
		send_msg.payload.Self_Tr_S.self_state = Self_State_Abnomal;
	}
	else if(unitFaultStat==0x02)
	{
		send_msg.payload.Self_Tr_S.video_input = Video_Input_Thermal_Off;
		send_msg.payload.Self_Tr_S.self_state = Self_State_Abnomal;
	}
	else if(unitFaultStat==0x03)
	{
		send_msg.payload.Self_Tr_S.video_input = Video_Input_Together_Off;
		send_msg.payload.Self_Tr_S.self_state = Self_State_Abnomal;
	}
	//OSA_printf(" %d: unitFault change from %x to %x\n", OSA_getCurTimeInMsec(),
			//send_msg.payload.Self_Tr_S.video_input, unitFaultStat);
		
	return ;
}
static void recvmsg_Sem(UInt32 elapsedTime)
{
	OSA_semSignal(&recvmsgSem);

}

static void sendmsg_Sem(UInt32 elapsedTime)
{
	sendMsg_Sem();
	
	OSA_semSignal(&sendthrSem);
}

static Void * uart_dataRecv(Void * prm)
{
	IPC_msg recv_msg;
		
	while(1){
		
		OSA_semWait(&recvmsgSem, OSA_TIMEOUT_FOREVER);		
		recv_msg = GetMessage();
		
		recvmsg_040(recv_msg);
	}
}

static Void * uart_dataSend(Void * prm)
{	
	send_core = (ICore_1001 *)ICore::Qury(COREID_1001);
	float unitTrkX,unitTrkY;		

	while(1){
		
		OSA_semWait(&sendthrSem, OSA_TIMEOUT_FOREVER);

		send_msg.payload.message_id = Message_Id_TV;
		
		if(send_core->m_stats.mainChId==Current_Video_TV)
			send_msg.payload.current_video = Current_Video_TV;
		else if(send_core->m_stats.mainChId==Current_Video_Thermal)
			send_msg.payload.current_video = Current_Video_Thermal;
		
		//printf("jet +++ xy(%d,%d)\n",send_msg.payload.Track_Deviation_X,send_msg.payload.Track_Deviation_Y);

		send_msg.payload.Senser_Aim_CurX = (int)send_core->m_stats.chn[send_core->m_stats.mainChId].axis.x;
		send_msg.payload.Senser_Aim_CurY = (int)send_core->m_stats.chn[send_core->m_stats.mainChId].axis.y;
		
		if(send_core->m_stats.enableTrack)
		{
			if(send_core->m_stats.iTrackorStat==0x01)
				send_msg.payload.Self_Tr_S.track_state = Track_State_Tracking;
			
			if(send_core->m_stats.iTrackorStat == 0x01)
			{
				b_trkStat=false;
			}
			else if(send_core->m_stats.iTrackorStat == 0x02)
			{
				if(!b_trkStat)
				{
					b_trkStat=true;
					itrkTime=OSA_getCurTimeInMsec();
				}
				
				if((OSA_getCurTimeInMsec()-itrkTime)>5000)
					send_msg.payload.Self_Tr_S.track_state=Track_State_Lose;
				else
					send_msg.payload.Self_Tr_S.track_state=Track_State_Memory;
			}		

			unitTrkX = (send_core->m_stats.trackPos.x - send_core->m_stats.chn[send_core->m_stats.mainChId].axis.x);
			unitTrkY = (send_core->m_stats.chn[send_core->m_stats.mainChId].axis.y - send_core->m_stats.trackPos.y);

			if(unitTrkX < -(vcapWH[send_core->m_stats.mainChId][0]/2))
				send_msg.payload.Track_Deviation_X = -vcapWH[send_core->m_stats.mainChId][0]/2*10;
			else if(unitTrkX > (vcapWH[send_core->m_stats.mainChId][0]/2))
				send_msg.payload.Track_Deviation_X = vcapWH[send_core->m_stats.mainChId][0]/2*10;
			else
				send_msg.payload.Track_Deviation_X = (int)(unitTrkX*10.0);
			
			if(unitTrkY < -(vcapWH[send_core->m_stats.mainChId][1]/2))
				send_msg.payload.Track_Deviation_Y = -vcapWH[send_core->m_stats.mainChId][1]/2*10;
			else if(unitTrkY > (vcapWH[send_core->m_stats.mainChId][1]/2))
				send_msg.payload.Track_Deviation_Y = vcapWH[send_core->m_stats.mainChId][1]/2*10;
			else
				send_msg.payload.Track_Deviation_Y = (int)(unitTrkY*10.0);

			//printf("jet +++ Trk=%d, unitTrkXY=(%f,%f),xy=(%f,%f),axis=(%f,%f),sendXY=(%d,%d)\n",send_msg.payload.Self_Tr_S.track_state,unitTrkX,unitTrkY,send_core->m_stats.trackPos.x,send_core->m_stats.trackPos.y,send_core->m_stats.chn[send_core->m_stats.mainChId].axis.x,send_core->m_stats.chn[send_core->m_stats.mainChId].axis.y,send_msg.payload.Track_Deviation_X,send_msg.payload.Track_Deviation_Y);
		}
		else
		{
			send_msg.payload.Self_Tr_S.track_state = Track_State_NA;
			send_msg.payload.Track_Deviation_X = 0x00;
			send_msg.payload.Track_Deviation_Y = 0x00;
		}

		SetReturnPack(&send_msg);
	}
}

static void detectVideo_timer(UInt32 elapsedTime)
{
	app_getvideostatus();

}
void set_Timer()
{
	CR_setTimer(RECV_MSG_TIMER, RECV_MSG_TICKS, recvmsg_Sem);
	CR_setTimer(SEND_MSG_TIMER, SEND_MSG_TICKS, sendmsg_Sem);
	CR_setTimer(DETECT_VIDEO_TIMER, DETECT_VIDEO_TICKS, detectVideo_timer);

}
void uart_thrCreat()
{
	uart_tsk_obj tsk;

	set_Timer();
		
	app_getCoredata();
	
	InitIPCModule();

	OSA_semCreate(&recvmsgSem, 1, 0);
	OSA_semCreate(&sendthrSem, 1, 0);
	
	OSA_thrCreate(&tsk.thrHandleDataIn,uart_dataRecv, 2, 0, &tsk);

	OSA_thrCreate(&tsk.thrHandleDataIn,uart_dataSend, 2, 0, &tsk);
}

