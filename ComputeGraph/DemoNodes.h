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
#ifndef _DSP_NODES_H_
#define _DSP_NODES_H_

#include "cg_status.h"

using namespace arm_cmsis_stream;

#include "IIR.h"
#include "ConversionNodes.h"
#include "SDSNodes.h"



template<typename IN, int inputSize>
class Debug;

// Debug node writing its input to stdout
template<int inputSize>
class Debug<q15_t,inputSize>:
public GenericSink<q15_t, inputSize>
{
public:
    Debug(FIFOBase<q15_t> &src):
    GenericSink<q15_t,inputSize>(src)
    { 
    };

    
    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };


    int run() final
    {
        q15_t *input=this->getReadBuffer();
        for(int i=0;i<inputSize;i++)
        {
            printf("%d\r\n",input[i]);
        }
        
        return(CG_SUCCESS);
    };

};


// Compare the input to a threshold and retunr
// 1 when above the threshold
template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class Threshold;

// Float 32 version
template<int inputSize>
class Threshold<float32_t,inputSize,
                float32_t,inputSize>: 
          public GenericNode<float32_t,inputSize,
                             float32_t,inputSize>
{
public:
    Threshold(FIFOBase<float32_t> &src,
              FIFOBase<float32_t> &dst,
              float32_t threshold):
    GenericNode<float32_t,inputSize,
                float32_t,inputSize>(src,dst),mThreshold(threshold)
    {
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

        if (src[0] >= mThreshold)
        {
            dst[0] = 1.0f;
        }
        else
        {
            dst[0] = 0.0f;
        }
        
        return(CG_SUCCESS);
    };

protected:
    float32_t mThreshold;

};


// RMS node
template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class RMS;

template<int inputSize>
class RMS<float32_t,inputSize,
          float32_t,1>: 
public GenericNode<float32_t,inputSize,
                   float32_t,1>
{
public:
    RMS(FIFOBase<float32_t> &src,
        FIFOBase<float32_t> &dst):
    GenericNode<float32_t,inputSize,
                float32_t,1>(src,dst)
    {
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
        float32_t mean;
        
        // Remove DC offset
        arm_mean_f32(src,inputSize,&mean);
        mean = -mean;
        arm_offset_f32(src,mean,src,inputSize);

        // Compute RMS
        arm_rms_f32(src,inputSize,dst);
        
        return(CG_SUCCESS);
    };
};

#endif