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
# Set voltage source calibration table (for range selected above.)

timeout = 5.0
voltage = float(sys.argv[1]) #-10.0
voltage, timeout = libxsmu.VS_setVoltage (deviceID, voltage, timeout)
print \
	"Voltage: ", voltage, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in VS_setVoltage'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
