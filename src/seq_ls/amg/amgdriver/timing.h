
/******************************************************************************
 *
 * Header info for HYPRE Timing routines
 *
 *****************************************************************************/

#ifndef HYPRE_TIMING_HEADER
#define HYPRE_TIMING_HEADER

#include <sys/times.h>
#include <sys/time.h>
#include <sys/param.h>
#include <unistd.h>


typedef hypre_longint amg_Clock_t;
#define AMG_TICKS_PER_SEC 10000

typedef clock_t amg_CPUClock_t;

#ifdef __STDC__
# define        P(s) s
#else
# define P(s) ()
#endif
 
 
/* timing.c */
void HYPRE_AMGClock_init P((void ));
amg_Clock_t HYPRE_AMGClock P((void ));
amg_CPUClock_t HYPRE_AMGCPUClock P((void ));
void HYPRE_PrintTiming P((HYPRE_Real time_ticks , HYPRE_Real cpu_ticks ));
 
#undef P

#endif
