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
	void sync (float* timeout);

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
	void syncCB (const CommCB* oCB);

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
};

/************************************************************************/
/************************************************************************/

} // namespae smu

typedef smu::Driver VirtuaSMU;

#endif
