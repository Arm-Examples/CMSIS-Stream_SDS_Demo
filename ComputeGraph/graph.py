# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

# Import definitions used to describe the graph
from nodes import *
from params import * 

import argparse

parser = argparse.ArgumentParser(description='Options for graph generation')
parser.add_argument('-r', action='store_true', help="Replay mode")

args = parser.parse_args()

##############################
#
# Description of the graph 
#
DSP_BLOCK_SIZE = 256
ENERGY_BLOCK_SIZE = 384
LOG_REC_BLOCK = 10

# Write parameters to a C header file
write_c_params(dsp_blocksize=DSP_BLOCK_SIZE)

# Creation of the nodes used in this demo

# Player is generating Q15 in this demo.
# In normal mode, the player is playing the sds file "Audio"
# In replay mode, the player is playing the sds file "AudioRec"
if args.r:
   player=SDSPlayer("player",CType(Q15),DSP_BLOCK_SIZE,"AudioRec")
else:
   player=SDSPlayer("player",CType(Q15),DSP_BLOCK_SIZE,"Audio")

# In replay mode, we do not record the audio signal anymore
# So the recorder node for audio is not used
if not args.r:
   rec=SDSRecorder("signalRec",CType(Q15),DSP_BLOCK_SIZE,"AudioRec")


energyRec=SDSRecorder("energyRec",CType(F32),LOG_REC_BLOCK,"EnergyRec")
detectionRec=SDSRecorder("detectionRec",CType(F32),LOG_REC_BLOCK,"DetectionRec")

to_fixed=ToFixedPoint("toFixedPoint",DSP_BLOCK_SIZE)
to_float=ToFloat("toFloat",DSP_BLOCK_SIZE)

# In replay mode, we replay the filtered audio signal so
# we do not need to filter the signal any more and the IIR node
# is no more used.
if not args.r:
   dsp_filter=IIR("iir",DSP_BLOCK_SIZE)

energy=RMS("rms",ENERGY_BLOCK_SIZE)
detection=Threshold("threshold",threshold_value=0.7)


# Create a Graph object
the_graph = Graph()


the_graph.connect(player.o,to_float.i)


# In replay mode, we do not filter the audio signal
# anymore since we are replaying the filtered signal.
# So the part of the graph related to filtering and recording
# of the audio is no more needed
if not args.r:
   the_graph.connect(to_float.o,dsp_filter.i)
   the_graph.connect(dsp_filter.o,to_fixed.i)
   the_graph.connect(to_fixed.o,rec.i)

   the_graph.connect(dsp_filter.o,energy.i)
else:
   the_graph.connect(to_float.o,energy.i)

the_graph.connect(energy.o,detection.i)
the_graph.connect(energy.o,energyRec.i)
the_graph.connect(detection.o,detectionRec.i)


##############################
#
# Generation of the code and 
# graphviz representation
#
print("Generate graphviz and code")

conf=Configuration()

# Argument of the scheduler function.
# Variables used in the initialization of some nodes
conf.cOptionalArgs=["graph_context *cg"]

# Name of files included by the scheduler
conf.appNodesCName="DemoNodes.h"
conf.customCName="globalCGSettings.h"
conf.schedulerCFileName="dsp_scheduler"
# Prefix for FIFO buffers
conf.prefix="dsp_"
# Name of scheduler function
conf.schedName="dsp_scheduler"
conf.CMSISDSP = True

# Enable event recorder tracing in the scheduler
conf.eventRecorder=True


sched = the_graph.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode("ComputeGraph",conf)

# GenericNodes.h is created in the folder "generated"
generateGenericNodes("ComputeGraph")

# cg_status.h is created in the folder "generated"
#generateCGStatus("ComputeGraph")
# Additonal error codes are used in this demo so we do 
# not regenerate the cg_status.h file again

# Create some styling for the graph
# (using graphviz color names)
class MyStyle(Style):

    # Colorize the SDSPlayer and SDSRecorder nodes
    def node_color(self,node):
        if node.typeName == "SDSRecorder":
           return("burlywood1")
        if node.typeName == "SDSPlayer":
           return("khaki")
        return super().node_color(node)

    # Different colors for float edges and q15 edges
    def edge_color(self,edge):
        s = self.edgeSrcPort(edge)
        d = self.edgeDstPort(edge)
        if s.theType.ctype=="float" or s.theType.ctype=="float32_t":
            return("red")
        else:
            return("darkblue")
        
        #return(super().edge_color(edge))

with open("ComputeGraph/Pictures/graph.dot","w") as f:
    myStyle=MyStyle()
    sched.graphviz(f,config=conf,style=myStyle)
