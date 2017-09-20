#include "libxsmu.h"
#include "../../code/app/app/virtuaSMU.h"

#include <iostream>
#include <cstring>

using namespace std;

/**************************************************************/
/**************************************************************/

static vector <VirtuaSMU *> virtuaSMUs;
static vector <smu::FTDI_DeviceInfo> devices;

int scan(void)
{
	devices = VirtuaSMU::scan();
	return devices.size();
}

const char *serialNo(int i)
{
	return devices[i].serialNo();
}

int open_device(const char *serialNo,
				float timeout, unsigned int *ret_goodID, float *ret_timeout)
{
	/**** Creates and registers an Comm object ****/
	VirtuaSMU *virtuaSMU = new VirtuaSMU;

	vector < VirtuaSMU * >::iterator it;
	for (it = virtuaSMUs.begin(); it != virtuaSMUs.end(); ++it)
		if (*it == 0)
			break;

	if (it == virtuaSMUs.end())
		it = virtuaSMUs.insert(it, virtuaSMU);
	else
		*it = virtuaSMU;

	/**************************************/

	float timeout_ = timeout;
	virtuaSMU->open (serialNo, &timeout_);

	*ret_goodID = virtuaSMU->goodID();
	*ret_timeout = timeout_;

	return it - virtuaSMUs.begin();
}

/************************************************************************/

void close_device(int deviceID)
{
	delete virtuaSMUs[deviceID];
	virtuaSMUs[deviceID] = 0;
}

/************************************************************************/
/************************************************************************/

void setSourceMode(int deviceID, int mode, float timeout,
				   unsigned int *ret_mode, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto mode_ = smu::toSourceMode (mode);
	float timeout_ = timeout;

	virtuaSMU->setSourceMode(&mode_, &timeout_);

	/**************************************/

	*ret_mode = 0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_mode = (int) mode_;
}

/************************************************************************/
/************************************************************************/

void CS_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto range_ = smu::toCS_Range(range);
	float timeout_ = timeout;

	virtuaSMU->CS_setRange(&range_, &timeout_);

	*ret_range = 0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_range = range_;
}

/************************************************************************/

void CS_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_dac,
					   float *ret_current, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int16_t dac_;
	float current_;

	virtuaSMU->CS_getCalibration(&index_, &dac_, &current_, &timeout_);

	*ret_index = 0;
	*ret_dac = 0;
	*ret_current = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_dac = dac_;
	*ret_current = current_;
}

/************************************************************************/

void CS_verifyCalibration(int deviceID, unsigned int index, float timeout,
						  unsigned int *ret_index, int *ret_dac,
						  float *ret_current, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int16_t dac_;
	float current_;

	virtuaSMU->CS_verifyCalibration(&index_, &dac_, &current_, &timeout_);

	*ret_index = 0;
	*ret_dac = 0;
	*ret_current = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_dac = dac_;
	*ret_current = current_;
}

/************************************************************************/

void CS_setCalibration(int deviceID, unsigned int index, float current,
					   float timeout, unsigned int *ret_index, int *ret_dac,
					   float *ret_current, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int16_t dac_;
	float current_ = current;

	virtuaSMU->CS_setCalibration(&index_, &dac_, &current_, &timeout_);

	*ret_index = 0;
	*ret_dac = 0;
	*ret_current = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_dac = dac_;
	*ret_current = current_;
}

/************************************************************************/

void CS_saveCalibration(int deviceID, float timeout, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->CS_saveCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/

void CS_setCurrent(int deviceID, float current, float timeout,
				   float *ret_current, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	float current_ = current;
	float timeout_ = timeout;

	virtuaSMU->CS_setCurrent(&current_, &timeout_);

	*ret_current = 0.0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_current = current_;
}

void CS_loadDefaultCalibration(int deviceID, float timeout,
							   float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->CS_loadDefaultCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/
/************************************************************************/

void VS_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto range_ = smu::toVS_Range(range);
	float timeout_ = timeout;

	virtuaSMU->VS_setRange(&range_, &timeout_);

	*ret_range = 0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_range = range_;
}

/************************************************************************/

void VS_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_dac,
					   float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int16_t dac_;
	float voltage_;

	virtuaSMU->VS_getCalibration(&index_, &dac_, &voltage_, &timeout_);

	*ret_index = 0;
	*ret_dac = 0;
	*ret_voltage = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_dac = dac_;
	*ret_voltage = voltage_;
}

