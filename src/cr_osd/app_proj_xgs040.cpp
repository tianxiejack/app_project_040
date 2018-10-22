/*****************************************************************************
 * Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
 * FileName:        APP_proj_xgs021.cpp
 * Author:          aloysa
 * Date:            2017-08-01
 * Description:     // description
 * Version:         // version
 * Function List:   // funs and description
 * History:         // histroy modfiy record
 *     <author>  <time>   <version >   <desc>
 *
*****************************************************************************/

#include "osa.h"
#include "osd_graph.h"
#include "app_proj_xgs040.h"
#include "globalData.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include "crCore.hpp"

#if 1
/***************************************
		osd and graph part
 ***************************************/
#define SHMEM_LEN 128

#define SYMBOL_longitude_d	128
#define SYMBOL_longitude_f	129
#define SYMBOL_longitude_m	130
#define SYMBOL_degress	128
#define SYMBOL_lager_star	132

static int lineTopPosY[4] = {20, 50, 80, 110};
static int lineBotPosY[4] = {1030, 1000, 970, 940};
static char gSysStr[3][8] = {"N/A ", "MBIT", "IBIT"};
static char gWkStr[12][10] = {"IBIT", "CAG ", "STO ", "MAN ", "SCN ", "TRA ", "STR ", "SLA ", "    ", "SER ", "    ", "IBIT"};

CGlobalData *precvOsd_text = CGlobalData::getInstance();

ICore_1001 *xgs040_core = NULL;

static unsigned int laserCnt = 0;

typedef enum
{
    eTextId_sensor  = 0x00,
    eTextId_sysTime     = 0x01,
    eTextId_fov,
    eTextId_enh,
    eTextId_workMode,
    eTextId_laserDistance,
    eTextId_laserStat,
    eTextId_laserCode,
    eTextId_axisX,
    eTextId_axisY,
    eTextId_PTAngle = 0x0a,
    eTextId_enLASER,
    eTextId_ZOOM,
    eTextId_LMC,
    eTextId_radiate,
    eTextId_radiateCnt,
    eTextId_laserZhaoShe,

    eTextId_max
} osdCR_TextId;

typedef struct
{
    unsigned int id;

    unsigned int OrgX;
    unsigned int OrgY;
} osdCR_TextItem;


osdCR_TextItem  g_osdCRTextPos[2][eTextId_max] =
{
    // channel TV
    {
        {eTextId_sensor, 50, 50},
        {eTextId_sysTime, 1700, 50},
        {eTextId_fov, 860, 1000},
        {eTextId_enh, 1200, 1000},
        {eTextId_workMode, 800, 1000},
        {eTextId_laserDistance, 50, 970},
        {eTextId_laserStat, 50, 940},
        {eTextId_laserCode, 50, 1000},
        {eTextId_axisX, 1700, 500},
        {eTextId_axisY, 1700, 530},
        {eTextId_PTAngle, 100, 50},
        {eTextId_enLASER, 150, 940},
        {eTextId_ZOOM, 300, 50},
        {eTextId_LMC, 1280, 1000},
        {eTextId_radiate, 50, 500},
        {eTextId_radiateCnt, 50, 520},
        {eTextId_laserZhaoShe, 130, 940},
    },
    // channel FR
    {

        {eTextId_sensor, 40, 40},
        {eTextId_sysTime, 1150, 40},
        {eTextId_fov, 560, 964},
        {eTextId_enh, 810, 964},
        {eTextId_workMode, 500, 964},
        {eTextId_laserDistance, 40, 944},
        {eTextId_laserStat, 40, 924},
        {eTextId_laserCode, 40, 964},
        {eTextId_axisX, 1150, 500},
        {eTextId_axisY, 1150, 520},
        {eTextId_PTAngle, 80, 40},
        {eTextId_enLASER, 120, 924},
        {eTextId_ZOOM, 240, 40},
        {eTextId_LMC, 860, 964},
        {eTextId_radiate, 40, 480},
        {eTextId_radiateCnt, 40, 500},
        {eTextId_laserZhaoShe, 100, 924}
    },
};

