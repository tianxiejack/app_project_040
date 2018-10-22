#ifndef _CR_TIMER_HEAD_
#define _CR_TIMER_HEAD_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_sem.h>
#include <osa_mutex.h>

int crTimer_create( int interval, void ( *fnxCall )( void ) );

void crTimer_destroy( );

#endif
