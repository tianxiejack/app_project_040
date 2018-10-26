20180830:
	钟；基于dss_xgs021裁剪，通过021硬件基础测试；保留：采集、显示、跟踪基础代码，增加t18通信协议

	
**************请保持该文件持续更新**************

**************20181018-王跟杰**************
1、增加串口接收发送模块
	创建接收和发送线程uart_dataRecv和uart_data_Send，
	其中接收线程等待定时器定时40ms发送信号再做消息处理OSA_semWait(&recvmsgSem,OSA_TIMEOUT_FOREVER);
	发送线程等待定时器定时33ms发送信号再做消息处理OSA_semWait(&sendmsgSem,OSA_TIMEOUT_FOREVER)。
2、增加定时器模块
	程序执行时创建CR_createTimer(10);设置定时器如：CR_setTimer(RECV_MSG_TIMER,RECV_MSG_TICKS,recvmsg_Sem)。
3、增加字符叠加模块
	创建osd_graph_init();设置osd刷新频率在processFrame_core_com(int cap_chid, unsigned char *src, struct v4l2_buffer capInfo, int format)。
4、增加globalData.cpp/.h文件
	为osd叠加模块和消息处理模块传递相关参数，实时处理。
5、增加Hard_Init();
	为检测视频有无提供转换接口，给消息处理模块提供。

调试记录：
1、osd叠加模块，显示字符的颜色必须时cvScalar(255),非255的都不能显示。
2、App_init_graphic_parms(int chId, int winId, void *pPrm)为osd叠加时参数初始化；App_text_genContext(int chId, int winId, char *str, int strBuffLen, Int16 *textLen)为具体叠加内容。

**************20181025-王跟杰**************
调试记录：
1、多目标处理流程：多目标开启时，上位机发送多目标的标号，通过多目标某一标号是否有效反馈给上位机，由上位机决定跟踪与否；
									  使能多目标开关app_ctrl_setMMT；判断多目标某一标号是否有效app_ctrl_setMmtSelect；进入多目标跟踪app_ctrl_setMMTTrk。
2、结构体OSD_CMD初始化操作	，在globalData.cpp中memset(&osd_core,0,sizeof(osd_core))。
3、同步422视频传输方式：选择某一通道是否使能编码来决定单传、同传操作app_ctrl_set422TransMode。
4、同步422视频速率切换：调用内部函数setEncTransLevel实现2、4、8M速率切换，参数0=2M，1=4M，2=8M。							  

**************XXXXXXXX**************