Int16 APP_text_genContext(int chId, int winId, char * str, int strBufLen, Int16 *textLen)
{
	char *pStr = str;
	Int16 lineStrLen = 0, strLen = 0;
	int i;
	int tmpAngle, tmpAltitude;
	int tmpD, tmpF, tmpM;
	float tmpData = 0.0;
	char osdStr[32] = {'\0'};

	OSA_assert((str != NULL) && (strBufLen > 0) && (textLen != NULL));
	OSA_assert(winId < GRPX_WINID_MAX);
	

	switch(winId)
	{
	case GRPX_WINID_SYS_TIMER_TEXT:
		{
			int hour, minute, secord;

			hour = (precvOsd_text->osd_core.osd_Time>>16)&0xFF;
			minute = (precvOsd_text->osd_core.osd_Time>>8)&0xFF;
			secord = (precvOsd_text->osd_core.osd_Time&0xFF);

			//printf("jet +++ time %02d:%02d:%02d,time=%d\n",hour,minute,secord,precvOsd_text->osd_core.osd_Time);
			sprintf(pStr, "%02d:%02d:%02d", hour, minute, secord);
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			/*
			if(1)
				sprintf(pStr, "NO SIGNAL");
			else
				sprintf(pStr, "         ");
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			sprintf(pStr, "LMC");
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	case GRPX_WINID_SYS_MODE_TEXT:
		{

			sprintf(pStr, "%s", gWkStr[precvOsd_text->osd_core.osd_WorkMode]);

			lineStrLen = textLen[0];
			pStr += lineStrLen;

			sprintf(pStr, "(%4.1f,%4.1f)  ", (float)(precvOsd_text->osd_core.osd_panAngle/100.0), (float)(precvOsd_text->osd_core.osd_tiltAngle/100.0));
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			/*
			tmpAltitude = 2;
			if(0)
				sprintf(pStr, "         ");
			else if(tmpAltitude >= 0)
				sprintf(pStr, "LMC:+%04d", tmpAltitude/2);
			else
				sprintf(pStr, "LMC:%05d", tmpAltitude/2);
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	case GRPX_WINID_SENSOR_TEXT:
		{
			if(chId == 0)
				sprintf(pStr, "TV");
			else if(chId == 1)
				sprintf(pStr, "FR");
			lineStrLen = textLen[0];
			pStr += lineStrLen;
			if(chId == 0)
				sprintf(pStr, "(%4.1f,%4.1f)  ", (precvOsd_text->osd_core.osd_TVFovAngle/10.0),(precvOsd_text->osd_core.osd_TVFovAngle/10.0*0.5625));
			else if(chId == 1)
				sprintf(pStr, "(%4.1f,%4.1f)  ", (precvOsd_text->osd_core.osd_FRFovAngle/10.0),(precvOsd_text->osd_core.osd_FRFovAngle/10.0*0.8));
			
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			
			if(precvOsd_text->osd_core.osd_enZoom||precvOsd_text->osd_core.osd_dispZoom)
				sprintf(pStr, "2X");
			else
				sprintf(pStr, "  ");
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			
		}
		break;

	case GRPX_WINID_ALG_MARK_TEXT:
		{
			/*
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			if(1)
				sprintf(pStr, "IE");
			else
				sprintf(pStr, "  ");
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			if(1)
				sprintf(pStr, "2X");
			else
				sprintf(pStr, "  ");
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	case GRPX_WINID_PLAT_ANGLE_TEXT:
		{
			/*
	       	int tmpx = 2;
			if(tmpx >= 0)
				sprintf(pStr, "AZ:+%06.2f", (float)tmpx /100.0);
			else
				sprintf(pStr, "AZ:%07.2f", (float)tmpx/100.0);
			lineStrLen = textLen[0];
			pStr += lineStrLen;

	       	int tmpy = 2;
			if(tmpy >= 0)
				sprintf(pStr, "EL:+%06.2f", (float)tmpy/100.0);
			else
				sprintf(pStr, "EL:%07.2f", (float)tmpy/100.0);
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	case GRPX_WINID_ORIENTATE_TEXT:
		{
			/*
			#if 0
			// "LP N:000d00f00m E:000d00f00m H:+0000"
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			// "PP N:000d00f00m E:000d00f00m H:+0000"
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			// "WP N:000d00f00m E:000d00f00m H:+0000"
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			#endif
			if(1)
			{
				for(i=1; i<7; i+=2)
				{
					// [0][1][2]
					memset(osdStr, '\0', sizeof(osdStr));
					if(i/2 == 0)
						sprintf(osdStr, "LP ");
					else if(i/2 == 1)
						sprintf(osdStr, "PP ");
					else if(i/2 == 2)
						sprintf(osdStr, "WP ");
					else
						sprintf(osdStr, "   ");
					memcpy(pStr, osdStr, strlen(osdStr));
					pStr[8] 	= SYMBOL_longitude_d;
					pStr[11] 	= SYMBOL_longitude_f;
					pStr[14] 	= SYMBOL_longitude_m;
					pStr[21] 	= SYMBOL_longitude_d;
					pStr[24] 	= SYMBOL_longitude_f;
					pStr[27] 	= SYMBOL_longitude_m;
				
					// [1][3][5] E:000d00f00m
					tmpAngle = 2;
					tmpData = (float)((float)tmpAngle/10000000.0 - tmpAngle/10000000);
					tmpD = abs(tmpAngle/10000000);
					tmpF = abs(tmpData*60.0);
					tmpM = abs((tmpData*60.0 - (int)(tmpData*60.0))*60.0);
					if(tmpM == 60){
						tmpM = 0;
						tmpF += 1;
					}
					if(tmpF == 60){
						tmpF = 0;
						tmpD += 1;
					}
					memset(osdStr, '\0', sizeof(osdStr));
					if(tmpAngle > 0){
						sprintf(osdStr, "E:%03d", tmpD);
					}else{
						sprintf(osdStr, "W:%03d", tmpD);
					}
					memcpy((pStr+16), osdStr, strlen(osdStr));
					memset(osdStr, '\0', sizeof(osdStr));
					sprintf(osdStr, "%02d", tmpF);
					memcpy((pStr+22), osdStr, strlen(osdStr));
					memset(osdStr, '\0', sizeof(osdStr));
					sprintf(osdStr, "%02d", tmpM);
					memcpy((pStr+25), osdStr, strlen(osdStr));

					// [2][4][6] N:000d00f00m
					tmpAngle = 2;
					tmpData = (float)((float)tmpAngle/10000000.0 - tmpAngle/10000000);
					tmpD = abs(tmpAngle/10000000);
					tmpF = abs(tmpData*60.0);
					tmpM = abs((tmpData*60.0 - (int)(tmpData*60.0))*60.0);
					if(tmpM == 60){
						tmpM = 0;
						tmpF += 1;
					}
					if(tmpF == 60){
						tmpF = 0;
						tmpD += 1;
					}
					if(tmpAngle > 0){
						sprintf(osdStr, "N:%03d", tmpD);
					}else{
						sprintf(osdStr, "S:%03d", tmpD);
					}
					memcpy((pStr+3), osdStr, strlen(osdStr));
					memset(osdStr, '\0', sizeof(osdStr));
					sprintf(osdStr, "%02d", tmpF);
					memcpy((pStr+9), osdStr, strlen(osdStr));
					memset(osdStr, '\0', sizeof(osdStr));
					sprintf(osdStr, "%02d", tmpM);
					memcpy((pStr+12), osdStr, strlen(osdStr));

					// [1][2][3] H:+0000
					tmpAltitude = 2;
					memset(osdStr, '\0', sizeof(osdStr));
					if(tmpAltitude > 0)
						sprintf(osdStr, "H:+%04d", tmpAltitude/2);
					else
						sprintf(osdStr, "H:%05d", tmpAltitude/2);
					memcpy((pStr+29), osdStr, strlen(osdStr));

					// [0][1][2]
					lineStrLen = textLen[i/2];
					pStr += lineStrLen;
				}
			}
			else
			{
				for(i=1; i<7; i+=2)
				{
					sprintf(pStr, "                                                 ");

					// [0][1][2]
					lineStrLen = textLen[i/2];
					pStr += lineStrLen;
				}
			}

			*/
		}
		break;

	case GRPX_WINID_LASER_STAT_TEXT:
		{
		        if(precvOsd_text->osd_core.osd_enLaser==0)
				sprintf(pStr,"OFF ");
		        else
		        {
				switch(precvOsd_text->osd_core.osd_laserStat)
				{
					case 0:
						sprintf(pStr,"ON  ");
						break;
					case 1:					
						sprintf(pStr,"STB ");
						break;
					case 2:

						sprintf(pStr,"SIG ");
						break;
					case 3:
						sprintf(pStr,"LRC1");
						break;
					case 4:
						sprintf(pStr,"LRC5");
						break;
					case 5:
						sprintf(pStr,"LD15");
						break;
					case 6:
						sprintf(pStr,"LD45");
						break;						
					default:
				
						break;
				}								
			}
				
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			sprintf(pStr, "D:%05d",precvOsd_text->osd_core.osd_Distance);
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			sprintf(pStr, "C %02d",precvOsd_text->osd_core.osd_laserCode);
			//pStr[3] = 130;
			//pStr[2] = 129;
			//pStr[1] = 128;
			//pStr[0] = 127;
			lineStrLen = textLen[2];
			pStr += lineStrLen;
		}
		break;

	case GRPX_WINID_LASER_WORK_TEXT:
		{
			/*
			if(1)
				sprintf(pStr, "D:  N/A");
			else
				sprintf(pStr, "D:xx");
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			if(0){
				sprintf(pStr, "    ");
			}else if(1){
				sprintf(pStr, "STOP");
			}else if(0){
				sprintf(pStr, "STB ");
			}else if(0){
				sprintf(pStr, "LRS ");
			}else if(0){
				sprintf(pStr, "LRC ");
			}else if(0){
				sprintf(pStr, "MARK");
			}else if(0){
				sprintf(pStr, "LD  ");
			}
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			sprintf(pStr, "C:8");
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	case GRPX_WINID_LASER_COUNT_TEXT:
		{
			/*
			if(1)
				sprintf(pStr, "TARGET ==");
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			sprintf(pStr, "23s");
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	case GRPX_WINID_POSE_ANGLE_TEXT:
		{
			/*
			int strPos[4][3] = {{0,0,0}, {3,8,11}, {21,26,29}, {38,43,46}};
			
			if(1)
			{
				//sprintf(pStr, "AZ:+000d00f00m PITCH:+000d00f00m ROLL:+000d00f00m");
				memset(osdStr, '\0', sizeof(osdStr));
				sprintf(osdStr, "AZ:");
				memcpy(pStr, osdStr, strlen(osdStr));
				sprintf(osdStr, "PITCH:");
				memcpy((pStr+15), osdStr, strlen(osdStr));
				sprintf(osdStr, "ROLL:");
				memcpy((pStr+33), osdStr, strlen(osdStr));
				pStr[7]  = SYMBOL_longitude_d;
				pStr[10] = SYMBOL_longitude_f;
				pStr[13] = SYMBOL_longitude_m;
				pStr[25] = SYMBOL_longitude_d;
				pStr[28] = SYMBOL_longitude_f;
				pStr[31] = SYMBOL_longitude_m;
				pStr[42] = SYMBOL_longitude_d;
				pStr[45] = SYMBOL_longitude_f;
				pStr[48] = SYMBOL_longitude_m;
			
				for(i=1; i<4; i++)
				{
					// [1]AZ [2]PITCH [3]ROLL
					tmpAngle = 2;
					memset(osdStr, '\0', sizeof(osdStr));
					if(tmpAngle > 0){
						sprintf(osdStr, "+%03d", tmpAngle/10000);
					}else{
						sprintf(osdStr, "%04d", tmpAngle/10000);
					}
					memcpy((pStr+strPos[i][0]), osdStr, strlen(osdStr));

					tmpData = (float)((float)tmpAngle/10000.0 - tmpAngle/10000);
					memset(osdStr, '\0', sizeof(osdStr));
					sprintf(osdStr, "%02d", (int)(abs(tmpData)*60.0));
					memcpy((pStr+strPos[i][1]), osdStr, strlen(osdStr));

					tmpData = tmpData*60.0 - (int)(tmpData*60.0);
					memset(osdStr, '\0', sizeof(osdStr));
					sprintf(osdStr, "%02d", (int)(abs(tmpData)*60.0));
					memcpy((pStr+strPos[i][2]), osdStr, strlen(osdStr));
				}
			}
			else
				sprintf(pStr, "                                                 ");
			lineStrLen = textLen[0];
			pStr += lineStrLen;

	   		if(1)
	   			sprintf(pStr, "IMU_OK ");
	   		else
	   			sprintf(pStr, "IMU_PRE");
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			if(1)
	   			sprintf(pStr, "WARM");
			else
	   			sprintf(pStr, "    ");
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	case GRPX_WINID_INFO_MSG_TEXT:
		{			

			if(precvOsd_text->osd_core.osd_enEnh||precvOsd_text->osd_core.osd_dispFREnh||precvOsd_text->osd_core.osd_dispTVEnh)
				sprintf((pStr), "IE");
			else
				sprintf((pStr), "  ");
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			if(precvOsd_text->osd_core.osd_LMC)
				sprintf(pStr,"LMC");
			else
				sprintf(pStr,"   ");
			lineStrLen = textLen[1];
			pStr += lineStrLen;
			
			if(precvOsd_text->osd_core.osd_enLA)
				sprintf(pStr, "LA");//pStr[0] = SYMBOL_lager_star;
			else
				sprintf(pStr, "  ");
			lineStrLen = textLen[2];
			pStr += lineStrLen;			
		}
		break;

	case GRPX_WINID_CROSS_TEXT:
		{
			if(laserCnt > 0xFFFFFFFF)
				laserCnt = 0;
			laserCnt ++;
			
			if(precvOsd_text->osd_core.osd_AxisMode)
				sprintf(pStr, "Bx:%d", (int)xgs040_core->m_stats.chn[xgs040_core->m_stats.mainChId].axis.x);
			else
				sprintf(pStr, "       ");
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			if(precvOsd_text->osd_core.osd_AxisMode)
				sprintf(pStr, "By:%d", (int)xgs040_core->m_stats.chn[xgs040_core->m_stats.mainChId].axis.y);
			else
				sprintf(pStr, "       ");
			lineStrLen = textLen[1];
			pStr += lineStrLen;
			
			if((precvOsd_text->osd_core.osd_laserStat==0x03)||(precvOsd_text->osd_core.osd_laserStat==0x04)||(precvOsd_text->osd_core.osd_laserStat==0x05)||(precvOsd_text->osd_core.osd_laserStat==0x06))
			{
				if((laserCnt%16)<=7)
					sprintf(pStr, "*");
				else
					sprintf(pStr, " ");
			}
			else
				sprintf(pStr, " ");
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			
		}
		break;

	case GRPX_WINID_TRKRTS_TEXT:
		{
			/*
			// "    :X(0000.0) Y(0000.0)"
			if(1){
				pStr[1] = 'L';
			}
			
			if(1){
				pStr[0] = 'U';
			}
		

			if(1){

					sprintf((pStr+3), " :X(0000.0) Y(0000.0)");
			}
			else
			{
				sprintf((pStr+3), "                     ");
			}
			lineStrLen = textLen[0];
			pStr += lineStrLen;

			if(1)
			{
				sprintf(pStr, "SAVEAXIS");
				
			}
			else
				sprintf(pStr, "        ");
			lineStrLen = textLen[1];
			pStr += lineStrLen;

			if(1)
			{
				if(1)
					sprintf(pStr, "TV-C");
				
			}
			else
				sprintf(pStr, "    ");
			lineStrLen = textLen[2];
			pStr += lineStrLen;
			*/
		}
		break;

	default:
		break;
	}
	OSA_assert(strBufLen >= strLen);

	return 0;
}

