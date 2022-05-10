from com.arm.debug.dtsl.configurations import DTSLv1
from com.arm.debug.dtsl.components import Device
from com.arm.debug.dtsl.components import CadiSyncSMPDevice


class fvp_a9x4_SMP(DTSLv1):
    def __init__(self, root):
        DTSLv1.__init__(self, root)

        # Create devices representing each core
        coreDevices = [ Device(self, 1, "Cortex-A9_0") , Device(self, 2, "Cortex-A9_1") , Device(self, 3, "Cortex-A9_2") , Device(self, 4, "Cortex-A9_3")]

        # Expose each core to the debugger
        for c in coreDevices:
            self.addDeviceInterface(c)

        # Create SMP cluster of all cores
        self.smp = CadiSyncSMPDevice(self, "Cortex-A9x4_SMP", coreDevices)
        self.addDeviceInterface(self.smp)
