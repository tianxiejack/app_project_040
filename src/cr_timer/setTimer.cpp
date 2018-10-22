#include "setTimer.h"

typedef struct _timer_t{

	UInt32  tmReg[CR_TIMER_MAX];
	UInt32  tmCnt[CR_TIMER_MAX];
	 UInt32  tmBegen[CR_TIMER_MAX];
	 TimerCallback	 funcTimer[CR_TIMER_MAX];
}Timer_OBJ;

static Timer_OBJ   gCrTimerObj ;

static bool g_bCreated = false;

static void CR_OnTimer( void )
{
    int		i;
    UInt32	curTM = OSA_getCurTimeInMsec( );

    for( i = 0; i < CR_TIMER_MAX; i++ )
    {
        if( gCrTimerObj.tmReg[i] == 0 )
        {
            continue;
        }

        while( curTM >= gCrTimerObj.tmBegen[i] + ( gCrTimerObj.tmCnt[i] + 1 ) * gCrTimerObj.tmReg[i] )
        {
        	/*to do simple work*/
        	if( gCrTimerObj.funcTimer[i] != NULL){
        		int elapsedTime = curTM - gCrTimerObj.tmBegen[i];
        		gCrTimerObj.funcTimer[i](elapsedTime);
        	}
            gCrTimerObj.tmCnt[i]++;
        }
    }
}

Int32 CR_createTimer( int interval)
{
	memset( &gCrTimerObj, 0, sizeof( gCrTimerObj ) );

	if(g_bCreated){
		crTimer_destroy( );
	}
	g_bCreated = true;
	return	crTimer_create( interval, CR_OnTimer );
}

void CR_destroyTimer()
{
	crTimer_destroy( );
	g_bCreated = false;
}

Int32 CR_setTimer( UInt32 timerId, UInt32 nMs, TimerCallback  funcTimer)
{
    if( timerId >= CR_TIMER_MAX )
    {
        return OSA_EFAIL;
    }

    gCrTimerObj.tmReg[timerId]	   = nMs;
    gCrTimerObj.tmBegen[timerId]	   = OSA_getCurTimeInMsec( );
    gCrTimerObj.tmCnt[timerId]	   = 0;
    gCrTimerObj.funcTimer[timerId] = funcTimer;

    return OSA_SOK;
}

Int32 CR_killTimer( UInt32 timerId )
{
    if( timerId >= CR_TIMER_MAX )
    {
        return OSA_EFAIL;
    }

    gCrTimerObj.tmReg[timerId] = 0;
    gCrTimerObj.funcTimer[timerId]  = NULL;

    return OSA_SOK;
}
