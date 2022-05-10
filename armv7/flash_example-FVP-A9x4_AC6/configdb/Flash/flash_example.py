from flashprogrammer.flash_method_v1 import FlashMethodv1
from com.arm.debug.flashprogrammer import FlashProgrammerRuntimeException
from com.arm.debug.flashprogrammer.IProgress import OperationResult
from com.arm.debug.flashprogrammer import TargetStatus

from java.lang import Exception
import os.path

from com.arm.debug.dtsl.interfaces import IDevice
from com.arm.rddi import RDDI
from com.arm.rddi import RDDI_ACC_SIZE
from com.arm.rddi import RDDIException
from com.arm.debug.dtsl import DTSLException
from jarray import zeros
import struct

from flashprogrammer.device import ensureDeviceOpen
from flashprogrammer.execution import ensureDeviceStopped, funcCall
from flashprogrammer.device_memory import writeToTarget

#import pydevd

MEMCPY = [              #    myMemCpy
            0x00, 0x25, #        0x200001c8:    2500        .%      MOVS     r5,#0
                        #    myMemCpyLoop
            0x44, 0x59, #        0x200001ca:    5944        DY      LDR      r4,[r0,r5]
            0x4c, 0x51, #        0x200001cc:    514c        LQ      STR      r4,[r1,r5]
            0x2d, 0x1d, #        0x200001ce:    1d2d        -.      ADDS     r5,r5,#4
            0x52, 0x1e, #        0x200001d0:    1e52        R.      SUBS     r2,r2,#1
            0xfa, 0xd1, #        0x200001d2:    d1fa        ..      BNE      myMemCpyLoop ; 0x200001ca
            0x70, 0x47, #        0x200001d4:    4770        pG      BX       lr
            0x00, 0x00,
    ]

def toBytes(buf):
    # Pack bytes into buffer for writing to target
    return ''.join(map(chr, buf))


