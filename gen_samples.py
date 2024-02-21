import struct
import numpy as np
import argparse

parser = argparse.ArgumentParser(description="Generate dummy audio signal")

parser.add_argument('-d', action='store_true', help="Debug signal")
parser.add_argument('-s', action='store_true', help="Square signal")

args = parser.parse_args()


def getDataType(data_type):
    match data_type:
        case "int16_t":
            return "h"
        case "uint16_t":
            return "H"
        case "int32_t":
            return "i"
        case "uint32_t":
            return "I"
        case "float":
            return "f"
        case "double":
            return "d"
        case _:
            print(f"Unknown data type: {data_type}\n")
            return None

# To generate a signal
SAMPLING_FREQ = 8000
MIN_FREQ = 0
MAX_FREQ = SAMPLING_FREQ / 2.0

l=list(np.linspace(-32768,32767,65536,dtype=np.int16))

t = (np.array(l)+32768) / SAMPLING_FREQ
duration = 65536.0 / SAMPLING_FREQ

if not args.d and not args.s:
    df = np.linspace(MIN_FREQ,MAX_FREQ,len(l),dtype=float)
    signal = np.floor(32767*np.cos(2*np.pi*t*df)).astype(np.int16)
    l = signal

if args.s:
    signal = np.floor(32767*np.heaviside(t-duration/2.0,0.5)).astype(np.int16)
    l = signal


NBSAMPLES = 128

dt = int(np.floor(1000.0*NBSAMPLES / SAMPLING_FREQ))

s = np.array_split(l,len(l)//NBSAMPLES)
d_type = getDataType("int16_t")

timestamp = 0
with open("Audio.0.sds","wb") as of:
  for block in s:
      timestamp = timestamp + dt 
      if (len(block)<NBSAMPLES):
        delta = NBSAMPLES - len(block)
        block = np.pad(block, [(0, delta)], mode='constant')
      samples=struct.pack(f"II{int(len(block))}{d_type}", timestamp,2*NBSAMPLES,*block)
      of.write(samples)
