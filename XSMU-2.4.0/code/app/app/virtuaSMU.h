#ifndef __SMU_VIRTUA_SMU__
#define __SMU_VIRTUA_SMU__

#include "Comm.h"
#include "CS.h"
#include "VS.h"
#include "CM.h"
#include "VM.h"
#include "VM2.h"
#include "RM.h"
#include "SystemConfig.h"
#include "version.h"

#include <bitset>
#include <stdint.h>
#include <future>
#include <mutex>
#include <queue>

namespace smu {

/************************************************************************/
/************************************************************************/

enum class SourceMode {

	CURRENT,
	VOLTAGE,
};

SourceMode toSourceMode (unsigned int i);

class AckBits:public std::bitset<64> {
};

class Driver {

 public:
	Driver (void);
	~Driver (void);

 public:
	static std::vector < FTDI_DeviceInfo > scan (void);
	static uint32_t library_version (void) {
		return LIBXSMU_VERSION;
	}

	uint32_t hardware_version (void) const {
		return versionInfo_->hardware_version();
	}

	uint32_t firmware_version (void) const {
		return versionInfo_->firmware_version();
	}

 public:
	void open (const char* serialNo, float* timeout);
	void close (void);

 public:
	/***************************************************/

	void identify (float* timeout);

	void keepAlive (uint32_t* lease_time_ms, float* timeout);
	void thread (void);
	void poll (uint16_t* size);

	void setSourceMode (SourceMode* mode, float* timeout);

	/***************************************************/

	void CS_setRange (CS_Range* range, float* timeout);

	void CS_getCalibration (uint16_t* index,
			       int16_t* dac, float* current, float* timeout);

	void CS_verifyCalibration (uint16_t* index,
				  int16_t* dac,
				  float* current, float* timeout);

	void CS_setCalibration (uint16_t* index,
			       int16_t* dac, float* current, float* timeout);

	void CS_saveCalibration (float* timeout);

	void CS_setCurrent (float* current, float* timeout);

	/***************************************************/

	void VS_setRange (VS_Range* range, float* timeout);

	void VS_getCalibration (uint16_t* index,
			       int16_t* dac, float* voltage, float* timeout);

	void VS_verifyCalibration (uint16_t* index,
				  int16_t* dac,
				  float* voltage, float* timeout);

	void VS_setCalibration (uint16_t* index,
			       int16_t* dac, float* voltage, float* timeout);

	void VS_saveCalibration (float* timeout);

	void VS_setVoltage (float* voltage, float* timeout);

	/***************************************************/

	void CM_setRange (CM_Range* range, float* timeout);

	void CM_getCalibration (uint16_t* index,
			       int32_t* adc, float* current, float* timeout);

	void CM_setCalibration (uint16_t* index,
			       int32_t* adc, float* current, float* timeout);

	void CM_saveCalibration (float* timeout);

	void CM_read (uint16_t* filterLength, float* current, float* timeout);

	/***************************************************/

	void VM_setRange (VM_Range* range, float* timeout);

	void VM_getCalibration (uint16_t* index,
			       int32_t* adc, float* voltage, float* timeout);

	void VM_setCalibration (uint16_t* index,
			       int32_t* adc, float* voltage, float* timeout);

	void VM_saveCalibration (float* timeout);

	void VM_read (uint16_t* filterLength, float* voltage, float* timeout);

	/***************************************************/

	void CS_loadDefaultCalibration (float* timeout);
	void VS_loadDefaultCalibration (float* timeout);
	void CM_loadDefaultCalibration (float* timeout);
	void VM_loadDefaultCalibration (float* timeout);

	/***************************************************/

	void RM_readAutoscale (uint16_t* filterLength,
						   float* resistance, float* timeout);

	/***************************************************/

	void SystemConfig_Save (float* timeout);
	void SystemConfig_LoadDefault (float* timeout);
	void SystemConfig_Set_hardwareVersion (uint32_t* version, float* timeout);
	void SystemConfig_Get_hardwareVersion (uint32_t* version, float* timeout);

	/***************************************************/

	void VM2_setRange (VM2_Range* range, float* timeout);

	void VM2_getCalibration (
		uint16_t* index, int32_t* adc, float* voltage, float* timeout);

	void VM2_setCalibration (
		uint16_t* index, int32_t* adc, float* voltage, float* timeout);

	void VM2_saveCalibration (float* timeout);
	void VM2_read (uint16_t* filterLength, float* voltage, float* timeout);
	void VM2_loadDefaultCalibration (float* timeout);

	/***************************************************/

	void VM_setTerminal (VM_Terminal* terminal, float* timeout);
	void VM_getTerminal (VM_Terminal* terminal, float* timeout);