class ExampleWriter(FlashMethodv1):
    def __init__(self, methodServices):
        FlashMethodv1.__init__(self, methodServices)


    def setup(self):
        # pydevd.settrace()
        # connect to core & stop
        self.conn = self.getConnection()
        coreName = self.getParameter("coreName")
        self.dev = self.conn.getDeviceInterfaces().get(coreName)
        self.deviceOpened = ensureDeviceOpen(self.dev)
        ensureDeviceStopped(self.dev)

        # get parameters for working RAM
        self.ramAddr = int(self.getParameter("ramAddress"), 0)
        self.ramSize = int(self.getParameter("ramSize"), 0)

        # calculate addresses of algorithm code, stack & write buffer in working RAM
        self.codeAddr = self.ramAddr
        self.stackBottom = self.codeAddr + len(MEMCPY)
        self.stackTop = self.stackBottom + 0x100 - 4
        self.writeBuffer = self.stackTop + 4
        self.writeBufferSize = 1024
        self.pageSize = 1024

        self.debug("Loading algorithm to %08x, stack: %08x..%08x, writeBuffer: %08x" % (
                   self.codeAddr, self.stackBottom, self.stackTop, self.writeBuffer)
                   )

        # load the mem copy routine to working RAM
        writeToTarget(self.dev, self.codeAddr, toBytes(MEMCPY))


    def teardown(self):
        if self.deviceOpened:
            # close device connection if opened by this script
            self.dev.closeConn()
        # register values have been changed
        return TargetStatus.STATE_LOST


    def program(self, regionID, offset, data, allowFullChipErase):
        # calculate the address to write to
        region = self.getRegion(regionID)
        addr = region.getAddress() + offset

        progress = self.operationStarted(
            'Programming 0x%x bytes to 0x%08x' % (data.getSize(), addr),
            100)

        try:
            self.doErase(addr, data.getSize(), self.subOperation(progress, 'Erasing', data.getSize(), 20))
            progress.progress('Erasing completed', 20)
    
            self.doWrite(addr, data, self.subOperation(progress, 'Writing', data.getSize(), 50))
            progress.progress('Writing completed', 20+50)
    
            self.doVerify(addr, data, self.subOperation(progress, 'Verifying', -1, 30))
            progress.progress('Verifying completed', 20+50+30)
    
            progress.completed(OperationResult.SUCCESS, 'All done')
    
        except Exception, e:
            # exceptions may be derived from Java Exception or Python Exception
            # report failure to progress monitor & rethrow
            progress.completed(OperationResult.FAILURE, 'Failed')
            raise

        except java.lang.Exception, e:
            # exceptions may be derived from Java Exception or Python Exception
            # report failure to progress monitor & rethrow
            progress.completed(OperationResult.FAILURE, 'Failed')
            raise

        # register values have been changed
        return TargetStatus.STATE_LOST


    def doErase(self, addr, size, progress):
        try:
            eraseBlock = 0x400
            bytesErased = 0
            while bytesErased < size:
                # blank out memory in 1kb chunks
                self.dev.memFill(0, addr, RDDI_ACC_SIZE.RDDI_ACC_WORD,
                                 RDDI.RDDI_MRUL_NORMAL, False, eraseBlock/4, 0)
    
                addr += eraseBlock
                bytesErased += eraseBlock
                progress.progress('Erased %d bytes' % bytesErased, bytesErased)
    
            progress.completed(OperationResult.SUCCESS, 'Erasing completed')
            
        except (Exception, java.lang.Exception), e:
            # exceptions may be derived from Java Exception or Python Exception
            # report failure to progress monitor & rethrow
            progress.completed(OperationResult.FAILURE, 'Erasing failed')
            raise


    def doWrite(self, addr, data, progress):
        try:
            data.seek(0)
            bytesWritten = 0
            while bytesWritten < data.getSize():
                # get next block of data
                buf = data.getData(self.pageSize)
                # write to buffer
                writeToTarget(self.dev, self.writeBuffer, buf)
                # run copy routine
                args = [ self.writeBuffer, addr, len(buf)/4 ]
                funcAddr = self.codeAddr | 1 # code is Thumb, so set bit 0
                funcCall(self.dev, funcAddr, args, self.stackTop)
    
                bytesWritten += len(buf)
                addr += len(buf)
                progress.progress('Written %d bytes' % bytesWritten, bytesWritten)
    
            progress.completed(OperationResult.SUCCESS, 'Writing completed')
 
        except FlashProgrammerRuntimeException, e:
            progress.completed(OperationResult.FAILURE, 'Writing failed')
            raise
            
        except RDDIException, e:
            progress.completed(OperationResult.FAILURE, 'Writing failed')
            raise
            
        except DTSLException, e:
            progress.completed(OperationResult.FAILURE, 'Writing failed')
            raise

        except Exception, e:
            # exceptions may be derived from Java Exception or Python Exception
            # report failure to progress monitor & rethrow
            progress.completed(OperationResult.FAILURE, 'Writing failed')
            raise

        except java.lang.Exception, e:
            # exceptions may be derived from Java Exception or Python Exception
            # report failure to progress monitor & rethrow
            progress.completed(OperationResult.FAILURE, 'Writing failed')
            raise

        except:
            progress.completed(OperationResult.FAILURE, 'Writing failed')
            raise
            
    def doVerify(self, addr, data, progress):
        try:
            data.seek(0)
            bytesVerified = 0
            while bytesVerified < data.getSize():
                # get next block of data
                buf = data.getData(self.pageSize)
                # read from the destination address
                readBuf = zeros(len(buf), 'b')
                self.dev.memRead(0, addr, RDDI_ACC_SIZE.RDDI_ACC_WORD,
                                 RDDI.RDDI_MRUL_NORMAL, len(readBuf), readBuf)
                # check the buffers match
                if readBuf != buf:
                    raise FlashProgrammerRuntimeException, "Verify failed: %s != %s" % (buf, readBuf)
    
                bytesVerified += len(buf)
                addr += len(buf)
                progress.progress('Verified %d bytes' % bytesVerified, bytesVerified)
     
            progress.completed(OperationResult.SUCCESS, 'Verifying completed')

        except (Exception, java.lang.Exception), e:
            # exceptions may be derived from Java Exception or Python Exception
            # report failure to progress monitor & rethrow
            progress.completed(OperationResult.FAILURE, 'Verify failed')
            raise


        
