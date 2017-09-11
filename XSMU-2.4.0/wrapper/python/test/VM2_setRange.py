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

print (
	"Device ID     :" + str (deviceID) + "\n" +
	"goodID        :" + str (goodID)   + "\n" +
	"Remaining time:" + str (timeout)  + "sec")

if (timeout == 0.0) or (not goodID):
	print 'Communication timeout in open_device.'
	exit (-2)

##########################################################################
# Selects source range to 1mA

VM2_RANGE_10V = 0

timeout = 1.0
voltageMeterRange = VM2_RANGE_10V

(voltageMeterRange, timeout) =
	libxsmu.VM2_setRange (deviceID, voltageMeterRange, timeout)

print (
	"voltageMeterRange     :" + str (voltageMeterRange) + "\n" +
	"Remaining time        :" + str (timeout) + "sec")

if (timeout == 0.0):
	print 'Communication timeout in VM2_setRange.'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