/************************************************************************/

void VS_verifyCalibration(int deviceID, unsigned int index, float timeout,
						  unsigned int *ret_index, int *ret_dac,
						  float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int16_t dac_;
	float voltage_;

	virtuaSMU->VS_verifyCalibration(&index_, &dac_, &voltage_, &timeout_);

	*ret_index = 0;
	*ret_dac = 0;
	*ret_voltage = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_dac = dac_;
	*ret_voltage = voltage_;
}

/************************************************************************/

void VS_setCalibration(int deviceID, unsigned int index, float voltage,
					   float timeout, unsigned int *ret_index, int *ret_dac,
					   float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int16_t dac_;
	float voltage_ = voltage;

	virtuaSMU->VS_setCalibration(&index_, &dac_, &voltage_, &timeout_);

	*ret_index = 0;
	*ret_dac = 0;
	*ret_voltage = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_dac = dac_;
	*ret_voltage = voltage_;
}

/************************************************************************/

void VS_saveCalibration(int deviceID, float timeout, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->VS_saveCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/

void VS_setVoltage(int deviceID, float voltage, float timeout,
				   float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	float voltage_ = voltage;
	float timeout_ = timeout;

	virtuaSMU->VS_setVoltage(&voltage_, &timeout_);

	*ret_voltage = 0.0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_voltage = voltage_;
}

void VS_loadDefaultCalibration(int deviceID, float timeout,
							   float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->VS_loadDefaultCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/
/************************************************************************/

void CM_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto range_ = smu::toCM_Range(range);
	float timeout_ = timeout;

	virtuaSMU->CM_setRange(&range_, &timeout_);

	*ret_range = 0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_range = range_;
}

/************************************************************************/

void CM_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_adc,
					   float *ret_current, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int32_t adc_;
	float current_;

	virtuaSMU->CM_getCalibration(&index_, &adc_, &current_, &timeout_);

	*ret_index = 0;
	*ret_adc = 0;
	*ret_current = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_adc = adc_;
	*ret_current = current_;
}

/************************************************************************/

void CM_setCalibration(int deviceID, unsigned int index, float current,
					   float timeout, unsigned int *ret_index, int *ret_adc,
					   float *ret_current, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int32_t adc_;
	float current_ = current;

	virtuaSMU->CM_setCalibration(&index_, &adc_, &current_, &timeout_);

	*ret_index = 0;
	*ret_adc = 0;
	*ret_current = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_adc = adc_;
	*ret_current = current_;
}

/************************************************************************/

