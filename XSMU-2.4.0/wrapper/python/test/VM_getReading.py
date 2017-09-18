import libxsmu, time, math
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
# Selects source range to 1mA

logfile = open ('log.txt', 'w')

for n in range (0, 100):
	filter_length = 32
	timeout = 1 + 0.03 * filter_length
	voltage, timeout = libxsmu.VM_getReading (deviceID, filter_length, timeout)
	print \
		"voltage               :", voltage, "\n" \
		"Remaining time        :", timeout, "sec", "\n"

	logfile.write (str (voltage) + '\n')
	logfile.flush()
	#sleep (1)

if (timeout == 0.0):
	print 'Communication timeout in VM_getReading.'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
