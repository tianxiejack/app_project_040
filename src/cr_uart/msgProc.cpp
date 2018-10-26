
#include"msgProc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include "crCore.hpp"

#include "cr_osd/app_proj_xgs040.h"
#define WHITECOLOR 		0x008080FF
#define YELLOWCOLOR 		0x009110D2
#define CRAYCOLOR		0x0010A6AA
#define GREENCOLOR		0x00223691
#define MAGENTACOLOR	0x00DECA6A
#define REDCOLOR			0x00F05A51
#define BLUECOLOR		0x006EF029
#define BLACKCOLOR		0x00808010
#define BLANKCOLOR		0x00000000

ICore_1001 *uart_core = NULL;

void app_getCoredata()
{
	uart_core = (ICore_1001 *)ICore::Qury(COREID_1001);

}

void app_ctrl_setSensor(CORE_CMD * pInCmd)
{
	int chIdBak=0;
	cv::Size acqSize;
	acqSize.width = trkWinWH[pInCmd->chId][1][0];
	acqSize.height = trkWinWH[pInCmd->chId][1][1];

	chIdBak=pInCmd->chId;
	
	if (chIdBak!= uart_core->m_stats.mainChId)
	{				
		uart_core->setMainChId(pInCmd->chId, 0, 0, acqSize);
		chIdBak = uart_core->m_stats.mainChId;
		OSA_printf("jet +++pInCmd->chId=%d,core_chId=%d\n",pInCmd->chId,uart_core->m_stats.mainChId);
	}
}

void app_ctrl_setEnhance(CORE_CMD * pInCmd)
{
	int enENHBak=0;

	enENHBak=pInCmd->enENH[pInCmd->chId];
	
	if (enENHBak!= uart_core->m_stats.chn[pInCmd->chId].enableEnh)
	{				
		uart_core->enableEnh(pInCmd->enENH[pInCmd->chId]);
		enENHBak = uart_core->m_stats.chn[pInCmd->chId].enableEnh;
		OSA_printf("jet +++pInCmd->enENH=%d,core_ENH=%d\n",pInCmd->enENH[pInCmd->chId],uart_core->m_stats.chn[pInCmd->chId].enableEnh);
	}
}

void app_ctrl_setTrkStat(CORE_CMD * pInCmd)
{
	int enTrkBak=0;
	cv::Size acqSize;
	acqSize.width = trkWinWH[pInCmd->chId][1][0];
	acqSize.height = trkWinWH[pInCmd->chId][1][1];

	enTrkBak=pInCmd->enTrk;
	
	if (enTrkBak!= uart_core->m_stats.enableTrack)
	{				
		uart_core->enableTrack(pInCmd->enTrk, acqSize, true);
		enTrkBak = uart_core->m_stats.enableTrack;
		OSA_printf("jet +++pInCmd->enTrk=%d,core_Trk=%d\n",pInCmd->enTrk,uart_core->m_stats.enableTrack);
	}
}

void app_ctrl_setSearchTrk(CORE_CMD * pInCmd)
{
	int enSecTrkBak=0;
	Rect2f winRect;
	winRect.width = (float)trkWinWH[pInCmd->chId][1][0];
	winRect.height = (float)trkWinWH[pInCmd->chId][1][1];
	winRect.x = (float)pInCmd->secondTrk_X - winRect.width/2;
	winRect.y = (float)pInCmd->secondTrk_Y - winRect.height/2;
	
	enSecTrkBak=pInCmd->enTrk;
	
	//if (enSecTrkBak!= uart_core->m_stats.enableTrack)
	{				
		uart_core->enableTrack(pInCmd->enTrk, winRect, true);
		enSecTrkBak = uart_core->m_stats.enableTrack;
		OSA_printf("jet +++pInCmd->secondTrk_XY=(%d,%d)\n",pInCmd->secondTrk_X,pInCmd->secondTrk_Y);
	}


}

