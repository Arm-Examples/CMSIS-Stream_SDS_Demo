from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource

##############################
#
# Define new types of Nodes 
#

# Converting a block of F32 samples to Q15
class ToFixedPoint(GenericNode):
    def __init__(self,name,ioLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),ioLength)
        self.addOutput("o",CType(Q15),ioLength)

    @property
    def typeName(self):
        return "ToFixedPoint"


# Converting a block of Q15 samples to F32
class ToFloat(GenericNode):
    def __init__(self,name,ioLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(Q15),ioLength)
        self.addOutput("o",CType(F32),ioLength)

    @property
    def typeName(self):
        return "ToFloat"

# IIR node
class IIR(GenericNode):
    def __init__(self,name,ioLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),ioLength)
        self.addOutput("o",CType(F32),ioLength)

    @property
    def typeName(self):
        return "IIR"


# Debug node
class Debug(GenericSink):
    def __init__(self,name,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",CType(Q15),inLength)

    @property
    def typeName(self):
        return "Debug"

# SDS Recorder.
# Work with any standard datatype (f32, q15 ...)
class SDSRecorder(GenericSink):
    def __init__(self,name,theType,inLength,filename=None,
        buffer_size=0,
        buffer_threshold=0):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addLiteralArg(filename)
        if isinstance(buffer_size, str):
            self.addVariableArg(buffer_size)
        else:
            self.addLiteralArg(buffer_size)

        if isinstance(buffer_threshold, str):
            self.addVariableArg(buffer_threshold)
        else:
            self.addLiteralArg(buffer_threshold)



    @property
    def typeName(self):
        return "SDSRecorder"

# SDS Player node
# Should work with any data type
# (must be coherent with the yml file)
class SDSPlayer(GenericSource):
    def __init__(self,name,theType,outLength,
        sensor_name="Audio",
        buffer_size=0,
        buffer_threshold=0,
        tmp_size=0):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)
        self.addLiteralArg(sensor_name)

        if isinstance(buffer_size, str):
            self.addVariableArg(buffer_size)
        else:
            self.addLiteralArg(buffer_size)

        if isinstance(buffer_threshold, str):
            self.addVariableArg(buffer_threshold)
        else:
            self.addLiteralArg(buffer_threshold)

        if isinstance(tmp_size, str):
            self.addVariableArg(tmp_size)
        else:
            self.addLiteralArg(tmp_size)


    @property
    def typeName(self):
        return "SDSPlayer"

# Decision node : Comparison with a threshold
class Threshold(GenericNode):
    def __init__(self,name,threshold_value=0.5):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),1)
        self.addOutput("o",CType(F32),1)
        if isinstance(threshold_value, str):
           self.addVariableArg(threshold_value)
        else:
           self.addLiteralArg(threshold_value)


    @property
    def typeName(self):
        return "Threshold"

# Root mean square of a window of samples using
# CMSIS-DSP
class RMS(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),inLength)
        self.addOutput("o",CType(F32),1)

    @property
    def typeName(self):
        return "RMS"



