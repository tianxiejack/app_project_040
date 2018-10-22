
#include "osa.h"
#include "osa_sem.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osd_graph.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include "crCore.hpp"
#include "globalData.h"

ICore_1001 *osd_core = NULL;

CGlobalData *precvOsd_grpx = CGlobalData::getInstance();


#define ALG_LINK_GRPX_MAX_CH	2
#define ALG_LINK_GRPX_MAX_WINDOWS	64


typedef struct
{
	UInt32 chId;
	//VDIS_DEV devId;
	Bool chEnable;

	UInt32 numWindows;
	Text_Param_fb winPrms[ALG_LINK_GRPX_MAX_WINDOWS];
	Text_Param_fb winPrms_pri[ALG_LINK_GRPX_MAX_WINDOWS];
} AlgLink_GrpxChParams;

typedef struct _Multich_graphic_
{
    Bool bGraphicInit;
    Int32 devFb_num;

    AlgLink_GrpxChParams  chParam[ALG_LINK_GRPX_MAX_CH];
    OSA_MutexHndl muxLock;
    OSA_SemHndl tskGraphicSem;
    OSA_ThrHndl tskGraphicHndl;
    Bool tskGraphicLoop;
    Bool tskGraphicStopDone;
} Multich_graphic;

Multich_graphic grpxChWinPrms = 
{
	.bGraphicInit = FALSE,
};

#ifdef OSD_MODE_RGB
#include "grpFont.h"

//RGBA
#define RGBA_RED 0xFF0000FF
#define RGBA_GREEN 0x00FF00FF
#define RGBA_BLUE 0x0000FFFF
#define RGBA_BLACK	0x000000FF
#define RGBA_WHITE	0xFFFFFFFF
#define RGBA_YELLOW 0xFFFF00FF
#define RGBA_BKGD 0x00000000	// background

#define MAKERGBA(R,G,B,A)	((unsigned int)((((unsigned int)(A)|((unsigned int)((unsigned int)(B))<<8))|(((unsigned int)(unsigned int)(G))<<16))|(((unsigned int)(unsigned int)(R))<<24)))
#define GETRGBA(R,G,B,A, RGBA)	(A) = RGBA & 0xff;	(B) = ((RGBA)>>8) & 0xff;	(G) = ((RGBA)>>16) & 0xff;	(R) = ((RGBA)>>24) & 0xff;

unsigned int GetRgbColour(unsigned int colorId)
{
	switch(colorId)
	{
	case ecolor_Black:
		return RGBA_BLACK;
		break;
	case ecolor_White:
		return RGBA_WHITE;
		break;
	case ecolor_Red:
		return RGBA_RED;
		break;
	case ecolor_Yellow:
		return RGBA_YELLOW;
		break;
	case ecolor_Blue:
		return RGBA_BLUE;
		break;
	case ecolor_Green:
		return RGBA_GREEN;
		break;

	default:
		return RGBA_BKGD;
		break;
	}
}

void osd_draw_string(cv::Mat frame, int startx, int starty, char *pString, UInt32 frcolor, UInt32 bgcolor, int chId)
{

	uchar *pin, *pChar, *fontData;
	int numchar = strlen(pString);
	UInt32 i,j,k,pixcolor,index, offset;
	int fontWidth, fontHeight;
	unsigned char data;
	bool FontFind = 0;
	int add=0;

	if(chId==0)
	{
		fontWidth		= 	16;
		fontHeight 	= 	20;
		fontData		=	FONT_LIBRARY_16x20;
	}
	else if(chId==1)
	{
		fontWidth		= 	10;
		fontHeight 	= 	18;
		fontData		=	FONT_LIBRARY_0814;
	}
	if(fontWidth%8!=0)
	{
		add=1;
	}
	else
	{
		add=0;
	}
	for(i=0; i<fontHeight; i++)
	{
		pin = frame.ptr<uchar>(starty+i);
		for(k=0; k<numchar; k++)
		{
			index = (UInt32)pString[k];
			index=index-' ';
			pChar = &fontData[i*(fontWidth/8+add)+index*(fontWidth/8+add)*fontHeight];
			for(j=startx+k*fontWidth; j<startx+k*fontWidth+fontWidth; j++)
			{
				offset 	= j-startx-k*fontWidth;
				data 	= *(pChar + offset/8);
				data 	<<= (offset%8);

				pixcolor		= (data&0x80)?frcolor:bgcolor;
				*(pin+j)		= pixcolor;
				
				//*(pin+j*4)		= pixcolor & 0xFF;
				//*(pin+j*4+1)	= (pixcolor >> 8) & 0xFF;
				//*(pin+j*4+2)	= (pixcolor >> 16) & 0xFF;
				//*(pin+j*4+3)	= (pixcolor >> 24) & 0xFF;
			}
		}
	}
}