	/***************************************************/

	void changeBaud (uint32_t* baudRate, float* timeout);

	void recSize  (uint16_t* recSize, float* timeout);
	void recData  (uint16_t* size, float* timeout);
	void StartRec (float* timeout);
	void StopRec  (float* timeout);

	/***************************************************/
 public:
	bool goodID (void) const;
	const char* identity (void) const {
		return identity_.c_str();
	}
 private:
	bool waitForResponse (uint16_t checkBit, float* timeout);
	bool waitForResponse (const AckBits & checkBits, float* timeout);

 private:
	static void comm_cb (void* user_data, const void* oCB);
	void comm_cb (const CommCB* oCB);

	void nopCB (const CommCB* oCB);
	void identityCB (const CommCB* oCB);
	void keepAliveCB (const CommCB* oCB);

	void setSourceModeCB (const CommCB* oCB);

	void CS_setRangeCB (const CommCB* oCB);
	void CS_getCalibrationCB (const CommCB* oCB);
	void CS_verifyCalibrationCB (const CommCB* oCB);
	void CS_setCalibrationCB (const CommCB* oCB);
	void CS_saveCalibrationCB (const CommCB* oCB);
	void CS_setCurrentCB (const CommCB* oCB);

	void VS_setRangeCB (const CommCB* oCB);
	void VS_getCalibrationCB (const CommCB* oCB);
	void VS_verifyCalibrationCB (const CommCB* oCB);
	void VS_setCalibrationCB (const CommCB* oCB);
	void VS_saveCalibrationCB (const CommCB* oCB);
	void VS_setVoltageCB (const CommCB* oCB);

	void CM_setRangeCB (const CommCB* oCB);
	void CM_getCalibrationCB (const CommCB* oCB);
	void CM_setCalibrationCB (const CommCB* oCB);
	void CM_saveCalibrationCB (const CommCB* oCB);
	void CM_readCB (const CommCB* oCB);

	void VM_setRangeCB (const CommCB* oCB);
	void VM_getCalibrationCB (const CommCB* oCB);
	void VM_setCalibrationCB (const CommCB* oCB);
	void VM_saveCalibrationCB (const CommCB* oCB);
	void VM_readCB (const CommCB* oCB);

	void CS_loadDefaultCalibrationCB (const CommCB* oCB);
	void VS_loadDefaultCalibrationCB (const CommCB* oCB);
	void CM_loadDefaultCalibrationCB (const CommCB* oCB);
	void VM_loadDefaultCalibrationCB (const CommCB* oCB);

	void RM_readAutoscaleCB (const CommCB* oCB);

	void SystemConfig_GetCB (const CommCB* oCB);
	void SystemConfig_SetCB (const CommCB* oCB);
	void SystemConfig_SaveCB (const CommCB* oCB);
	void SystemConfig_LoadDefaultCB (const CommCB* oCB);

	void VM2_setRangeCB (const CommCB* oCB);
	void VM2_getCalibrationCB (const CommCB* oCB);
	void VM2_setCalibrationCB (const CommCB* oCB);
	void VM2_saveCalibrationCB (const CommCB* oCB);
	void VM2_readCB (const CommCB* oCB);
	void VM2_loadDefaultCalibrationCB (const CommCB* oCB);

	void VM_setTerminalCB (const CommCB* oCB);
	void VM_getTerminalCB (const CommCB* oCB);

	void changeBaudCB (const CommCB* oCB);
	void recSizeCB (const CommCB* oCB);
	void recDataCB (const CommCB* oCB);
	void StartRecCB (const CommCB* oCB);
	void StopRecCB (const CommCB* oCB);

 private:
	Comm* comm_;
	CS* cs_;
	VS* vs_;
	CM* cm_;
	VM* vm_;
	VM2* vm2_;
	RM* rm_;
	SystemConfig* sysconf_;
	VersionInfo* versionInfo_;
	AckBits ackBits_;
	std::string identity_;

private:
	uint32_t baudRate_;
	bool _alive;
	std::future<void> _thread_future;

private:

	uint16_t recSize_;             //Stores size of available data with FW
	std::queue<int32_t> _dataq_32; //Stores ADC data obtained from FW

	//Stores ADC data converted to float using calibration table
	std::queue<float> _dataq;

	std::mutex _dataq_lock;

public:
	std::vector<float> getData (void);

private:
	bool _rec;

private:
	float applyCalibration (int32_t adc_value);
};

/************************************************************************/
/************************************************************************/

} // namespace smu

typedef smu::Driver VirtuaSMU;

#endif