void CM_saveCalibration(int deviceID, float timeout, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->CM_saveCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/

void CM_getReading(int deviceID, unsigned int filterLength, float timeout,
				   float *ret_current, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t filterLength_ = filterLength;
	float timeout_ = timeout;
	float current_;

	virtuaSMU->CM_read(&filterLength_, &current_, &timeout_);

	*ret_current = 0.0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_current = current_;
}

void CM_loadDefaultCalibration(int deviceID, float timeout,
							   float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->CM_loadDefaultCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/
/************************************************************************/

void VM_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto range_ = smu::toVM_Range(range);
	float timeout_ = timeout;

	virtuaSMU->VM_setRange(&range_, &timeout_);

	*ret_range = 0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_range = range_;
}

/************************************************************************/

void VM_getCalibration(int deviceID, unsigned int index,
					   float timeout,unsigned int *ret_index,
					   int *ret_adc,float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int32_t adc_;
	float voltage_;

	virtuaSMU->VM_getCalibration(&index_, &adc_, &voltage_, &timeout_);

	*ret_index = 0;
	*ret_adc = 0;
	*ret_voltage = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_adc = adc_;
	*ret_voltage = voltage_;
}

/************************************************************************/

void VM_setCalibration(int deviceID, unsigned int index, float voltage,
					   float timeout, unsigned int *ret_index, int *ret_adc,
					   float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int32_t adc_;
	float voltage_ = voltage;

	virtuaSMU->VM_setCalibration(&index_, &adc_, &voltage_, &timeout_);

	*ret_index = 0;
	*ret_adc = 0;
	*ret_voltage = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_adc = adc_;
	*ret_voltage = voltage_;
}

/************************************************************************/

void VM_saveCalibration(int deviceID, float timeout, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->VM_saveCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/

void VM_getReading(int deviceID, unsigned int filterLength, float timeout,
				   float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t filterLength_ = filterLength;
	float timeout_ = timeout;
	float voltage_;

	virtuaSMU->VM_read(&filterLength_, &voltage_, &timeout_);

	*ret_voltage = 0.0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_voltage = voltage_;
}

/************************************************************************/

void VM_loadDefaultCalibration(int deviceID, float timeout,
							   float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->VM_loadDefaultCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/
/************************************************************************/

void RM_getReadingAutoscale (int deviceID, unsigned int filterLength,
							 float timeout, float *ret_resistance,
							 float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t filterLength_ = filterLength;
	float timeout_ = timeout;
	float resistance_;

	virtuaSMU->RM_readAutoscale (&filterLength_, &resistance_, &timeout_);

	*ret_resistance = 0.0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_resistance = resistance_;
}

/************************************************************************/
/************************************************************************/

void SystemConfig_Save (int deviceID, float timeout, float* ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->SystemConfig_Save (&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/

void SystemConfig_LoadDefault (int deviceID, float timeout, float* ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->SystemConfig_LoadDefault (&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/

void SystemConfig_Get_hardwareVersion (int deviceID,
		float timeout, unsigned int* ret_hardwareVersion, float* ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint32_t hardwareVersion_;
	float timeout_ = timeout;
	virtuaSMU->SystemConfig_Get_hardwareVersion (&hardwareVersion_,
												 &timeout_);

	*ret_hardwareVersion = 0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_hardwareVersion = hardwareVersion_;
}

/************************************************************************/

void SystemConfig_Set_hardwareVersion (int deviceID,
					unsigned int hardwareVersion, float timeout,
					unsigned int* ret_hardwareVersion, float* ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint32_t hardwareVersion_ = hardwareVersion;
	float timeout_ = timeout;
	virtuaSMU->SystemConfig_Set_hardwareVersion (&hardwareVersion_,
												 &timeout_);

	*ret_hardwareVersion = 0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_hardwareVersion = hardwareVersion_;
}

/************************************************************************/
/************************************************************************/

void library_version (int deviceID, unsigned int* version)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	*version = virtuaSMU->library_version();
}

void hardware_version (int deviceID, unsigned int* version)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	*version = virtuaSMU->hardware_version();
}

void firmware_version (int deviceID, unsigned int* version)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	*version = virtuaSMU->firmware_version();
}

unsigned int make_version (unsigned int major,
						   unsigned int minor,
						   unsigned int bugfix)
{
	return smu::MAKE_VERSION_NO (major, minor, bugfix);
}

unsigned int major_version_no (unsigned int version)
{
	return smu::MAJOR_VERSION_NO (version);
}

unsigned int minor_version_no (unsigned int version)
{
	return smu::MINOR_VERSION_NO (version);
}

unsigned int bugfix_version_no (unsigned int version)
{
	return smu::BUGFIX_VERSION_NO (version);
}

/************************************************************************/
/************************************************************************/

void VM2_setRange(int deviceID, int range, float timeout,
				  unsigned int *ret_range, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto range_ = smu::toVM2_Range(range);
	float timeout_ = timeout;

	virtuaSMU->VM2_setRange(&range_, &timeout_);

	*ret_range = 0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_range = range_;
}

/************************************************************************/

void VM2_getCalibration(int deviceID, unsigned int index,
						float timeout, unsigned int *ret_index,
						int *ret_adc, float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int32_t adc_;
	float voltage_;

	virtuaSMU->VM2_getCalibration(&index_, &adc_, &voltage_, &timeout_);

	*ret_index = 0;
	*ret_adc = 0;
	*ret_voltage = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_adc = adc_;
	*ret_voltage = voltage_;
}

/************************************************************************/

void VM2_setCalibration(int deviceID, unsigned int index, float voltage,
						float timeout, unsigned int *ret_index, int *ret_adc,
						float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t index_ = index;
	float timeout_ = timeout;
	int32_t adc_;
	float voltage_ = voltage;

	virtuaSMU->VM2_setCalibration(&index_, &adc_, &voltage_, &timeout_);

	*ret_index = 0;
	*ret_adc = 0;
	*ret_voltage = 0.0;
	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_index = index_;
	*ret_adc = adc_;
	*ret_voltage = voltage_;
}

/************************************************************************/

void VM2_saveCalibration(int deviceID, float timeout, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->VM2_saveCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/

void VM2_getReading(int deviceID, unsigned int filterLength, float timeout,
					float *ret_voltage, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	uint16_t filterLength_ = filterLength;
	float timeout_ = timeout;
	float voltage_;

	virtuaSMU->VM2_read(&filterLength_, &voltage_, &timeout_);

	*ret_voltage = 0.0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_voltage = voltage_;
}

/************************************************************************/

void VM2_loadDefaultCalibration(int deviceID, float timeout,
								float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->VM2_loadDefaultCalibration(&timeout);
	*ret_timeout = timeout;
}

/************************************************************************/
/************************************************************************/

void VM_setTerminal(int deviceID, int terminal, float timeout,
				 unsigned int *ret_terminal, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto terminal_ = smu::toVM_Terminal(terminal);
	float timeout_ = timeout;

	virtuaSMU->VM_setTerminal(&terminal_, &timeout_);

	*ret_terminal = 0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_terminal = terminal_;
}

/************************************************************************/

void VM_getTerminal(int deviceID, float timeout,
				 unsigned int *ret_terminal, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	auto terminal_ = smu::toVM_Terminal(0);
	float timeout_ = timeout;

	virtuaSMU->VM_getTerminal(&terminal_, &timeout_);

	*ret_terminal = 0;

	if ((*ret_timeout = timeout_) == 0)
		return;

	*ret_terminal = terminal_;
}

/************************************************************************/
/************************************************************************/

void changeBaud (int deviceID, unsigned int baudRate, float timeout,
				unsigned int *ret_baudRate, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	unsigned int baudRate_ = baudRate;
	float timeout_ = timeout;

	virtuaSMU->changeBaud (&baudRate_, &timeout_);

	*ret_baudRate = 9600;

	if ((*ret_timeout = timeout_) == 0)
        return;

	*ret_baudRate = baudRate_;
}

/************************************************************************/

// void getData (int deviceID, std::vector<float> *ret_data)
// {
// 	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
//
// 	std::vector<float> data_;
//
// 	data_ = virtuaSMU->getData();
//
// 	*ret_data = data_;
// }

void startRec (int deviceID)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->startRec();
}

void stopRec (int deviceID)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];
	virtuaSMU->stopRec();
}

/************************************************************************/
//Only for testing

void keepAlive (int deviceID, unsigned int lease_time_ms, float timeout,
				unsigned int *ret_lease_time_ms, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	unsigned int lease_time_ms_ = lease_time_ms;
	float timeout_ = timeout;

	virtuaSMU->keepAlive (&lease_time_ms_, &timeout_);

	*ret_lease_time_ms = 0;

	if ((*ret_timeout = timeout_) == 0)
        return;

	*ret_lease_time_ms = lease_time_ms_;
}

void recSize (int deviceID, float timeout,
			  short unsigned int *ret_recSize, float *ret_timeout)
{
	VirtuaSMU *virtuaSMU = virtuaSMUs[deviceID];

	short unsigned int recSize_;
	float timeout_ = timeout;

	virtuaSMU->recSize (&recSize_, &timeout_);

	*ret_recSize = 0;

	if ((*ret_timeout = timeout_) == 0)
        return;

	*ret_recSize = recSize_;
}

/************************************************************************/
/************************************************************************/