void osd_draw_text(cv::Mat frame, void *prm, int chId)
{
	Text_Param_fb * pObj = (Text_Param_fb *)prm;
	UInt32 fontfrColor,fontbgColor, enableWin;

	if(pObj == NULL)
		return ;

	int i;
	int x[3];
	int y[3];
	int vaild[3];
	char textStr[3][128];
	int textLenPrev[3];

	for (i = 0; i < 3; i++)
	{
		x[i] = pObj->text_x[i];
		y[i] = pObj->text_y[i];
	}
	vaild[0] = pObj->text_valid&0x0001;
	vaild[1] = (pObj->text_valid >> 1)&0x0001;
	vaild[2] = (pObj->text_valid >> 2)&0x0001;
	textLenPrev[0] = 0;
	textLenPrev[1] = pObj->textLen[0];
	textLenPrev[2] = pObj->textLen[0] + pObj->textLen[1];
	OSA_assert( textLenPrev[0] < 128 && textLenPrev[1] < 128 && textLenPrev[2] < 128 );

	memcpy(textStr[0], pObj->textAddr, pObj->textLen[0]);
	textStr[0][pObj->textLen[0]] = '\0';
	memcpy(textStr[1], pObj->textAddr + textLenPrev[1], pObj->textLen[1]);
	textStr[1][pObj->textLen[1]] = '\0';
	memcpy(textStr[2], pObj->textAddr + textLenPrev[2], pObj->textLen[2]);
	textStr[2][pObj->textLen[2]] = '\0';

	for (i = 0; i < 3; i++)
	{
		if(vaild[i] != 0)
			fontfrColor = GetRgbColour(2);
		else
			fontfrColor = 0x00000000;
		fontbgColor = pObj->bgcolor;
		osd_draw_string(frame, x[i], y[i], textStr[i], fontfrColor, fontbgColor, chId);
	}

	return ;
}
#endif

#ifdef OSD_MODE_CV
CvScalar GetcvColour(unsigned int colorId)
{
	switch(colorId)
	{
	case ecolor_Black:
		return cvScalar(0,0,0,255);
		break;
	case ecolor_White:
		return cvScalar(255,255,255,255);
		break;
	case ecolor_Red:
		return cvScalar(0,0,255,255);
		break;
	case ecolor_Yellow:
		return cvScalar(0,255,255,255);
		break;
	case ecolor_Blue:
		return cvScalar(255,0,0,255);
		break;
	case ecolor_Green:
		return cvScalar(0,255,0,255);
		break;

	default:
		return cvScalar(0,0,0,0);
		break;
	}
}

void osd_cvdraw_line(cv::Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	cv::Point pt1,pt2;
	CvScalar lineColor = GetcvColour(pObj->frcolor);
	UInt32 linePixels = pObj->linePixels;

	pt1.x=pObj->x;
	pt1.y=pObj->y;
	pt2.x=pObj->x+pObj->width;
	pt2.y=pObj->y+pObj->height;
	line(frame, pt1, pt2, lineColor, linePixels, 8, 0 );

}

