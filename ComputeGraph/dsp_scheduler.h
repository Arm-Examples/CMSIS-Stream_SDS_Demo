/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef _DSP_SCHEDULER_H_ 
#define _DSP_SCHEDULER_H_

#ifdef   __cplusplus
extern "C"
{
#endif

#include "EventRecorder.h"

#define EvtSched 0x01 

#define Evt_Scheduler   EventID (EventLevelAPI,   EvtSched, 0x00)
#define Evt_Node        EventID (EventLevelAPI,   EvtSched, 0x01)
#define Evt_Error       EventID (EventLevelError,   EvtSched, 0x02)



extern uint32_t dsp_scheduler(int *error,graph_context *cg);

#ifdef   __cplusplus
}
#endif

#endif

