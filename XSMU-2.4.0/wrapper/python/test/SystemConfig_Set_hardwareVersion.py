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

timeout = 5.0
deviceID, goodID, timeout = libxsmu.open_device (serialNo, timeout)
print \
	"Device ID     :", deviceID, "\n" \
	"goodID        :", goodID, "\n" \
	"Remaining time:", timeout, "sec", "\n"

if (timeout == 0.0) or (not goodID):
	print 'Communication timeout in open_device.'
	exit (-2)

##########################################################################
# Sets PCB version from EEPROM

timeout = 1.0
hardwareVersion = libxsmu.make_version (4, 0, 0)
hardwareVersion, timeout = (
	libxsmu.SystemConfig_Set_hardwareVersion (
		deviceID, hardwareVersion, timeout))

print \
	"Hardware version :", \
		libxsmu.major_version_no (hardwareVersion), ".", \
		libxsmu.minor_version_no (hardwareVersion), ".", \
		libxsmu.bugfix_version_no (hardwareVersion), "\n", \
	"Remaining time   :", timeout, "sec", "\n"

if (timeout == 0.0):
	print 'Communication timeout in SystemConfig_Set_hardwareVersion.'
	exit (-2)

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