void osd_cvdraw_rect(cv::Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	cv::Point pt1,pt2;
	CvScalar lineColor = GetcvColour(pObj->frcolor);
	UInt32 linePixels = pObj->linePixels;
/*
	pt1.x=pObj->x-pObj->width/2;
	pt1.y=pObj->y-pObj->height/2;
	pt2.x=pObj->x+pObj->width/2;
	pt2.y=pObj->y+pObj->height/2;
*/

	pt1.x = 100+precvOsd_grpx->osd_core.osd_secondTrk_X - pObj->width/2;
	pt1.y = 100+precvOsd_grpx->osd_core.osd_secondTrk_Y -pObj->height/2;
	pt2.x = 100+precvOsd_grpx->osd_core.osd_secondTrk_X + pObj->width/2;
	pt2.y = 100+precvOsd_grpx->osd_core.osd_secondTrk_Y + pObj->height/2;

	
	rectangle( frame,pt1,pt2,lineColor, linePixels, 8);


}

void osd_cvdraw_compass(cv::Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iR = 0;
	int deltaX, deltaY;
	UInt32 linePixels, width, height;
	CvScalar lineColor = GetcvColour(pObj->frcolor);
	double Angle = 0.0;
	UInt32 pX1,pX2,pX3,pX4, pY1,pY2,pY3,pY4;

	if(pObj == NULL)
		return ;

	linePixels 	= 1;//pObj->linePixels;

	iX = pObj->x;
	iY = pObj->y;

	iR 			= (pObj->width + pObj->height) / 3;
	Angle 		= (double)pObj->res0/360.0*2.0*3.141593;
	deltaX 		= (int)(sin(Angle) * (double)iR);
	deltaY 		= (int)(cos(Angle) * (double)iR) * (-1);
	pX1 = (iX+deltaX);
	pY1 = (iY+deltaY);

	iR 			= (pObj->width + pObj->height) / 3;
	Angle 		= (double)(pObj->res0+180)/360.0*2.0*3.141593;
	deltaX 		= (int)(sin(Angle) * (double)iR);
	deltaY 		= (int)(cos(Angle) * (double)iR) * (-1);
	pX2 = (iX+deltaX);
	pY2 = (iY+deltaY);

	iR 			= (pObj->width + pObj->height) / 8;
	Angle 		= (double)(pObj->res0+90)/360.0*2.0*3.141593;
	deltaX 		= (int)(sin(Angle) * (double)iR);
	deltaY 		= (int)(cos(Angle) * (double)iR) * (-1);
	pX3 = (iX+deltaX);
	pY3 = (iY+deltaY);

	iR 			= (pObj->width + pObj->height) / 8;
	Angle 		= (double)(pObj->res0-90)/360.0*2.0*3.141593;
	deltaX 		= (int)(sin(Angle) * (double)iR);
	deltaY 		= (int)(cos(Angle) * (double)iR) * (-1);
	pX4 = (iX+deltaX);
	pY4 = (iY+deltaY);

	line(frame,
		cv::Point((float)pX3, (float)pY3),
		cv::Point((float)pX4, (float)pY4),
		lineColor, linePixels, 8);
	line(frame,
		cv::Point((float)pX1, (float)pY1),
		cv::Point((float)pX3, (float)pY3),
		lineColor, linePixels, 8);
	line(frame,
		cv::Point((float)pX1, (float)pY1),
		cv::Point((float)pX4, (float)pY4),
		lineColor, linePixels, 8);
	line(frame,
		cv::Point((float)pX2, (float)pY2),
		cv::Point((float)pX3, (float)pY3),
		lineColor, linePixels, 8);
	line(frame,
		cv::Point((float)pX2, (float)pY2),
		cv::Point((float)pX4, (float)pY4),
		lineColor, linePixels, 8);

	int npt[] = {3};
	cv::Point pt[] = {cv::Point((float)pX1, (float)pY1),cv::Point((float)pX3, (float)pY3),cv::Point((float)pX4, (float)pY4)};
	const cv::Point* pts[] = {pt};
	cv::fillPoly(frame, 
		pts, npt, 1,
		lineColor);

}