int APP_init_graphic_parms(int chId, int winId, void *pPrm)
{
	Text_Param_fb * textParam = (Text_Param_fb *)pPrm;
	Line_Param_fb * lineParam = (Line_Param_fb *)pPrm;
	Rect_Param_fb * rectParam = (Rect_Param_fb *)pPrm;
	OSA_assert(textParam != NULL);

	xgs040_core = (ICore_1001 *)ICore::Qury(COREID_1001);

	textParam->enableWin	= 1;
	textParam->frcolor 	= ecolor_White;
	switch(winId)
	{
	case GRPX_WINID_SYS_TIMER_TEXT:
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= g_osdCRTextPos[chId][eTextId_sysTime].OrgX;
		textParam->text_y[0] 	= g_osdCRTextPos[chId][eTextId_sysTime].OrgY;
		textParam->textLen[0] 	= 8;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr), "00:00:00");

		/*
		textParam->text_x[1] 		= g_osdCRTextPos[chId][eTextId_fov].OrgX;
		textParam->text_y[1] 		= g_osdCRTextPos[chId][eTextId_fov].OrgY;
		textParam->textLen[1] 		= 9;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"NO SIGNAL");

		textParam->text_x[2] 		= g_osdCRTextPos[chId][eTextId_enh].OrgX;
		textParam->text_y[2] 		= g_osdCRTextPos[chId][eTextId_enh].OrgY;
		textParam->textLen[2] 		= 5;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"   00");	//  scale
		*/
		break;

	case GRPX_WINID_SYS_MODE_TEXT:
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= g_osdCRTextPos[chId][eTextId_workMode].OrgX;
		textParam->text_y[0] 	= g_osdCRTextPos[chId][eTextId_workMode].OrgY;
		textParam->textLen[0] 	= 5;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"IBIT");

		textParam->text_x[1] 	= g_osdCRTextPos[chId][eTextId_fov].OrgX;
		textParam->text_y[1] 	= g_osdCRTextPos[chId][eTextId_fov].OrgY;
		textParam->textLen[1] 	= 20;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"(0000.0,0000.0)");	// pan tilt angle

		/*
		textParam->text_x[2] 	= 30;
		textParam->text_y[2] 	= lineTopPosY[2];
		textParam->textLen[2] 	= 9;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"LMC:+0000");
		*/
		break;

	case GRPX_WINID_SENSOR_TEXT:
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= g_osdCRTextPos[chId][eTextId_sensor].OrgX;
		textParam->text_y[0] 	= g_osdCRTextPos[chId][eTextId_sensor].OrgY;
		textParam->textLen[0] 	= 2;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"TV");

		textParam->text_x[1] 	= g_osdCRTextPos[chId][eTextId_PTAngle].OrgX;
		textParam->text_y[1] 	= g_osdCRTextPos[chId][eTextId_PTAngle].OrgY;
		textParam->textLen[1] 	= 20;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"(0000.0,0000.0)");

		
		textParam->text_x[2] 	= g_osdCRTextPos[chId][eTextId_ZOOM].OrgX;
		textParam->text_y[2] 	= g_osdCRTextPos[chId][eTextId_ZOOM].OrgY;
		textParam->textLen[2] 	= 2;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"2X");
		
		break;

	case GRPX_WINID_ALG_MARK_TEXT:
		/*
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= 0;
		textParam->text_y[0] 	= 0;
		textParam->textLen[0] 	= 0;	// no use
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr), "IM");

		textParam->text_x[1] 	= 292;
		textParam->text_y[1] 	= lineTopPosY[1];
		textParam->textLen[1] 	= 2;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"IE");

		textParam->text_x[2] 	= 244;
		textParam->text_y[2] 	= lineTopPosY[1];
		textParam->textLen[2] 	= 2;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"2X");
		*/
		break;

	case GRPX_WINID_PLAT_ANGLE_TEXT:
		/*
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= 340;
		textParam->text_y[0] 	= lineTopPosY[0];
		textParam->textLen[0] 	= 10;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"AZ:+000.00");

		textParam->text_x[1] 	= 340;
		textParam->text_y[1] 	= lineTopPosY[1];
		textParam->textLen[1] 	= 10;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"EL:-000.00");

		textParam->text_x[2] 	= 0;
		textParam->text_y[2] 	= 0;
		textParam->textLen[2] 	= 0;	// no use
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1]," ");
		*/
		break;

	case GRPX_WINID_ORIENTATE_TEXT:
		/*
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= 20;
		textParam->text_y[0] 	= lineBotPosY[3];
		textParam->textLen[0] 	= 36;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)textParam->textAddr,"LP N:000d00f00m E:000d00f00m H:+0000");
		*((char*)(textParam->textAddr)+8) 	= SYMBOL_longitude_d;
		*((char*)(textParam->textAddr)+11) 	= SYMBOL_longitude_f;
		*((char*)(textParam->textAddr)+14) 	= SYMBOL_longitude_m;
		*((char*)(textParam->textAddr)+21) 	= SYMBOL_longitude_d;
		*((char*)(textParam->textAddr)+24) 	= SYMBOL_longitude_f;
		*((char*)(textParam->textAddr)+27) 	= SYMBOL_longitude_m;

		textParam->text_x[1] 	= 20;
		textParam->text_y[1] 	= lineBotPosY[1];
		textParam->textLen[1] 	= 36;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]), "PP N:000d00f00m E:000d00f00m H:+0000");
		*(textParam->textAddr+textParam->textLen[0]+8) 	= SYMBOL_longitude_d;
		*(textParam->textAddr+textParam->textLen[0]+11) = SYMBOL_longitude_f;
		*(textParam->textAddr+textParam->textLen[0]+14) = SYMBOL_longitude_m;
		*(textParam->textAddr+textParam->textLen[0]+21) = SYMBOL_longitude_d;
		*(textParam->textAddr+textParam->textLen[0]+24) = SYMBOL_longitude_f;
		*(textParam->textAddr+textParam->textLen[0]+27) = SYMBOL_longitude_m;

		textParam->text_x[2] 	= 20;
		textParam->text_y[2] 	= lineBotPosY[2];
		textParam->textLen[2] 	= 36;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1]),"WP N:000d00f00m E:000d00f00m H:+0000");
		*((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1])+8) = SYMBOL_longitude_d;
		*((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1])+11) = SYMBOL_longitude_f;
		*((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1])+14) = SYMBOL_longitude_m;
		*((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1])+21) = SYMBOL_longitude_d;
		*((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1])+24) = SYMBOL_longitude_f;
		*((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1])+27) = SYMBOL_longitude_m;
		*/
		break;

	case GRPX_WINID_LASER_STAT_TEXT:
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 		= g_osdCRTextPos[chId][eTextId_laserStat].OrgX;
		textParam->text_y[0] 		= g_osdCRTextPos[chId][eTextId_laserStat].OrgY;
		textParam->textLen[0] 		= 5;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"OFF  ");	// laser enable

		textParam->text_x[1] 		= g_osdCRTextPos[chId][eTextId_laserDistance].OrgX;
		textParam->text_y[1] 		= g_osdCRTextPos[chId][eTextId_laserDistance].OrgY;
		textParam->textLen[1] 		= 8;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"D:000000");	// laser power

		textParam->text_x[2] 		= g_osdCRTextPos[chId][eTextId_laserCode].OrgX;
		textParam->text_y[2] 		= g_osdCRTextPos[chId][eTextId_laserCode].OrgY;
		textParam->textLen[2] 		= 4;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"C   ");	// laser temp
		*(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1]+3) = SYMBOL_degress;
		*(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1]+4) = 129;
		break;

	case GRPX_WINID_LASER_WORK_TEXT:
		/*
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 		= 1792;
		textParam->text_y[0] 		= lineBotPosY[1];
		textParam->textLen[0] 		= 7;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"D:00000");	// laser distance

		textParam->text_x[1] 		= 1755;
		textParam->text_y[1] 		= lineBotPosY[0];
		textParam->textLen[1] 		= 4;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"    ");// laser work mode

		textParam->text_x[2] 		= 1840;
		textParam->text_y[2] 		= lineBotPosY[0];
		textParam->textLen[2] 		= 4;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"C:01");	// laser code
		*/
		break;

	case GRPX_WINID_LASER_COUNT_TEXT:
		/*
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 		= g_osdCRTextPos[chId][eTextId_radiate].OrgX;
		textParam->text_y[0] 		= g_osdCRTextPos[chId][eTextId_radiate].OrgY;
		textParam->textLen[0] 		= 9;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"TARGET ==");	// laser time

		
		textParam->text_x[1] 		= g_osdCRTextPos[chId][eTextId_radiateCnt].OrgX;
		textParam->text_y[1] 		= g_osdCRTextPos[chId][eTextId_radiateCnt].OrgY;
		textParam->textLen[1] 		= 3;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"00s");

		
		textParam->text_x[2] 		= 10;
		textParam->text_y[2] 		= lineBotPosY[0];
		textParam->textLen[2] 		= 0;		// no used
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 2);
		//sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"    ");
		*/
		break;

	case GRPX_WINID_POSE_ANGLE_TEXT:
		/*
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 		= 592;
		textParam->text_y[0] 		= lineTopPosY[2];
		textParam->textLen[0] 		= 49;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"AZ:+000d00f00m PITCH:+000d00f00m ROLL:+000d00f00m");
		*((char*)(textParam->textAddr)+7)  = SYMBOL_longitude_d;
		*((char*)(textParam->textAddr)+10) = SYMBOL_longitude_f;
		*((char*)(textParam->textAddr)+13) = SYMBOL_longitude_m;
		*((char*)(textParam->textAddr)+25) = SYMBOL_longitude_d;
		*((char*)(textParam->textAddr)+28) = SYMBOL_longitude_f;
		*((char*)(textParam->textAddr)+31) = SYMBOL_longitude_m;
		*((char*)(textParam->textAddr)+42) = SYMBOL_longitude_d;
		*((char*)(textParam->textAddr)+45) = SYMBOL_longitude_f;
		*((char*)(textParam->textAddr)+48) = SYMBOL_longitude_m;

		textParam->text_x[1] 		= 1792;
		textParam->text_y[1] 		= 970;
		textParam->textLen[1] 		= 7;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"IMU_PRE");

		textParam->text_x[2] 		= 1712;
		textParam->text_y[2] 		= 970;
		textParam->textLen[2] 		= 4;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1]),"WARM");
		*/
		break;

	case GRPX_WINID_INFO_MSG_TEXT:
		
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= g_osdCRTextPos[chId][eTextId_enh].OrgX;
		textParam->text_y[0] 	= g_osdCRTextPos[chId][eTextId_enh].OrgY;
		textParam->textLen[0] 	= 2;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr), "IE");

		textParam->text_x[1]	= g_osdCRTextPos[chId][eTextId_LMC].OrgX;
		textParam->text_y[1]	= g_osdCRTextPos[chId][eTextId_LMC].OrgY;
		textParam->textLen[1]	= 3;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"LMC");

		
		textParam->text_x[2] 	= g_osdCRTextPos[chId][eTextId_enLASER].OrgX;
		textParam->text_y[2] 	= g_osdCRTextPos[chId][eTextId_enLASER].OrgY;
		textParam->textLen[2] 	= 2;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)textParam->textAddr+textParam->textLen[0]+textParam->textLen[1],"LA");	// enablelaser
		
		break;

	case GRPX_WINID_CROSS_TEXT:
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 	= g_osdCRTextPos[chId][eTextId_axisX].OrgX;
		textParam->text_y[0] 	= g_osdCRTextPos[chId][eTextId_axisX].OrgY;
		textParam->textLen[0] 	= 7;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"Bx:0000");

		textParam->text_x[1] 	= g_osdCRTextPos[chId][eTextId_axisY].OrgX;
		textParam->text_y[1] 	= g_osdCRTextPos[chId][eTextId_axisY].OrgY;
		textParam->textLen[1] 	= 7;
		textParam->text_valid 	= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"By:0000");

		
		textParam->text_x[2] 		= g_osdCRTextPos[chId][eTextId_laserZhaoShe].OrgX;
		textParam->text_y[2] 		= g_osdCRTextPos[chId][eTextId_laserZhaoShe].OrgY;
		textParam->textLen[2] 		= 1;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1])," ");
		
		break;

	case GRPX_WINID_TRKRTS_TEXT:
		/*
		textParam->objType 	= grpx_ObjId_Text;

		textParam->text_x[0] 		= 1530;
		textParam->text_y[0] 		= lineBotPosY[3];
		textParam->textLen[0] 		= 24;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 0);
		sprintf((char*)(textParam->textAddr),"    :X(0000.0) Y(0000.0)");

		textParam->text_x[1] 		= 30;
		textParam->text_y[1] 		= 520;
		textParam->textLen[1] 		= 8;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 1);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]),"SAVEAXIS");

		textParam->text_x[2] 		= 1840;
		textParam->text_y[2] 		= 910;
		textParam->textLen[2] 		= 4;
		textParam->text_valid 		= BIT_SET(textParam->text_valid, 2);
		sprintf((char*)(textParam->textAddr+textParam->textLen[0]+textParam->textLen[1]),"    ");	// TV-C/B
		*/
		break;
