/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "main.h"
#include "cmsis_os2.h"

#include "graph_context.h"
#include "dsp_scheduler.h"
#include "cg_status.h"


/*

Context used to communicate between the graph and the main application

*/
static graph_context cg;


/*

Record and error in the graph context

*/
static void recorder_event_callback (sdsRecId_t id, uint32_t event) {
  if (event & SDS_REC_EVENT_IO_ERROR) 
  {
      cg.recorderHasError = 1;
      cg.recorderWithError = id;
  }
}



// Player event callback
static void player_event_callback (sdsPlayId_t id, uint32_t event) {
  if (event & SDS_PLAY_EVENT_IO_ERROR) 
  {
     cg.playerHasError = 1;
     cg.playerWithError = id;
  }
}

// Sensor Demo
static __NO_RETURN void demo (void *argument) 
{
  int error;
  int res;

  /*

  Set error flag to 0 in the graph context

  */
  cg.recorderHasError = 0;
  cg.playerHasError = 0;

 
  sdsRecInit(recorder_event_callback);
  sdsPlayInit(player_event_callback);

    
  printf("Start compute graph\r\n");
  uint32_t nbIterations = dsp_scheduler(&error,&cg);
    
  printf("Nb iterations = %d\r\n",nbIterations);

  switch(error)
  {
     case CG_BUFFER_UNDERFLOW:
      printf("Buffer underflow\r\n");
      break;
     case CG_BUFFER_OVERFLOW:
      printf("Buffer overflow\r\n");
      break;
     case CG_MEMORY_ALLOCATION_FAILURE:
      printf("Memory allocation failure\r\n");
      break;
     case CG_INIT_FAILURE:
      printf("Initialization failure\r\n");
      break;
     case CG_BUFFER_ERROR:
      printf("Buffer allocation error\r\n");
      break;
     case CG_OS_ERROR:
      printf("OS Error\r\n");
      break;
     case CG_STOP_SCHEDULER:
      printf("Normal end of the scheduler\r\n");
      break;
     case CG_SDS_NODE_ERROR:
      printf("Error occured in a SDS node:\r\n");
      if (cg.playerHasError==1)
      {
         printf("  Error in a player.\r\n");
      }
      if (cg.recorderHasError==1)
      {
         printf("  Error in a recorder.\r\n");
      }
      break;
     default:
      printf("Unknown error %d\r\n",error);
  }

  // Only for demo with virtual hardware
  exit(0);
}

// Application initialization
int32_t app_initialize (void) {
  osThreadNew(demo, NULL, NULL);
  return 0;
}