void drawdashline(cv::Mat frame,int startx,int starty,int endx,int endy,int linelength,int dashlength,int color)
{
	int i=0;
	int flagx=1;
	int flagy=1;
	double totallengthy=0.0;
	double  totallengthx=0.0;
	int totallength=dashlength+linelength;  
	int len=abs(endy-starty)+abs(endx-startx);
	double nCount=len/totallength;
	CvScalar colour=GetcvColour(color);
	totallengthx=abs(endx-startx)*1.0/nCount;
	totallengthy=abs(endy-starty)*1.0/nCount;

	//printf("jet +++ color = %f\n",colour);
	cv::Point start,end;//start and end point of each dash  
	if(startx>endx)
		flagx=-1;
	if(starty>endy)
		flagy=-1;

	if(startx==endx)
	{		
		for (int i=0;i<nCount;i=i+2)
		{  
			end.x=startx;
			start.x=startx; 
			start.y=cvRound(starty+i*totallengthy*flagy);
			end.y=cvRound(starty+(i+1)*totallengthy*flagy);//draw left dash line
			line(frame, start, end, colour, 2, 8, 0 ); 
		}  
		return ;
	}
	if(starty==endy)
	{
		for (int i=0;i<nCount;i=i+2)
		{  
			start.x=cvRound(startx+i*totallengthx*flagx);
			end.x=cvRound(startx+(i+1)*totallengthx*flagx); 
			start.y=starty;
			end.y=starty; 
			line(frame, start, end, colour, 2, 8, 0 ); 
		}  
		return ;
	}

	for (int i=0;i<nCount;i=i+2)
	{  
		end.x=cvRound(startx+(i+1)*totallengthx*flagx);//draw top dash line  
		start.x=cvRound(startx+i*totallengthx*flagx);  
		start.y=cvRound(starty+i*totallengthy*flagy);  
		end.y=cvRound(starty+(i+1)*totallengthy*flagy);//draw left dash line  

		line(frame, start, end, colour, 2, 8, 0 ); 
	}  
}

void DrawcvDashcross(cv::Mat frame,Line_Param_fb *lineparm,int linelength,int dashlength, bool b_show)
{
	int centx, centy, width;
	if(lineparm==NULL)
		return;
	
	centx=lineparm->x;
	centy=lineparm->y;
	width=lineparm->width;
	int startx=centx-width/2;
	int starty=centy;
	int endx=centx+width/2;
	int endy=centy;
	if(b_show)
		drawdashline(frame,startx,starty,endx,endy,linelength,dashlength,lineparm->frcolor);

	startx=centx;
	starty=centy-width/2;
	endx=centx;
	endy=centy+width/2;
	if(b_show)
		drawdashline(frame,startx,starty,endx,endy,linelength,dashlength,lineparm->frcolor);	

}

void osd_cvdraw_SecondTrk_Cross(cv::Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;

	pObj->x = precvOsd_grpx->osd_core.osd_secondTrk_X;
	pObj->y = precvOsd_grpx->osd_core.osd_secondTrk_Y;
	
	if(precvOsd_grpx->osd_core.osd_enSecondTrk)
		DrawcvDashcross(frame,pObj,2,2,true);
	else
		DrawcvDashcross(frame,pObj,2,2,false);
}