/*
	case GRPX_WINID_SENSOR_RECT:
		rectParam->objType 	= grpx_ObjId_Rect;
		rectParam->x 		= 80;	// 30+16
		rectParam->y 		= 60;	// 50+20/2-linePixels
		rectParam->width 		= 48;
		rectParam->height 	= 30;
		rectParam->linePixels 	= 2;
		break;

	case GRPX_WINID_FOVMATCH_RECT:
		rectParam->objType 	= grpx_ObjId_Rect;
		rectParam->x 		= 192;
		rectParam->y 		= lineTopPosY[0] - 4;	// lineUpPosY[0] - linePixels - 1
		rectParam->width 		= 64;
		rectParam->height 	= 2;
		rectParam->linePixels 	= 2;
		break;

	case GRPX_WINID_COMPASS:
		rectParam->objType 		= grpx_ObjId_Compass;
		rectParam->x 			= 546;
		rectParam->y 			= lineTopPosY[1] - 4;
		rectParam->width 			= 50;
		rectParam->height 		= 50;
		rectParam->linePixels 		= 2;
		rectParam->lineGapWidth 	= 0;
		rectParam->lineGapHeight 	= 0;
		rectParam->res0    = 0;	// angle
		break;
*/
	case GRPX_WINID_SECONDTRK_CROSS:
		rectParam->objType 	= grpx_ObjId_SecondTrk_Cross;
		rectParam->x 		= 300;	// 30+16
		rectParam->y 		= 500;	// 50+20/2-linePixels
		rectParam->width 		= 80;
		rectParam->height 	= 60;
		rectParam->linePixels 	= 2;
		break;


	default:
		return OSA_EFAIL;
	}

	return OSA_SOK;
}

