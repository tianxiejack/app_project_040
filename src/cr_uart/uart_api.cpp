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

int EncTransLevel,videoTransMode;

void recvmsg_040(IPC_msg data)
{
	static bool b_secondTrk=false;
	static bool b_mmtTrk=false;
	static bool b_saveAxis = false;
	unsigned int selectOk = 0;
	unsigned char TimeSelect = 0;
	unsigned char Time_Hour,Time_Minute,Time_Second;
	static unsigned short secondTrk_XBak,secondTrk_YBak;
	
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

	psendOsd->osd_core.osd_enZoom = pMsg->enZoomx = data.payload.MulTarget_Num.TV_SizeChage;
	app_ctrl_setZoom(pMsg);

	//printf("jet +++ TrkWin=%d\n",data.payload.MulTarget_Num.bomen_size);
	
	psendOsd->osd_core.osd_WorkMode = data.payload.Work_Pattern.cur_mode;
	switch(data.payload.Work_Pattern.cur_mode)
	{
		case Current_Mode_Lock:
			pMsg->enTrk = false;
			psendOsd->osd_core.osd_enSecondTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Collect:
			pMsg->enTrk = false;
			psendOsd->osd_core.osd_enSecondTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Manual:

			pMsg->enTrk = false;
			psendOsd->osd_core.osd_enSecondTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Scan:
			pMsg->enTrk = false;
			psendOsd->osd_core.osd_enSecondTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		case Current_Mode_Tracking:	
			pMsg->enTrk = true;

			app_ctrl_setTrkStat(pMsg);
			
			if(b_secondTrk)
			{
				psendOsd->osd_core.osd_enSecondTrk = false;
				b_secondTrk = false;

				pMsg->secondTrk_X = secondTrk_XBak;//(data.payload.Track_Search.HighOrder_Hor <<8)|data.payload.Track_Search.LowOrder_Hor;
				pMsg->secondTrk_Y = secondTrk_YBak;//(data.payload.Track_Search.HighOrder_Ver <<8)|data.payload.Track_Search.LowOrder_Ver;
				
				app_ctrl_setSearchTrk(pMsg);
			}
			else if(b_mmtTrk)
			{
				b_mmtTrk = false;

				pMsg->nTarget_index = data.payload.MulTarget_Num.Mul_Number;
				app_ctrl_setMMTTrk(pMsg);
			}

			pMsg->TrkWinSize = (int)data.payload.MulTarget_Num.bomen_size;
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
			
			if(psendOsd->osd_core.osd_secondTrk_X>(vcapWH[pMsg->chId][0]-50))
				secondTrk_XBak = psendOsd->osd_core.osd_secondTrk_X = (vcapWH[pMsg->chId][0]-50);
			else if(psendOsd->osd_core.osd_secondTrk_X<50)
				secondTrk_XBak = psendOsd->osd_core.osd_secondTrk_X = 50;
			else
				secondTrk_XBak = psendOsd->osd_core.osd_secondTrk_X = (data.payload.Track_Search.HighOrder_Hor <<8)|data.payload.Track_Search.LowOrder_Hor;

			if(psendOsd->osd_core.osd_secondTrk_Y>(vcapWH[pMsg->chId][1]-30))
				secondTrk_YBak = psendOsd->osd_core.osd_secondTrk_Y = (vcapWH[pMsg->chId][1]-30);
			else if(psendOsd->osd_core.osd_secondTrk_Y<30)
				secondTrk_YBak = psendOsd->osd_core.osd_secondTrk_Y = 30;
			else
				secondTrk_YBak = psendOsd->osd_core.osd_secondTrk_Y = (data.payload.Track_Search.HighOrder_Ver <<8)|data.payload.Track_Search.LowOrder_Ver;

			//printf("jet +++ W=%02x,secondTrk_XY=(%d,%d)\n", (data.payload.Track_Search.HighOrder_Hor <<8)|data.payload.Track_Search.LowOrder_Hor,secondTrk_XBak,secondTrk_YBak);
			break;
		case Current_Mode_Geografia_Search:
			pMsg->enTrk = false;
			psendOsd->osd_core.osd_enSecondTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;		
		case Current_Mode_Serve:
			pMsg->enTrk = false;
			psendOsd->osd_core.osd_enSecondTrk = false;
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
			psendOsd->osd_core.osd_enSecondTrk = false;
			app_ctrl_setTrkStat(pMsg);
			break;
		default:
			break;
	}

	//printf("jet +++ enMMT=%d\n",data.payload.Track_Search.MulTarget_Detect);
	pMsg->enMTD = data.payload.Track_Search.MulTarget_Detect;
	if(pMsg->enMTD && (data.payload.Work_Pattern.cur_mode!=Current_Mode_Tracking))// && (data.payload.Work_Pattern.cur_mode!=Current_Mode_Track_Search))
	{
		app_ctrl_setMMT(pMsg);
		
		pMsg->nTarget_index = data.payload.MulTarget_Num.Mul_Number;
		if((pMsg->nTarget_index<9)&&(pMsg->nTarget_index>0))
		{
			selectOk = app_ctrl_setMmtSelect(pMsg);
			if(selectOk == 0x01)
			{
				//OSA_printf("jet +++ pMsg->nTarget_index=%d\n",pMsg->nTarget_index);
				send_msg.payload.Self_Tr_S.mul_target_AVT = Multi_Target_AVT_YES;
				b_mmtTrk = true;
			}
			else
				send_msg.payload.Self_Tr_S.mul_target_AVT = Multi_Target_AVT_NO;
		}
	}
	else
	{
		pMsg->enMTD = false;
		app_ctrl_setMMT(pMsg);
		send_msg.payload.Self_Tr_S.mul_target_AVT = Multi_Target_AVT_NA;
	}

	//*****************422Video start*****************//
		
	if(data.payload.Video_C.interface_rate==0x01)
	{
		pMsg->EncTransLevel = 0x00;
		EncTransLevel = 0x01;
	}
	else if(data.payload.Video_C.interface_rate==0x02)
	{
		pMsg->EncTransLevel = 0x01;
		EncTransLevel = 0x02;
	}
	else if(data.payload.Video_C.interface_rate==0x03)
	{
		pMsg->EncTransLevel = 0x02;
		EncTransLevel = 0x03;
	}
	else
	{
		pMsg->EncTransLevel = 0x01;
		EncTransLevel = 0x02;
	}
	
	app_ctrl_set422Rate(pMsg);
	
	videoTransMode = pMsg->videoTransMode = data.payload.Video_C.video_transfer;
	app_ctrl_set422TransMode(pMsg);

	//OSA_printf("jet +++ pMsg->EncTransLevel=%d,rate=%d,transmode=%d,EncTransLevel=%d,videoTransMode=%d\n",pMsg->EncTransLevel,data.payload.Video_C.interface_rate,data.payload.Video_C.video_transfer,EncTransLevel,videoTransMode);

	//*****************422Video end*****************//


	//*****************osd start*****************//
	
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
	psendOsd->osd_core.osd_enLA = (bool)data.payload.Laser_Mes.laser_mes_d;
	psendOsd->osd_core.osd_laserCode = data.payload.Laser_Mes.laser_code;

	//printf("jet +++ tvFov=%d,frFov=%d\n",data.payload.Sensor_Tv_Angle,data.payload.Sensor_Thermal_Angle);
	psendOsd->osd_core.osd_TVFovAngle = data.payload.Sensor_Tv_Angle;
	psendOsd->osd_core.osd_FRFovAngle = data.payload.Sensor_Thermal_Angle;
	psendOsd->osd_core.osd_LMC = data.payload.Work_Pattern.LMC;	
	psendOsd->osd_core.osd_panAngle = data.payload.TurretAngle_Hor;
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

	//*****************osd end*****************//
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

	send_msg.payload.Self_Tr_S.video_input = (Video_Input)unitFaultStat;
	if(unitFaultStat==0x00)
		send_msg.payload.Self_Tr_S.self_state = Self_State_Normal;
	else
		send_msg.payload.Self_Tr_S.self_state = Self_State_Abnomal;
	
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

		/*************************************************************/
		//reply curVideoStat,curAxis_XY,sync422 rate,sync422 videoTransMode
		send_msg.payload.current_video = (Current_Video)send_core->m_stats.mainChId;				
		send_msg.payload.Senser_Aim_CurX = (int)send_core->m_stats.chn[send_core->m_stats.mainChId].axis.x;
		send_msg.payload.Senser_Aim_CurY = (int)send_core->m_stats.chn[send_core->m_stats.mainChId].axis.y;
		send_msg.payload.r_interface_rate = (Interface_Rate)EncTransLevel;
		send_msg.payload.Compress_Transfer = (Video_Transfer)videoTransMode;
		/*************************************************************/

		/*************************************************************/
		//reply Trk_XY,Trk_state
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
				
				if((OSA_getCurTimeInMsec()-itrkTime)>ASSI_TIME)// assi time
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
		/*************************************************************/
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

