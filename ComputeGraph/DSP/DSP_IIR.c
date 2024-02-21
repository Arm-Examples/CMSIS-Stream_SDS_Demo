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
 
#include <math.h>
#include "arm_math.h"
#include "DSP_IIR.h"

#define NUMSTAGES       7
/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
float32_t                     iirState_f32[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_f32  iirInstance_f32; 

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
arm_biquad_mod_coef_f32 mveCoefs[NUMSTAGES];
#endif

float32_t                     iirCoeff_f32[NUMSTAGES * 5] =
                                 {
         +0.0005417488,  /* section  1   B0 */
         +0.0000000000,  /* section  1   B1 */
         -0.0005417488,  /* section  1   B2 */
         +0.4513013891,  /* section  1  -A1 */
         -0.9272611378,  /* section  1  -A2 */
         +1.0000000000,  /* section  2   B0 */
         -0.0660055252,  /* section  2   B1 */
         +1.0000000000,  /* section  2   B2 */
         +0.3673880693,  /* section  2  -A1 */
         -0.9434605941,  /* section  2  -A2 */
         +1.0000000000,  /* section  3   B0 */
         -0.8341951022,  /* section  3   B1 */
         +1.0000000000,  /* section  3   B2 */
         +0.5412177502,  /* section  3  -A1 */
         -0.9446630419,  /* section  3  -A2 */
         +1.0000000000,  /* section  4   B0 */
         -0.2255739376,  /* section  4   B1 */
         +1.0000000000,  /* section  4   B2 */
         +0.3225721603,  /* section  4  -A1 */
         -0.9717005903,  /* section  4  -A2 */
         +1.0000000000,  /* section  5   B0 */
         -0.6973225706,  /* section  5   B1 */
         +1.0000000000,  /* section  5   B2 */
         +0.5964276222,  /* section  5  -A1 */
         -0.9726435976,  /* section  5  -A2 */
         +1.0000000000,  /* section  6   B0 */
         -0.2608531811,  /* section  6   B1 */
         +1.0000000000,  /* section  6   B2 */
         +0.3071651926,  /* section  6  -A1 */
         -0.9920790142,  /* section  6  -A2 */
         +1.0000000000,  /* section  7   B0 */
         -0.6656717807,  /* section  7   B1 */
         +1.0000000000,  /* section  7   B2 */
         +0.6198150081,  /* section  7  -A1 */
         -0.9923797882,  /* section  7  -A2 */

                                 };

/*----------------------------------------------------------------------------
   Initialize the IIR data structure  
 *---------------------------------------------------------------------------*/
void iirInit_f32 (void) {
#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
  arm_biquad_cascade_df1_mve_init_f32(&iirInstance_f32,NUMSTAGES,iirCoeff_f32,mveCoefs,iirState_f32);
#else
  arm_biquad_cascade_df1_init_f32(&iirInstance_f32, NUMSTAGES, iirCoeff_f32, iirState_f32); 
#endif
}


/*----------------------------------------------------------------------------
   Execute the IIR processing function  
 *---------------------------------------------------------------------------*/
void iirExec_f32 (float32_t *pSrc, float32_t *pDst) {

  arm_biquad_cascade_df1_f32(&iirInstance_f32, pSrc, pDst, DSP_BLOCKSIZE); 
}



