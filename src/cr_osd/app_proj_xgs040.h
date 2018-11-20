/*****************************************************************************
 * Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
 * FileName:        APP_proj_xgs021.h
 * Author:          aloysa
 * Date:            2017-08-01
 * Description:     // description
 * Version:         // version
 * Function List:   // funs and description
 * History:         // histroy modfiy record
 *     <author>  <time>   <version >   <desc>
 *
*****************************************************************************/

#ifndef __APP_PROJ_XGS40_H_
#define __APP_PROJ_XGS40_H_


static int vcapWH[2][2] = {{1920, 1080}, {1280, 1024}};

static int vdisWH[2][2] = {{1920, 1080}, {640, 360}};		// main and picp

#define MMT_NUMBER 8

static int trkWinWH[2][3][2]=
{
	//TV
	{
		{120,100},
		{80,60},
		{60,45},
	},
	//FR
	{
		{80,64},
		{60,48},
		{40,32},
	},
};

static int fovSize[2][5] = 
{
	{
		2400,
		1000,
		330,
		100,
		50
	},
	{
		4000,
		1000,
		330,
		83,
		41
	}

};

typedef enum
{
    eAxis_ref_no			,
    eAxis_ref_left			,
    eAxis_ref_right		,
    eAxis_ref_up			,
    eAxis_ref_down		,
    eAxis_ref_upleft		,
    eAxis_ref_upright		,
    eAxis_ref_downleft	,
    eAxis_ref_downright	,    
}Axis_move;

typedef enum
{
    eTrk_ref_no			,
    eTrk_ref_left			,
    eTrk_ref_right			,
    eTrk_ref_up			,
    eTrk_ref_down		,
    eTrk_ref_upleft		,
    eTrk_ref_upright		,
    eTrk_ref_downleft		,
    eTrk_ref_downright	,    
}TrkWin_move;

typedef enum {
	eSen_ColTV		= 0x00,
	eSen_BlackTV    = 0x01,
	eSen_FR    		= 0x02,
	eSen_ShortFR    = 0x03,
	eSen_Max,
}eSenserStat;

typedef enum _workMode
{
    eMode_svr       = 0x00,
    eMode_trk       = 0x06,
    eMode_search    = 0x07,
} eWorkMode;

typedef enum Dram_DispGradeColor
{
	ecolor_Default	= 0x00,
	ecolor_Black		= 0x01,
	ecolor_White	= 0x02,
	ecolor_Red		= 0x03,
	ecolor_Yellow	= 0x04,
	ecolor_Blue		= 0x05,
	ecolor_Green 	= 0x06,
} eOSDColor;

typedef enum
{
	GRPX_WINID_SYS_TIMER_TEXT 		= 0x00,
	GRPX_WINID_SYS_MODE_TEXT,
	GRPX_WINID_SENSOR_TEXT,
	GRPX_WINID_ALG_MARK_TEXT,
	GRPX_WINID_PLAT_ANGLE_TEXT,
	GRPX_WINID_ORIENTATE_TEXT		= 0x05,
	GRPX_WINID_LASER_STAT_TEXT,
	GRPX_WINID_LASER_WORK_TEXT,
	GRPX_WINID_LASER_COUNT_TEXT,
	GRPX_WINID_POSE_ANGLE_TEXT,
	GRPX_WINID_INFO_MSG_TEXT,
	GRPX_WINID_CROSS_TEXT			= 0x0B,
	GRPX_WINID_TRKRTS_TEXT,
	GRPX_WINID_SENSOR_RECT,
	GRPX_WINID_FOVMATCH_RECT,
	GRPX_WINID_COMPASS,
	GRPX_WINID_SECONDTRK_CROSS,

	GRPX_WINID_MAX			// please don't modify this id name

} osdCR_graph_winId;

#define WINID_MAX (GRPX_WINID_MAX)

#define BYTE2BIT(Byte, BitNum, At) (((BitNum+At)<=8) ? ((Uint8)(Byte<<(8-At-BitNum)) >> (8-BitNum)) : -1)

int APP_init_graphic_parms(int chId, int winId, void *pPrm);
int APP_set_graphic_parms(int chId, int winId, void *pPrm);

#endif
/*************************************END***************************************/