void osd_cvdraw_text(cv::Mat frame, void *prm)
{
	Text_Param_fb * pObj = (Text_Param_fb *)prm;
	cv::Point pt[3];
	CvScalar lineColor = GetcvColour(pObj->frcolor);
	int i;
	int vaild[3];
	int textLenPrev[3];
	char textStr[3][128];

	if(pObj == NULL)
		return ;

	for (i = 0; i < 3; i++)
	{
		pt[i] = cv::Point(pObj->text_x[i], pObj->text_y[i]);
	}
	vaild[0] = pObj->text_valid&0x0001;
	vaild[1] = (pObj->text_valid >> 1)&0x0001;
	vaild[2] = (pObj->text_valid >> 2)&0x0001;
	textLenPrev[0] = 0;
	textLenPrev[1] = pObj->textLen[0];
	textLenPrev[2] = pObj->textLen[0] + pObj->textLen[1];
	OSA_assert( textLenPrev[0] < 128 && textLenPrev[1] < 128 && textLenPrev[2] < 128);

	memcpy(textStr[0], pObj->textAddr, pObj->textLen[0]);
	textStr[0][pObj->textLen[0]] = '\0';
	memcpy(textStr[1], pObj->textAddr + textLenPrev[1], pObj->textLen[1]);
	textStr[1][pObj->textLen[1]] = '\0';
	memcpy(textStr[2], pObj->textAddr + textLenPrev[2], pObj->textLen[2]);
	textStr[2][pObj->textLen[2]] = '\0';

	for (i = 0; i < 3; i++)
	{
		if(vaild[i] == 0)
			lineColor = GetcvColour(ecolor_Default);//(ecolor_Default);
		else
			lineColor = GetcvColour(2);//(ecolor_Default);
		putText(frame, textStr[i],
				pt[i],
				CV_FONT_HERSHEY_COMPLEX, 1.0,
				lineColor);
	}

}
#endif

void Erase_graph_win(cv::Mat frame, void *Parampri, int chId)
{
	Text_Param_fb * textParam = (Text_Param_fb *)Parampri;
	textParam->frcolor = ecolor_Default;

	switch(textParam->objType)
	{
	case grpx_ObjId_Line:
		//osd_cvdraw_line(frame, Parampri);
		break;
	case grpx_ObjId_Rect:
		//osd_cvdraw_rect(frame, Parampri);
		break;
	case grpx_ObjId_Rect_gap:
		//osd_cvdraw_rect_gap(frame, Parampri);
		break;
	case grpx_ObjId_Compass:
		//osd_cvdraw_compass(frame, Parampri);
		break;
	case grpx_ObjId_Text:
		//osd_cvdraw_text(frame, Parampri);
		//osd_draw_text(frame, Parampri, chId);
		break;
	case grpx_ObjId_SecondTrk_Cross:
		//osd_cvdraw_SecondTrk_Cross(frame, Parampri);
		break;
	// for project
	// for project end

	default:
		break;
	}

}

void Draw_graph_win(cv::Mat frame, void *Param, int chId)
{
	Text_Param_fb * textParam = (Text_Param_fb *)Param;

	textParam->frcolor = ecolor_White;

	switch(textParam->objType)
	{
	case grpx_ObjId_Line:
		//osd_cvdraw_line(frame, Param);
		break;
	case grpx_ObjId_Rect:
		//osd_cvdraw_rect(frame, Param);
		break;
	case grpx_ObjId_Rect_gap:
		//osd_cvdraw_rect_gap(frame, Param);
		break;
	case grpx_ObjId_Compass:
		//osd_cvdraw_compass(frame, Param);
		break;
	case grpx_ObjId_Text:
		//osd_cvdraw_text(frame, Param);
		osd_draw_text(frame, Param, chId);
		break;
	case grpx_ObjId_SecondTrk_Cross:
		//osd_cvdraw_SecondTrk_Cross(frame, Param);
		break;

	// for project
	// for project end

	default:
		break;
	}

}

