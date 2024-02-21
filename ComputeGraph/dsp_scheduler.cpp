/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include "arm_math.h"
#include "globalCGSettings.h"
#include "GenericNodes.h"
#include "cg_status.h"
#include "DemoNodes.h"
#include "dsp_scheduler.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif

#if !defined(CG_BEFORE_ITERATION)
#define CG_BEFORE_ITERATION
#endif 

#if !defined(CG_AFTER_ITERATION)
#define CG_AFTER_ITERATION
#endif 

#if !defined(CG_BEFORE_SCHEDULE)
#define CG_BEFORE_SCHEDULE
#endif

#if !defined(CG_AFTER_SCHEDULE)
#define CG_AFTER_SCHEDULE
#endif

#if !defined(CG_BEFORE_BUFFER)
#define CG_BEFORE_BUFFER
#endif

#if !defined(CG_BEFORE_FIFO_BUFFERS)
#define CG_BEFORE_FIFO_BUFFERS
#endif

#if !defined(CG_BEFORE_FIFO_INIT)
#define CG_BEFORE_FIFO_INIT
#endif

#if !defined(CG_BEFORE_NODE_INIT)
#define CG_BEFORE_NODE_INIT
#endif

#if !defined(CG_AFTER_INCLUDES)
#define CG_AFTER_INCLUDES
#endif

#if !defined(CG_BEFORE_SCHEDULER_FUNCTION)
#define CG_BEFORE_SCHEDULER_FUNCTION
#endif

#if !defined(CG_BEFORE_NODE_EXECUTION)
#define CG_BEFORE_NODE_EXECUTION(ID)
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION(ID)
#endif



CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[122]=
{ 
5,10,4,1,9,7,5,10,4,1,9,7,6,2,8,5,10,4,1,9,7,6,2,8,5,10,4,1,9,7,5,10,4,1,9,7,6,2,8,5,
10,4,1,9,7,6,2,8,5,10,4,1,9,7,5,10,4,1,9,7,6,2,8,5,10,4,1,9,7,6,2,8,5,10,4,1,9,7,5,10,
4,1,9,7,6,2,8,5,10,4,1,9,7,6,2,8,5,10,4,1,9,7,5,10,4,1,9,7,6,2,8,5,10,4,1,9,7,6,2,3,
8,0,
};


CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 256
#define FIFOSIZE1 256
#define FIFOSIZE2 256
#define FIFOSIZE3 10
#define FIFOSIZE4 256
#define FIFOSIZE5 256
#define FIFOSIZE6 512
#define FIFOSIZE7 1
#define FIFOSIZE8 1
#define FIFOSIZE9 10

#define BUFFERSIZE1 256
CG_BEFORE_BUFFER
q15_t dsp_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 256
CG_BEFORE_BUFFER
float dsp_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 256
CG_BEFORE_BUFFER
q15_t dsp_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 10
CG_BEFORE_BUFFER
float dsp_buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 256
CG_BEFORE_BUFFER
float dsp_buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 256
CG_BEFORE_BUFFER
float dsp_buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 512
CG_BEFORE_BUFFER
float dsp_buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 1
CG_BEFORE_BUFFER
float dsp_buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 1
CG_BEFORE_BUFFER
float dsp_buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 10
CG_BEFORE_BUFFER
float dsp_buf10[BUFFERSIZE10]={0};



CG_BEFORE_SCHEDULER_FUNCTION
uint32_t dsp_scheduler(int *error,graph_context *cg)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<q15_t,FIFOSIZE0,1,0> fifo0(dsp_buf1);
    FIFO<float,FIFOSIZE1,1,0> fifo1(dsp_buf2);
    FIFO<q15_t,FIFOSIZE2,1,0> fifo2(dsp_buf3);
    FIFO<float,FIFOSIZE3,0,0> fifo3(dsp_buf4);
    FIFO<float,FIFOSIZE4,1,0> fifo4(dsp_buf5);
    FIFO<float,FIFOSIZE5,1,0> fifo5(dsp_buf6);
    FIFO<float,FIFOSIZE6,0,0> fifo6(dsp_buf7);
    FIFO<float,FIFOSIZE7,1,0> fifo7(dsp_buf8);
    FIFO<float,FIFOSIZE8,1,0> fifo8(dsp_buf9);
    FIFO<float,FIFOSIZE9,0,0> fifo9(dsp_buf10);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    SDSRecorder<float,10> detectionRec(fifo3,"DetectionRec",0,0); /* Node ID = 0 */
    Duplicate<float,256,float,256> dup0(fifo4,{&fifo5,&fifo6}); /* Node ID = 1 */
    Duplicate<float,1,float,1> dup1(fifo7,{&fifo8,&fifo9}); /* Node ID = 2 */
    SDSRecorder<float,10> energyRec(fifo9,"EnergyRec",0,0); /* Node ID = 3 */
    IIR<float,256,float,256> iir(fifo1,fifo4); /* Node ID = 4 */
    SDSPlayer<q15_t,256> player(fifo0,"Audio",0,0,0); /* Node ID = 5 */
    RMS<float,384,float,1> rms(fifo6,fifo7); /* Node ID = 6 */
    SDSRecorder<q15_t,256> signalRec(fifo2,"AudioRec",0,0); /* Node ID = 7 */
    Threshold<float,1,float,1> threshold(fifo8,fifo3,0.7); /* Node ID = 8 */
    ToFixedPoint<float,256,q15_t,256> toFixedPoint(fifo5,fifo2); /* Node ID = 9 */
    ToFloat<q15_t,256,float,256> toFloat(fifo0,fifo1); /* Node ID = 10 */

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        EventRecord2 (Evt_Scheduler, nbSchedule, 0);
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 122; id++)
        {
            EventRecord2 (Evt_Node, schedule[id], 0);
            CG_BEFORE_NODE_EXECUTION(schedule[id]);

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = detectionRec.run();
                }
                break;

                case 1:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 2:
                {
                   cgStaticError = dup1.run();
                }
                break;

                case 3:
                {
                   cgStaticError = energyRec.run();
                }
                break;

                case 4:
                {
                   cgStaticError = iir.run();
                }
                break;

                case 5:
                {
                   cgStaticError = player.run();
                }
                break;

                case 6:
                {
                   cgStaticError = rms.run();
                }
                break;

                case 7:
                {
                   cgStaticError = signalRec.run();
                }
                break;

                case 8:
                {
                   cgStaticError = threshold.run();
                }
                break;

                case 9:
                {
                   cgStaticError = toFixedPoint.run();
                }
                break;

                case 10:
                {
                   cgStaticError = toFloat.run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION(schedule[id]);
            if (cgStaticError<0)
            {
                EventRecord2 (Evt_Error, cgStaticError, 0);
            }
            CHECKERROR;
        }
       CG_AFTER_ITERATION;
       nbSchedule++;
    }

errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
}
