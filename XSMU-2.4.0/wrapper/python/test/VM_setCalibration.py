import libxsmu, time, math

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
# Set voltage source calibration table (for range selected above.)

timeout = 1.0
i, adc, voltage, timeout = \
	libxsmu.VM_setCalibration (deviceID, 4, 100.878, timeout)
print i, adc, voltage, timeout

if (timeout == 0.0):
	print 'Communication timeout in VM_setCalibration'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
