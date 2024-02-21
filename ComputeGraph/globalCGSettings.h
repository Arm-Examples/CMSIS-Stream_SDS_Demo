/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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
#ifndef _GLOBAL_CG_SETTINGS_H_
#define _GLOBAL_CG_SETTINGS_H_

#include "cmsis_os2.h" 
#include "EventRecorder.h"

// Generated from Python to ensure the
// settings are consistent between C and Python
// The file contains:
// DSP block size 
//     Size of blocks produced and consumed by the
//     DSP blocks like player, IIR filter ...

#include "params.h"

#include "graph_context.h"

// Checked if a recorder error occured. If yes,
// stop the graph execution
// It assumes no recorder is used outside of the
// graph
#define CG_AFTER_NODE_EXECUTION(ID)                         \
  if ((cg->recorderHasError) || (cg->playerHasError))       \
  {                                                         \
    cgStaticError = CG_SDS_NODE_ERROR;                      \
    goto errorHandling;                                     \
  }


#endif