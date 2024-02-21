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

extern "C" {
#include "cmsis_os2.h"
#include "sds_play.h"
#include "sds_rec.h"

}

#include <iostream>


template<typename OUT,int outputSize>
class SDSPlayer;

template<typename T,int outputSize>
class SDSPlayer: 
 public GenericSource<T,outputSize>
{
public:
    SDSPlayer(FIFOBase<T> &dst,
        const char* name,
        const uint32_t playBuffer_size,
        const uint32_t playBuffer_threshold,
        const uint32_t tmpBuffer_size):
    GenericSource<T,outputSize>(dst),
    mError(false),
    playId(NULL),
    playBuf(NULL),
    tmpBuf(NULL),
    tmpPos(0),
    tmpBufSize(0U)
    {


        const uint32_t bufSize = playBuffer_size != 0 ? playBuffer_size : (outputSize<<1)*sizeof(T);
        const uint32_t bufThreshold = playBuffer_threshold != 0 ? playBuffer_threshold : (bufSize >> 1) ;
        tmpBufSize = tmpBuffer_size != 0 ? tmpBuffer_size : (outputSize<<1)*sizeof(T);

        tmpBuf = (uint8_t*)malloc(tmpBufSize);
        if (tmpBuf==NULL)
        {
            mError=true;
        }
        else 
        {
            playBuf = (uint8_t*)malloc(bufSize);
            if (playBuf==NULL)
            {
                free(tmpBuf);
                tmpBuf = NULL;
                mError=true;
            }
            else
            {
                playId = sdsPlayOpen(name,
                                     playBuf,
                                     bufSize,
                                     bufThreshold);
                
                if (playId == NULL)
                {
                    free(playBuf);
                    free(tmpBuf);
                    playBuf = NULL;
                    tmpBuf = NULL;
                    mError = true;
                }
            }
        }

        tick = osKernelGetTickCount();


    };

    ~SDSPlayer()
    {
        if (playId != NULL)
        {
           sdsPlayClose(playId);
        }

        if (playBuf!=NULL)
        {
           free(playBuf);
        }

        if (tmpBuf!=NULL)
        {
           free(tmpBuf);
        }
    }

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final
    {

        uint32_t  timestamp;

        T *output=this->getWriteBuffer();
        uint8_t *buf = reinterpret_cast<uint8_t*>(output);
        if (mError)
        {
            return(CG_INIT_FAILURE);
        }

        uint32_t size=0U;
        uint32_t remaining = outputSize*sizeof(T);
        uint8_t *p = buf;

        // Read remaining bytes to fill the output buffer of this
        // node
        while(remaining > 0)
        {
            // Try to get a record from the player
            do {
                size = sdsPlayGetSize(playId);

                if (size == 0U) 
                {
                   if (sdsPlayEndOfStream(playId) != 0) 
                   {
                      return(CG_STOP_SCHEDULER);
                   }
                }
            } while (size == 0U);

            // A full record of size is available
            // Check there is enough space in destination tmpBuffer
            if ((tmpBufSize-tmpPos) < size)
            {
               return(CG_BUFFER_OVERFLOW);
            }

            // Read the record and add it to the tmp buffer
            if (size > 0)
            {
               uint32_t read = sdsPlayRead(playId, &timestamp, tmpBuf+tmpPos, size);
               if (read < size)
               {
                 return(CG_BUFFER_UNDERFLOW);
               }
               tmpPos += size;
            }
            uint32_t toCopy;

            // If there is some data from the tmp buffer, we read some
            // to fill the output buffer
            if (tmpPos>0)
            {
                // Read what's available up to the maximun needed
                if (tmpPos >= remaining)
                {
                    toCopy = remaining;
                }
                else 
                {
                    toCopy = tmpPos;
                }

                // Copy data to output buffer
                memcpy(p,tmpBuf,toCopy);
                p += toCopy;
                remaining -= toCopy;

                // Update the temp buffer
                memmove(tmpBuf,tmpBuf+toCopy,tmpPos-toCopy);
                tmpPos -= toCopy;
            }
            
        }

        // Delay until timestamp
        osDelayUntil(tick + timestamp);
        
        // End of execution for this node
        return(CG_SUCCESS);

    };

protected:
    bool mError;
    sdsPlayId_t playId;
    uint8_t *playBuf;
    uint8_t *tmpBuf;
    uint32_t tmpPos;
    uint32_t tick;
    uint32_t tmpBufSize;
};

#undef PLAY_BUF_SIZE
#undef PLAY_IO_THRESHOLD
#undef TEMP_BUF_SIZE


template<typename IN,int inputSize>
class SDSRecorder: GenericSink<IN,inputSize>
{
public:
    SDSRecorder(FIFOBase<IN> &src,
                const char* fileName,
                const uint32_t bufferSize = 0,
                const uint32_t threshold = 0):
    GenericSink<IN,inputSize>(src),
    buf(NULL),
    mRecId(NULL),
    mError(false)
    {
        
        // Open the recorder using the
        // configuration passed as argument
                      
        const uint32_t bufSize = bufferSize != 0 ? bufferSize : (inputSize << 1)*sizeof(IN);
        const uint32_t bufThreshold = threshold != 0 ? threshold : (bufSize >> 1);

        buf=malloc(bufSize);

        if (buf != NULL)
        {

            mRecId = sdsRecOpen(fileName,
                                buf,
                                bufSize,
                                bufThreshold);
            if (mRecId == NULL)
            {
                free(buf);
                buf=NULL;
                mError = true;
            }
        }
        else 
        {
            mError = true;
        }
        
    };

    ~SDSRecorder()
    {
        if (mRecId != NULL)
        {
           sdsRecClose(mRecId); 
        }
        if (buf != NULL)
        {
            free(buf);
        }
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        IN *b=this->getReadBuffer();
        if (mError)
        {
            return(CG_INIT_FAILURE);
        }

        uint32_t timestamp;
        timestamp = osKernelGetTickCount();


        /*

        Try to write all the input buffer to the SDS recorder
        stream.

        The timestamp used corresponds to when this SDS recorder node
        is executed. This time may be different from the time when
        the samples have been received by the CMSIS-Stream graph.

        SDS framework is recovering timestamps using the known
        sampling frequency of the sensors so in general it is
        not a problem if this timestamp used here is not accurate.

        */
        uint32_t num = sdsRecWrite(mRecId, timestamp, b, inputSize*sizeof(IN));
        
        /*

        Check for SDS Recorder overflow

        */
        if (num < inputSize*sizeof(IN)) {
            return(CG_BUFFER_OVERFLOW);
        }
        return(CG_SUCCESS);
    };

protected:
    sdsRecId_t mRecId;
    void *buf;
    bool mError;
};