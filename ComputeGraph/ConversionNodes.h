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
#ifndef _CONVERSION_NODES_H_ 
#define _CONVERSION_NODES_H_ 

/*

Conversions between float and fixed point.

*/

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class ToFixedPoint;

// The node is specialized to the right
// input / output datatypes : f32 -> q15
template<int inputSize>
class ToFixedPoint<float32_t,inputSize,
               q15_t,inputSize>: 
          public GenericNode<float32_t,inputSize,q15_t,inputSize>
{
public:
    ToFixedPoint(FIFOBase<float32_t> &src,FIFOBase<q15_t> &dst):
    GenericNode<float32_t,inputSize,q15_t,inputSize>(src,dst)
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
        q15_t *dst=this->getWriteBuffer();
        arm_float_to_q15(src, dst, inputSize); 
        return(CG_SUCCESS);
    };



};


template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class ToFloat;


template<int inputSize>
class ToFloat<q15_t,inputSize,
               float32_t,inputSize>: 
          public GenericNode<q15_t,inputSize,float32_t,inputSize>
{
public:
    ToFloat(FIFOBase<q15_t> &src,FIFOBase<float32_t> &dst):
    GenericNode<q15_t,inputSize,float32_t,inputSize>(src,dst)
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
        q15_t *src=this->getReadBuffer();
        float32_t *dst=this->getWriteBuffer();
        arm_q15_to_float(src, dst, inputSize); 
        return(CG_SUCCESS);
    };



};

#endif