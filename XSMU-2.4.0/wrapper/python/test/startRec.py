import libxsmu, time, math, sys
from time import sleep

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

sleep (5)

##########################################################################
# Start recording streamed data from the SMU

timeout = 5.0
print \
	"Starting Recording Streamed Data"
timeout = libxsmu.StartRec (deviceID, timeout)
print \
	"Started Recording Streamed Data"

sleep(20)

timeout = 5.0
timeout = libxsmu.StopRec (deviceID, timeout)
print \
	"Stopped Recording Streamed Data"

##########################################################################
# closes the device.

sleep(5)
libxsmu.close_device(deviceID)
