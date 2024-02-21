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
#ifndef _IIR_H_ 
#define _IIR_H_

extern "C" {
#include "DSP_IIR.h"
}

/*

Wrappers around the DSP_IIR.c implementations.
Filter coefficients cannot be passed to those wrappers
since they are hardcoded into DSP_IIR.c

*/

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class IIR;

template<int inputSize>
class IIR<float32_t,inputSize,
          float32_t,inputSize>: 
          public GenericNode<float32_t,inputSize,float32_t,inputSize>
{
public:
    IIR(FIFOBase<float32_t> &src,FIFOBase<float32_t> &dst):
    GenericNode<float32_t,inputSize,float32_t,inputSize>(src,dst)
    {
        iirInit_f32();
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow() ||
            this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        float32_t *src=this->getReadBuffer();
        float32_t *dst=this->getWriteBuffer();
        iirExec_f32 (src,dst);
        return(CG_SUCCESS);
    };



};




#endif