int APP_set_graphic_parms(int chId, int winId, void *pPrm)
{
	Text_Param_fb * textParam = (Text_Param_fb *)pPrm;
	Line_Param_fb * lineParam = (Line_Param_fb *)pPrm;
	Rect_Param_fb * rectParam = (Rect_Param_fb *)pPrm;
	OSA_assert(textParam != NULL);
	//int grpText = 0, grpRect = 0;
	int grpLevel = 0;

#if 0
	textParam->frcolor		= pIStuts->DispColor[chId];
	//grpText = (pIStuts->DispGrp[chId] & eDisp_show_text)?1:0;			// display text control
	//grpRect = (pIStuts->DispGrp[chId] & eDisp_show_rect)?1:0;			// display rect control
	grpLevel = (pIStuts->DispGrp[chId]);		// display level control
#else
	textParam->frcolor		= 1;
	grpLevel = precvOsd_text->osd_core.osd_enDisp;		// display level control
#endif

	switch(winId)
	{
	case GRPX_WINID_SYS_TIMER_TEXT:
		APP_text_genContext(chId, winId, 
							(char*)(textParam->textAddr), 
							SHMEM_LEN, textParam->textLen);
		if((grpLevel==0x01)||(grpLevel==0x02))
			textParam->text_valid = 0x00;
		else
			textParam->text_valid = 0x07;	
		break;

	case GRPX_WINID_SYS_MODE_TEXT:
	case GRPX_WINID_ALG_MARK_TEXT:
	case GRPX_WINID_SENSOR_TEXT:
	case GRPX_WINID_PLAT_ANGLE_TEXT:
	case GRPX_WINID_LASER_STAT_TEXT:
	case GRPX_WINID_LASER_WORK_TEXT:
	case GRPX_WINID_LASER_COUNT_TEXT:
	case GRPX_WINID_POSE_ANGLE_TEXT:
	case GRPX_WINID_INFO_MSG_TEXT:
	case GRPX_WINID_CROSS_TEXT:
	case GRPX_WINID_TRKRTS_TEXT:
		APP_text_genContext(chId, winId, 
							(char*)(textParam->textAddr), 
							SHMEM_LEN, textParam->textLen);
		if((grpLevel==0x01)||(grpLevel==0x02))
			textParam->text_valid = 0x00;
		else
			textParam->text_valid = 0x07;
		break;

	case GRPX_WINID_ORIENTATE_TEXT:
		APP_text_genContext(chId, winId, 
							(char*)(textParam->textAddr), 
							SHMEM_LEN, textParam->textLen);
		if((grpLevel==0x01)||(grpLevel==0x02))
			textParam->text_valid = 0x00;
		else
			textParam->text_valid = 0x07;
		break;

	/**************************************/
	case GRPX_WINID_SENSOR_RECT:
		if(1){
			//draw TV rect
			rectParam->x			= 80;	// 30+16
			rectParam->width 		= 60;
		}else if(0){
			//draw SWIR rect
			rectParam->x 		= 116;	// 84+16*2
			rectParam->width 		= 80;
		}else if(0){
			//draw MWIR rect
			rectParam->x			= 196;	// 164+16*2
			rectParam->width 		= 80;
		}

		if(grpLevel >= 2)
			rectParam->frcolor		= ecolor_Default;
		break;

	case GRPX_WINID_FOVMATCH_RECT:
		if(1)
			rectParam->frcolor		= ecolor_Default;

		if(grpLevel >= 2)
			rectParam->frcolor		= ecolor_Default;
		break;

	case GRPX_WINID_COMPASS:
		rectParam->res0    = (30)%360;	// angle

		if(grpLevel >= 2)
			rectParam->frcolor		= ecolor_Default;
		break;

	case GRPX_WINID_SECONDTRK_CROSS:
		if(1){
			//draw TV rect
			rectParam->x			= 300;	// 30+16
			rectParam->width 		= 80;
		}else if(0){
			//draw SWIR rect
			rectParam->x 		= 116;	// 84+16*2
			rectParam->width 		= 80;
		}else if(0){
			//draw MWIR rect
			rectParam->x			= 196;	// 164+16*2
			rectParam->width 		= 80;
		}

		if(grpLevel >= 2)
			rectParam->frcolor		= ecolor_Default;
		break;


	default:
		return OSA_EFAIL;
		break;
	}

	return OSA_SOK;
}

#endif
/************************************** The End Of File **************************************/

