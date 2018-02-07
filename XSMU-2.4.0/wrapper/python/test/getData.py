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

##########################################################################
# Start recording streamed data from the XSMU

timeout = 5
timeout = libxsmu.StartRec (deviceID, timeout)
print \
	"Started Recording Streamed Data"

sleep (5)

##########################################################################
# Get streamed data from data queue

print \
	"Getting Data"

logfile = open ('log.txt', 'w')

t0 = time.time()
_time_ = 0
while (time.time() - t0 < 60*60):
	print "Remaining time: ", 60 - (time.time() - t0) / 60.0, " minutes" 
	timeout = 5.0
	recSize, timeout = libxsmu.recSize (deviceID, timeout)
	data = libxsmu.getData (deviceID)
	for voltage in data:
		logfile.write (str (_time_) + ", " + str (voltage) + '\n')
		print _time_, ',\t', voltage * 1e9, ' ADC'
		logfile.flush()
		_time_ = _time_ + 0.1
	time.sleep (10)

timeout = 5
timeout = libxsmu.StopRec (deviceID, timeout)
print \
	"Stopped Recording Streamed Data"
##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
