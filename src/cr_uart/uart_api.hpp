
#include <osa_tsk.h>
#include <osa_rng.h>
#include <osa_sem.h>

#define CORE_CHN_MAX	(4)
#define CORE_TGT_NUM_MAX	(12)

typedef struct uart_tsk_obj
{
    OSA_TskHndl tsk;

    OSA_ThrHndl thrHandleDataIn;
    //OSA_SemHndl dataInNotifySem;

}uart_tsk_obj;

typedef struct
{
	bool enTrk;
	bool enMTD;
	short secondTrk_X;
	short secondTrk_Y;	
	int chId;
	int fovId;
	int nTarget_index;
	int enENH[4];
	int DispLever;
	int nFrames;
	int enZoomx;
	int Color;
	int EncTransLevel;
	int videoTransMode;
	int TrkWinSize;
}CORE_CMD;



void uart_thrCreat();

