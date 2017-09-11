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

timeout = 1.0
SOURCE_MODE_CS = 0
SOURCE_MODE_VS = 1
sourceMode, timeout = \
	libxsmu.setSourceMode (deviceID, SOURCE_MODE_VS, timeout)

timeout = 1.0
VM_RANGE_100mV = 2
VM_RANGE_1V    = 3
VM_RANGE_10V   = 4
voltageMeterRange, timeout = \
        libxsmu.VM_setRange (deviceID, VM_RANGE_10V, timeout)

timeout = 1.0
VM2_RANGE_10V = 0
voltageMeterRange2, timeout = \
        libxsmu.VM2_setRange (deviceID, VM2_RANGE_10V, timeout)

##########################################################################
# Set voltage source calibration table (for range selected above.)

index = 0
filter_length = 1

for v in [-10.0, -9.0, 0.0, +9.0, +10.0]:

	timeout = 1.0
	vset, timeout = \
		libxsmu.VS_setVoltage (deviceID, v, timeout)

	if (timeout == 0.0):
		print 'Communication timeout in VS_setVoltage'
		exit (-2)

	time.sleep (5)

	timeout = 10.0
	vsrc, timeout = \
		libxsmu.VM_getReading (deviceID, filter_length, timeout) 

	if (timeout == 0.0):
		print 'Communication timeout in VM_getReading'
		exit (-2)

	timeout = 10.0
	i, adc, voltage, timeout = \
		libxsmu.VM2_setCalibration (deviceID, index, vsrc, timeout)

	if (timeout == 0.0):
		print 'Communication timeout in VM2_setCalibration'
		exit (-2)

	index = index + 1
	print i, adc, voltage, timeout

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)

