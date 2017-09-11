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

set_voltage = 10
filter_length = 1024
acqTimeout = 10
waitTime = 10

##########################################################################

timeout = 1.0
sourceMode, timeout = libxsmu.setSourceMode (deviceID, 1, timeout)
print \
	"sourceMode    :", sourceMode, "\n" \
	"Remaining time:", timeout, "sec", "\n"

if (timeout == 0.0):
	print 'Communication timeout in setSourceMode.'
	exit (-2)

##########################################################################
# Selects source range

timeout = 1.0
voltageSourceRange = 0 # 0: 10V, 1: 100V
voltageSourceRange, timeout = \
	libxsmu.VS_setRange (deviceID, voltageSourceRange, timeout)
print \
	"voltageSourceRange    :", voltageSourceRange, "\n" \
	"Remaining time        :", timeout, "sec", "\n"

if (timeout == 0.0):
	print 'Communication timeout in VS_setRange.'
	exit (-2)

timeout = 1.0
currentMeterRange = 1 # 0: 10uA, 1: 100uA, 2: 1mA, 3: 10mA, 4: 100mA
currentMeterRange, timeout = \
	libxsmu.CM_setRange (deviceID, currentMeterRange, timeout)
print \
	"currentMeterRange    :", currentMeterRange, "\n" \
	"Remaining time        :", timeout, "sec", "\n"

if (timeout == 0.0):
	print 'Communication timeout in CS_setRange.'
	exit (-2)

timeout = 1.0
voltageMeterRange = 4 # 0: 1mV, 1: 10mV, 2: 100mV, 3: 1V, 4: 10V, 5: 100V
voltageMeterRange, timeout = \
	libxsmu.VM_setRange (deviceID, voltageMeterRange, timeout)
print \
	"voltageMeterRange    :", voltageMeterRange, "\n" \
	"Remaining time        :", timeout, "sec", "\n"

if (timeout == 0.0):
	print 'Communication timeout in VM_setRange.'
	exit (-2)

##########################################################################
##########################################################################
# Set current source calibration table (for range selected above.)

timeout = 1.0
voltage = set_voltage
voltage, timeout = libxsmu.VS_setVoltage (deviceID, voltage, timeout)
print \
	"Voltage: ", voltage, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in VS_setVoltage'
	exit (-2)

time.sleep (waitTime)

##########################################################################
# Set current source calibration table (for range selected above.)

timeout = acqTimeout
p_current, timeout = libxsmu.CM_getReading (deviceID, filter_length, timeout)
print \
	"Positive current: ", p_current, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in CM_getReading'
	exit (-2)

timeout = acqTimeout
p_voltage, timeout = libxsmu.VM_getReading (deviceID, filter_length, timeout)
print \
	"Positive voltage: ", p_voltage, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in VM_getReading'
	exit (-2)

##########################################################################
# Set current source calibration table (for range selected above.)

timeout = 1.0
voltage = -set_voltage
voltage, timeout = libxsmu.VS_setVoltage (deviceID, voltage, timeout)
print \
	"Voltage: ", voltage, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in VS_setVoltage'
	exit (-2)

time.sleep (waitTime)

##########################################################################
# Set current source calibration table (for range selected above.)

timeout = acqTimeout
n_current, timeout = libxsmu.CM_getReading (deviceID, filter_length, timeout)
print \
	"Negetive current: ", n_current, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in CM_getReading'
	exit (-2)

timeout = acqTimeout
n_voltage, timeout = libxsmu.VM_getReading (deviceID, filter_length, timeout)
print \
	"Negetive voltage: ", n_voltage, "\n" \
	"Timeout: ", timeout

if (timeout == 0.0):
	print 'Communication timeout in VM_getReading'
	exit (-2)

##########################################################################
# Set voltage source calibration table (for range selected above.)

print ((n_voltage - p_voltage) / (n_current - p_current)) / 1e6

##########################################################################
# closes the device.

libxsmu.close_device(deviceID)
