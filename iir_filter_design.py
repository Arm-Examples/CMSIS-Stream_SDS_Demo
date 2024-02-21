import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import matplotlib.ticker
import argparse
    
import matplotlib.patches as mpatches

parser = argparse.ArgumentParser(description='Generate IIR coefficients')
parser.add_argument('-p', action='store_true', help="Plot transfer function")
parser.add_argument('-t', action='store_true', help="Test filter")

args = parser.parse_args()


sampling_frequency = 8000
gpass = 0.1
gstop = 60
iir_type = 'ellip'


sweep_low = 400 
sweep_high = 3000
sweep_middle = (sweep_high + sweep_low) / 2.0

passband_edge_frequency = sweep_middle - 100 
stopband_edge_frequency = sweep_middle + 100 

wp = [passband_edge_frequency / (sampling_frequency / 2.0),
      stopband_edge_frequency / (sampling_frequency / 2.0)]


ws = [wp[0]-0.01,wp[1]+0.01]


header = """/*
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

#define NUMSTAGES       %d"""



f32_iirdef = """/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
float32_t                     iirState_f32[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_f32  iirInstance_f32; 

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
arm_biquad_mod_coef_f32 mveCoefs[NUMSTAGES];
#endif

float32_t                     iirCoeff_f32[NUMSTAGES * 5] =
                                 {
%s
                                 };
"""



end_of_file="""/*----------------------------------------------------------------------------
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


"""

F32 = 0


def compute_normalization(x):
    nb = 0 
    while(x>=1.0):
        x = x / 2
        nb = nb + 1 
    return(nb)

def get_value(dt,shift,val):
    if dt == F32:
        return(f"{val:+0.10f}")
   

def gen_c_array(dt,shift,coef):
    section = 1
    while True:
        try:
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}   B0 */\n")
            
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}   B1 */\n")
            
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}   B2 */\n")
            
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}  -A1 */\n")
            
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}  -A2 */\n")
            section = section + 1
        except StopIteration:
            break

def gen_c_code(dt,coef_list):
    max_value = np.max(np.abs(np.array(coef_list)))
    coef = iter(coef_list)
    shift = compute_normalization(max_value)
    code = gen_c_array(dt,shift,coef)
    return(shift,"".join(code))


if args.p:
   system = signal.iirdesign(wp, ws, gpass, gstop,ftype=iir_type)
   
   w, h = signal.freqz(*system)
   
   fig, ax1 = plt.subplots()
   ax1.set_title('Digital filter frequency response')
   ax1.plot(w, 20 * np.log10(abs(h)), 'b')
   ax1.set_ylabel('Amplitude [dB]', color='b')
   ax1.set_xlabel('Frequency [rad/sample]')
   ax1.grid(True)
   ax1.set_ylim([-120, 20])

   # Draw axis for frequency in the middle of the sine sweep
   xavg = 2.0*np.pi * sweep_middle / sampling_frequency
   ax1.axvline(xavg , color = 'r')

   xmin = 2.0*np.pi * sweep_low / sampling_frequency
   ax1.axvline(xmin , color = 'y')

   xmax = 2.0*np.pi * sweep_high / sampling_frequency
   ax1.axvline(xmax , color = 'y')


   rect=mpatches.Rectangle((xmin,-120),(xmax-xmin),140, 
                        fill = True,
                        color = "lightgray",
                        linewidth = 1)
   ax1.add_patch(rect)
   
   

   ax2 = ax1.twinx()
   angles = np.unwrap(np.angle(h))
   ax2.plot(w, angles, 'g')
   ax2.set_ylabel('Angle (radians)', color='g')
   ax2.grid(True)
   ax2.axis('tight')
   ax2.set_ylim([-6, 1])
   nticks = 8
   ax1.yaxis.set_major_locator(matplotlib.ticker.LinearLocator(nticks))
   ax2.yaxis.set_major_locator(matplotlib.ticker.LinearLocator(nticks))
   
   plt.show()

elif args.t:
   sos = signal.iirdesign(wp, ws, gpass, gstop,ftype=iir_type,output='sos')
   SAMPLING_FREQ = 8000
   MIN_FREQ = 0
   MAX_FREQ = SAMPLING_FREQ / 2.0
   l=list(np.linspace(-32768,32767,65536,dtype=np.int16))
   t = (np.array(l)+32768) / SAMPLING_FREQ
   duration = 65536.0 / SAMPLING_FREQ
   df = np.linspace(MIN_FREQ,MAX_FREQ,len(l),dtype=float)
   sig = np.cos(2*np.pi*t*df)
   res=signal.sosfilt(sos,sig)
   plt.plot(t,res)
   plt.show()

else:
   sos = signal.iirdesign(wp, ws, gpass, gstop,ftype=iir_type,output='sos')
   num_stages = len(sos)
   #print(f"Num stages = {num_stages}")
   nb_coefficients = num_stages*5
   
   
   coefs=np.reshape(np.hstack((sos[:,:3],-sos[:,4:])),nb_coefficients)
   
   with open("ComputeGraph/DSP/DSP_IIR.c","w") as f:
        print(header % num_stages,file=f)
        
        shift, f32_code = gen_c_code(F32,coefs)
        print(f32_iirdef % f32_code,file=f)
        
        
        print(end_of_file,file=f)   