/*************************************************************
	Proj App
**************************************************************/
static void* MultichGrpx_task(void *pPrm)
{
	int status = 0;
	int chId = 0, winId = 0;
	cv::Mat frame;
	AlgLink_GrpxChParams *pChPrm;

	osd_core = (ICore_1001 *)ICore::Qury(COREID_1001);
	
	OSA_printf(" %s task start!!! \r\n", __func__);
	Multich_graphic *pMultGraphicObj = &grpxChWinPrms;

	while (pMultGraphicObj->tskGraphicLoop == TRUE)
	{
		status = OSA_semWait(&pMultGraphicObj->tskGraphicSem, OSA_TIMEOUT_FOREVER);
		//OSA_printf(" %s run %d\r\n", __func__, OSA_getCurTimeInMsec());

		if (pMultGraphicObj->tskGraphicLoop == FALSE)
			break;

		//no create  muxLock
		//OSA_mutexLock(&pMultGraphicObj->muxLock);
		for(chId = 0; chId < pMultGraphicObj->devFb_num; chId++)
		{
			frame = osd_core->m_dc[chId];
			pChPrm = &pMultGraphicObj->chParam[chId];
			for(winId = 0; winId < pChPrm->numWindows; winId++)
			{
				APP_set_graphic_parms(chId, winId, 
					&pChPrm->winPrms[winId]);

				if(!pChPrm->winPrms[winId].enableWin)
					continue;

				Erase_graph_win(frame,&pChPrm->winPrms_pri[winId], chId);
				Draw_graph_win(frame,&pChPrm->winPrms[winId], chId);
				
				memcpy(&pChPrm->winPrms_pri[winId], &pChPrm->winPrms[winId], sizeof(Text_Param_fb));
			}
		}
		//OSA_mutexUnlock(&pMultGraphicObj->muxLock);
	}

	pMultGraphicObj->tskGraphicStopDone = TRUE;

	OSA_printf(" %s task exit!!! \r\n", __func__);

	return NULL;
}

void  osd_graph_init()
{
	int status=0;
	int i, devId, winId;
	memset(&grpxChWinPrms, 0, sizeof(Multich_graphic));

	grpxChWinPrms.devFb_num = ALG_LINK_GRPX_MAX_CH;
	for ( devId = 0; devId < grpxChWinPrms.devFb_num; devId++)
	{
		grpxChWinPrms.chParam[devId].chId = devId;
		grpxChWinPrms.chParam[devId].chEnable = TRUE;
		grpxChWinPrms.chParam[devId].numWindows = WINID_MAX;

		for(i = 0; i < grpxChWinPrms.chParam[devId].numWindows; i++)
		{
			winId = i;
			APP_init_graphic_parms(devId, winId, 
					&grpxChWinPrms.chParam[devId].winPrms[winId]);
		}
	}

	grpxChWinPrms.tskGraphicLoop = TRUE;
	grpxChWinPrms.tskGraphicStopDone = FALSE;
	status = OSA_semCreate(&grpxChWinPrms.tskGraphicSem, 1, 0);
	OSA_assert(status == OSA_SOK);
	status = OSA_thrCreate(
			&grpxChWinPrms.tskGraphicHndl,
			MultichGrpx_task,
			OSA_THR_PRI_DEFAULT,
			0,
			0
			);
	OSA_assert(status == OSA_SOK);

	grpxChWinPrms.bGraphicInit = TRUE;
	OSA_printf(" %s done.\n", __func__);

}

void osd_graph_uninit(void)
{
	Multich_graphic *pMultGraphicObj = &grpxChWinPrms;

	if (!pMultGraphicObj->bGraphicInit)
		return ;

	grpxChWinPrms.tskGraphicLoop = FALSE;
	OSA_semSignal(&pMultGraphicObj->tskGraphicSem);

	OSA_waitMsecs(1000);
	OSA_thrDelete(&grpxChWinPrms.tskGraphicHndl);
	OSA_semDelete(&pMultGraphicObj->tskGraphicSem);

	grpxChWinPrms.bGraphicInit = FALSE;
	OSA_printf(" %s done.\n", __func__);

}

void osd_grpxUpdate()
{
	osd_graph_update_sem_post();

}
void osd_graph_update_sem_post(void)
{
	Multich_graphic *pMultGraphicObj = &grpxChWinPrms;

	if (!pMultGraphicObj->bGraphicInit)
		return ;

	OSA_semSignal(&pMultGraphicObj->tskGraphicSem);

}

