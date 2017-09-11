import libxsmu, time, math, sys

##########################################################################
# Scans USB bus for Xplore SMU.

N = libxsmu.scan()
print "Total device:", N

if N == 0:
	print 'No Xplore SMU device found.'
	exit (-1)

##########################################################################
# Queries serial number of the first device.
# This should be sufficient if only a single device is present.

serialNo = libxsmu.serialNo(0)
print "Seial number:", serialNo

timeout = 1.0
deviceID, goodID, timeout = libxsmu.open_device (serialNo, timeout)
print \
	"Device ID     :", deviceID, "\n" \
	"goodID        :", goodID, "\n" \
	"Remaining time:", timeout, "sec", "\n"

if (timeout == 0.0) or (not goodID):
	print 'Communication timeout in open_device.'
	exit (-2)

##########################################################################
# Selects source range to 1mA

timeout = 1.0
voltageMeterRange = int(sys.argv [1]) # 0: 1mV, 1: 10mV, 2: 100mV, 3: 1V, 4: 10V, 5: 100V
voltageMeterRange, timeout = \
	libxsmu.VM_setRange (deviceID, voltageMeterRange, timeout)
print \
	"voltageMeterRange    :", voltageMeterRange, "\n" \
	"Remaining time        :", timeout, "sec", "\n"

if (timeout == 0.0):
	print 'Communication timeout in VM_setRange.'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
