settings = """#ifndef _CPARAMS_H_
#define _CPARAMS_H_ 

// Generated from Python. Don't edit
#define DSP_BLOCKSIZE %d

#endif
"""

def write_c_params(dsp_blocksize=128):
    with open("ComputeGraph/params.h","w") as f:
        print(settings % (dsp_blocksize), file = f)