void app_ctrl_setAxisPos(int dir, int step)
{
	cv::Point2f curPos = uart_core->m_stats.chn[uart_core->m_stats.mainChId].axis;
	
	switch(dir)
	{
		case eAxis_ref_up:
			{
				cv::Point pos(curPos.x+0.5, curPos.y-0.5-1*step);
				uart_core->setAxisPos(pos);
			}
			break;
		case eAxis_ref_down:
			{
				cv::Point pos(curPos.x+0.5, curPos.y+0.5+1*step);
				uart_core->setAxisPos(pos);
			}
			break;
		case eAxis_ref_left:
			{
				cv::Point pos(curPos.x-0.5-1*step, curPos.y+0.5);
				uart_core->setAxisPos(pos);
			}
			break;
		case eAxis_ref_right:
			{
				cv::Point pos(curPos.x+0.5+1*step, curPos.y+0.5);
				uart_core->setAxisPos(pos);
			}
			break;
		default:
			break;
	}
}

void app_ctrl_setAimPos(int dir, int step)
{
	switch(dir)
	{
	case eTrk_ref_up:
		{
			cv::Point raf(0, -1*step);
			uart_core->setTrackPosRef(raf);
		}
		break;
	case eTrk_ref_down:
		{
			cv::Point raf(0, step);
			uart_core->setTrackPosRef(raf);
		}
		break;
	case eTrk_ref_left:
		{
			cv::Point raf(-1*step, 0);
			uart_core->setTrackPosRef(raf);
		}
		break;
	case eTrk_ref_right:
		{
			cv::Point raf(step, 0);
			uart_core->setTrackPosRef(raf);
		}
		break;
	default:
		break;
	}
}

void app_ctrl_SaveAxisPos()
{
	uart_core->saveAxisPos();
}

void app_ctrl_setDispColor(CORE_CMD * pInCmd)
{
	static int colorTab[] = {WHITECOLOR,BLACKCOLOR,CRAYCOLOR,GREENCOLOR,MAGENTACOLOR,REDCOLOR,BLUECOLOR,BLACKCOLOR};

	static int enColorBak=0;

	if (enColorBak!= pInCmd->Color)
	{				
		uart_core->setOSDColor(colorTab[pInCmd->Color]);
		enColorBak = pInCmd->Color;
		OSA_printf("jet +++pInCmd->Color=%d\n", pInCmd->Color);
	}
}

void app_ctrl_setDispGrade(CORE_CMD * pInCmd)
{
	static bool DispLvBak=true;
	bool enDispLever=true;

	if((pInCmd->DispLever==0x00)||(pInCmd->DispLever==0x01))
		enDispLever = true;
	else if(pInCmd->DispLever==0x02)
		enDispLever = false;

	//OSA_printf("jet +++pInCmd->Color=%d,menDispLever=%d,DispLvBak=%d\n", pInCmd->DispLever,enDispLever,DispLvBak);
	if(DispLvBak!= enDispLever)
	{		
		uart_core->enableOSD(enDispLever);		
		DispLvBak = enDispLever;
		OSA_printf("jet +++pInCmd->DispLever=%d\n", pInCmd->DispLever);
	}
}

void app_ctrl_setMMT(CORE_CMD * pInCmd)
{
	bool enMMTBak=false;

	enMMTBak=pInCmd->enMTD;
	
	if (enMMTBak!= uart_core->m_stats.enableMMTD)
	{				
		uart_core->enableMMTD(pInCmd->enMTD,MMT_NUMBER);
		enMMTBak = uart_core->m_stats.enableMMTD;
		OSA_printf("jet +++pInCmd->enMTD=%d,core_MMT=%d\n",pInCmd->enMTD,uart_core->m_stats.enableMMTD);
	}
}

int app_ctrl_setMmtSelect(CORE_CMD * pInCmd)
{
	int ret;
	int nTarget_indexBak;
	
	nTarget_indexBak = pInCmd->nTarget_index-1;
	
	if(uart_core->m_stats.tgts[nTarget_indexBak].valid)
		ret = 0x01;
	else
		ret = 0x00;

	//OSA_printf("jet +++ pInCmd->nTarget_index=%d valid=%d,ret=%d\n",pInCmd->nTarget_index,uart_core->m_stats.tgts[nTarget_indexBak].valid,ret);

	return ret;
}

