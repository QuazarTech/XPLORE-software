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
# Load default calibration

timeout = 1.0
timeout = libxsmu.CS_loadDefaultCalibration (deviceID, timeout)

print "Remaining time        :", timeout, "sec", "\n"

if (timeout == 0.0):
	print 'Communication timeout in CS_loadDefaultCalibration.'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
