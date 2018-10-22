#ifndef 		_SET_TIMER_HEAD_
#define 	_SET_TIMER_HEAD_

#include "crTimer.h"

#define CR_TIMER_MAX				( 32 )

#define RECV_MSG_TIMER			(0x01)
#define RECV_MSG_TICKS			(40) //ms

#define SEND_MSG_TIMER			(0x02)
#define SEND_MSG_TICKS			(33) //ms

#define DETECT_VIDEO_TIMER		(0x03)
#define DETECT_VIDEO_TICKS		(300) //ms

typedef void ( *TimerCallback )( UInt32 elapsedTime);

Int32 CR_createTimer( int interval); // return 0:successful

void CR_destroyTimer();

Int32 CR_setTimer( UInt32 timerId, UInt32 nMs, TimerCallback  funcTimer );

Int32 CR_killTimer( UInt32 timerId );


#endif