void app_ctrl_setMMTTrk(CORE_CMD * pInCmd)
{

	int mmtTrkId;
	cv::Size acqSize;
	acqSize.width = trkWinWH[pInCmd->chId][1][0];
	acqSize.height = trkWinWH[pInCmd->chId][1][1];

	mmtTrkId = pInCmd->nTarget_index-1;
	
	if(uart_core->m_stats.tgts[mmtTrkId].valid)
		uart_core->enableTrackByMMTD(mmtTrkId, &acqSize, true);
	else
		uart_core->enableTrack(true, acqSize, true);

	OSA_printf("jet +++ pInCmd->nTarget_index=%d valid=%d\n",pInCmd->nTarget_index,uart_core->m_stats.tgts[mmtTrkId].valid);

}

void app_ctrl_setZoom(CORE_CMD * pInCmd)
{
	static int enZoomBak=0x01;
	int enZoom=0x00;

	if(pInCmd->enZoomx==0x01)
		enZoom = 0x02;
	else if(pInCmd->enZoomx==0x00)
		enZoom = 0x01;

	if(enZoomBak!= enZoom)
	{		
		uart_core->setEZoomx(enZoom);		
		enZoomBak = enZoom;
		OSA_printf("jet +++pInCmd->enZoomx=%d\n", pInCmd->enZoomx);
	}
}

void app_ctrl_setTrkBomen(CORE_CMD * pInCmd)
{
	static int TrkWinBak=0x00;
	cv::Size acqSize;
	
	if(pInCmd->TrkWinSize==0x01)
	{
		acqSize.width = trkWinWH[pInCmd->chId][0][0];
		acqSize.height = trkWinWH[pInCmd->chId][0][1];
	}
	else if(pInCmd->TrkWinSize==0x02)
	{
		acqSize.width = trkWinWH[pInCmd->chId][1][0];
		acqSize.height = trkWinWH[pInCmd->chId][1][1];
	}
	else if(pInCmd->TrkWinSize==0x03)
	{
		acqSize.width = trkWinWH[pInCmd->chId][2][0];
		acqSize.height = trkWinWH[pInCmd->chId][2][1];
	}	
	else
	{
		acqSize.width = trkWinWH[pInCmd->chId][1][0];
		acqSize.height = trkWinWH[pInCmd->chId][1][1];
	}
	if(TrkWinBak!= pInCmd->TrkWinSize)
	{		
		uart_core->enableTrack(pInCmd->enTrk, acqSize, true);	
		TrkWinBak = pInCmd->TrkWinSize;
		OSA_printf("jet +++ pInCmd->TrkWinSize=%d,enTrk=%d,WH=(%d,%d)\n", pInCmd->TrkWinSize,pInCmd->enTrk,acqSize.width,acqSize.height);
	}
}
void app_ctrl_set422Rate(CORE_CMD * pInCmd )
{
	static int EncTransLevelBak=0x01;

	if(EncTransLevelBak!= pInCmd->EncTransLevel)
	{		
		uart_core->setEncTransLevel(pInCmd->EncTransLevel);
		EncTransLevelBak = pInCmd->EncTransLevel;
		OSA_printf("jet +++ pInCmd->EncTransLevel=%d\n", pInCmd->EncTransLevel);
	}
}

void app_ctrl_set422TransMode(CORE_CMD * pInCmd )
{
	static int videoTransModeBak=0x03;

	if(videoTransModeBak!= pInCmd->videoTransMode)
	{		
		if(pInCmd->videoTransMode == 0x01)
		{
			uart_core->enableEncoder(0, true);
			uart_core->enableEncoder(1, false);
		}
		else if(pInCmd->videoTransMode == 0x02)
		{
			uart_core->enableEncoder(0, false);
			uart_core->enableEncoder(1, true);
		}
		else if(pInCmd->videoTransMode == 0x03)
		{
			uart_core->enableEncoder(0, true);
			uart_core->enableEncoder(1, true);
		}
		else
		{
			uart_core->enableEncoder(0, false);
			uart_core->enableEncoder(1, false);
		}
		
		videoTransModeBak = pInCmd->videoTransMode;
		OSA_printf("jet +++ pInCmd->videoTransMode=%d\n", pInCmd->videoTransMode);
	}
}
