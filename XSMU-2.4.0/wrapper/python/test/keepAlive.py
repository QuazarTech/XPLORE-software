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
# Sends Keep_Alive Packet to device

timeout       = 5.0
lease_time_ms = 10000

lease_time_ms, timeout = libxsmu.keepAlive (deviceID, lease_time_ms, timeout)
print \
	"Lease Time: ", lease_time_ms, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in keepAlive'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
