%module libxsmu
%include "typemaps.i"
%{
#include "libxsmu.h"
%}

%{

extern int scan(void);

extern const char *serialNo(int i);

extern int open_device(const char *serialNo, float timeout,
					   unsigned int *ret_goodID, float *ret_timeout);

extern void close_device(int deviceID);

/**************************************************************/

extern void setSourceMode(int deviceID, int mode,
						  float timeout, unsigned int *ret_mode,
						  float *ret_timeout);

/**************************************************************/

extern void CS_setRange(int deviceID, int range,
						float timeout, unsigned int *ret_range,
						float *ret_timeout);

extern void CS_getCalibration(int deviceID, unsigned int index,
							  float timeout, unsigned int *ret_index,
							  int *ret_dac, float *ret_current,
							  float *ret_timeout);

extern void CS_verifyCalibration(int deviceID, unsigned int index,
								 float timeout,
								 unsigned int *ret_index, int *ret_dac,
								 float *ret_current,
								 float *ret_timeout);

extern void CS_setCalibration(int deviceID, unsigned int index,
							  float current, float timeout,
							  unsigned int *ret_index, int *ret_dac,
							  float *ret_current, float *ret_timeout);

extern void CS_saveCalibration(int deviceID, float timeout,
							   float *ret_timeout);

extern void CS_setCurrent(int deviceID, float current, float timeout,
						  float *ret_current, float *ret_timeout);

extern void CS_loadDefaultCalibration (int deviceID,
									   float timeout, float *ret_timeout);

/**************************************************************/

extern void VS_setRange(int deviceID, int range,
						float timeout, unsigned int *ret_range,
						float *ret_timeout);

extern void VS_getCalibration(int deviceID, unsigned int index,
							  float timeout, unsigned int *ret_index,
							  int *ret_dac, float *ret_voltage,
							  float *ret_timeout);

extern void VS_verifyCalibration(int deviceID, unsigned int index,
								 float timeout,
								 unsigned int *ret_index, int *ret_dac,
								 float *ret_voltage,
								 float *ret_timeout);

extern void VS_setCalibration(int deviceID, unsigned int index,
							  float voltage, float timeout,
							  unsigned int *ret_index, int *ret_dac,
							  float *ret_voltage, float *ret_timeout);

extern void VS_saveCalibration(int deviceID, float timeout,
							   float *ret_timeout);

extern void VS_setVoltage(int deviceID, float voltage, float timeout,
						  float *ret_voltage, float *ret_timeout);

extern void VS_loadDefaultCalibration (int deviceID,
									   float timeout, float *ret_timeout);

/**************************************************************/

extern void CM_setRange(int deviceID, int range,
						float timeout, unsigned int *ret_range,
						float *ret_timeout);

extern void CM_getCalibration(int deviceID, unsigned int index,
							  float timeout, unsigned int *ret_index,
							  int *ret_adc, float *ret_current,
							  float *ret_timeout);

extern void CM_setCalibration(int deviceID, unsigned int index,
							  float current, float timeout,
							  unsigned int *ret_index, int *ret_adc,
							  float *ret_current, float *ret_timeout);

extern void CM_saveCalibration(int deviceID, float timeout,
							   float *ret_timeout);

extern void CM_getReading(int deviceID, unsigned int filterLength,
						  float timeout, float *ret_current,
						  float *ret_timeout);

extern void CM_loadDefaultCalibration (int deviceID,
									   float timeout, float *ret_timeout);

/**************************************************************/

extern void VM_setRange(int deviceID, int range,
						float timeout, unsigned int *ret_range,
						float *ret_timeout);

extern void VM_getCalibration(int deviceID, unsigned int index,
							  float timeout, unsigned int *ret_index,
							  int *ret_adc, float *ret_voltage,
							  float *ret_timeout);

extern void VM_setCalibration(int deviceID, unsigned int index,
							  float voltage, float timeout,
							  unsigned int *ret_index, int *ret_adc,
							  float *ret_voltage, float *ret_timeout);

extern void VM_saveCalibration(int deviceID, float timeout,
							   float *ret_timeout);

extern void VM_getReading(int deviceID, unsigned int filterLength,
						  float timeout, float *ret_current,
						  float *ret_timeout);

extern void VM_loadDefaultCalibration (int deviceID,
									   float timeout, float *ret_timeout);

/**************************************************************/

extern void RM_getReadingAutoscale (int deviceID, unsigned int filterLength,
									float timeout, float *ret_resistance,
									float *ret_timeout);

/**************************************************************/

extern void SystemConfig_Save (int deviceID,
						float timeout, float* ret_timeout);

extern void SystemConfig_LoadDefault (int deviceID,
						float timeout, float* ret_timeout);

extern void SystemConfig_Get_hardwareVersion (int deviceID,
		float timeout, unsigned int* ret_hardwareVersion, float* ret_timeout);

extern void SystemConfig_Set_hardwareVersion (int deviceID,
					unsigned int hardwareVersion, float timeout,
					unsigned int* ret_hardwareVersion, float* ret_timeout);

extern void library_version (int deviceID, unsigned int* version);
extern void hardware_version (int deviceID, unsigned int* version);
extern void firmware_version (int deviceID, unsigned int* version);

extern unsigned int make_version (unsigned int major,
				unsigned int minor, unsigned int bugfix);

extern unsigned int major_version_no (unsigned int version);
extern unsigned int minor_version_no (unsigned int version);
extern unsigned int bugfix_version_no (unsigned int version);

/**************************************************************/

extern void VM2_setRange(int deviceID, int range,
						float timeout, unsigned int *ret_range,
						float *ret_timeout);

extern void VM2_getCalibration(int deviceID, unsigned int index,
							  float timeout, unsigned int *ret_index,
							  int *ret_adc, float *ret_voltage,
							  float *ret_timeout);

extern void VM2_setCalibration(int deviceID, unsigned int index,
							  float voltage, float timeout,
							  unsigned int *ret_index, int *ret_adc,
							  float *ret_voltage, float *ret_timeout);

extern void VM2_saveCalibration(int deviceID, float timeout,
							   float *ret_timeout);

extern void VM2_getReading(int deviceID, unsigned int filterLength,
						  float timeout, float *ret_current,
						  float *ret_timeout);

extern void VM2_loadDefaultCalibration (int deviceID,
									   float timeout, float *ret_timeout);

/**************************************************************/

extern void VM_setTerminal(int deviceID, int terminal,
						float timeout, unsigned int *ret_terminal,
						float *ret_timeout);

extern void VM_getTerminal(int deviceID,
						float timeout, unsigned int *ret_terminal,
						float *ret_timeout);

/**************************************************************/

extern void changeBaud (int deviceID, unsigned int baudRate, float timeout,
						  unsigned int *ret_baudRate, float *ret_timeout);

/**************************************************************/

%}

/**************************************************************/
/**************************************************************/

extern int scan(void);

extern const char *serialNo(int i);

extern int open_device(const char *serialNo, float timeout,
					   unsigned int *OUTPUT, float *OUTPUT);

extern void close_device(int deviceID);

/**************************************************************/

extern void setSourceMode(int deviceID, int mode,
						  float timeout, unsigned int *OUTPUT, float *OUTPUT);

/**************************************************************/

extern void CS_setRange(int deviceID, int range, float timeout,
						unsigned int *OUTPUT, float *OUTPUT);

extern void CS_getCalibration(int deviceID, unsigned int index, float timeout,
							  unsigned int *OUTPUT, int *OUTPUT, float *OUTPUT,
							  float *OUTPUT);

extern void CS_verifyCalibration(int deviceID, unsigned int index,
								 float timeout, unsigned int *OUTPUT,
								 int *OUTPUT, float *OUTPUT, float *OUTPUT);

extern void CS_setCalibration(int deviceID, unsigned int index, float current,
							  float timeout, unsigned int *OUTPUT, int *OUTPUT,
							  float *OUTPUT, float *OUTPUT);

extern void CS_saveCalibration(int deviceID, float timeout, float *OUTPUT);

extern void CS_setCurrent(int deviceID, float current, float timeout,
						  float *OUTPUT, float *OUTPUT);

extern void CS_loadDefaultCalibration (int deviceID,
									   float timeout, float *OUTPUT);

/**************************************************************/

extern void VS_setRange(int deviceID, int range, float timeout,
						unsigned int *OUTPUT, float *OUTPUT);

extern void VS_getCalibration(int deviceID, unsigned int index, float timeout,
							  unsigned int *OUTPUT, int *OUTPUT, float *OUTPUT,
							  float *OUTPUT);

extern void VS_verifyCalibration(int deviceID, unsigned int index,
								 float timeout, unsigned int *OUTPUT,
								 int *OUTPUT, float *OUTPUT, float *OUTPUT);

extern void VS_setCalibration(int deviceID, unsigned int index, float voltage,
							  float timeout, unsigned int *OUTPUT, int *OUTPUT,
							  float *OUTPUT, float *OUTPUT);

extern void VS_saveCalibration(int deviceID, float timeout, float *OUTPUT);

extern void VS_setVoltage(int deviceID, float voltage, float timeout,
						  float *OUTPUT, float *OUTPUT);

extern void VS_loadDefaultCalibration (int deviceID,
									   float timeout, float *OUTPUT);

/**************************************************************/

extern void CM_setRange(int deviceID, int range, float timeout,
						unsigned int *OUTPUT, float *OUTPUT);

extern void CM_getCalibration(int deviceID, unsigned int index, float timeout,
							  unsigned int *OUTPUT, int *OUTPUT, float *OUTPUT,
							  float *OUTPUT);

extern void CM_setCalibration(int deviceID, unsigned int index, float current,
							  float timeout, unsigned int *OUTPUT, int *OUTPUT,
							  float *OUTPUT, float *OUTPUT);

extern void CM_saveCalibration(int deviceID, float timeout, float *OUTPUT);

extern void CM_getReading(int deviceID, unsigned int filterLength,
						  float timeout, float *OUTPUT, float *OUTPUT);

extern void CM_loadDefaultCalibration (int deviceID,
									   float timeout, float *OUTPUT);

/**************************************************************/

extern void VM_setRange(int deviceID, int range, float timeout,
						unsigned int *OUTPUT, float *OUTPUT);

extern void VM_getCalibration(int deviceID, unsigned int index, float timeout,
							  unsigned int *OUTPUT, int *OUTPUT, float *OUTPUT,
							  float *OUTPUT);

extern void VM_setCalibration(int deviceID, unsigned int index, float voltage,
							  float timeout, unsigned int *OUTPUT, int *OUTPUT,
							  float *OUTPUT, float *OUTPUT);

extern void VM_saveCalibration(int deviceID, float timeout, float *OUTPUT);

extern void VM_getReading(int deviceID, unsigned int filterLength,
						  float timeout, float *OUTPUT, float *OUTPUT);

extern void VM_loadDefaultCalibration (int deviceID,
									   float timeout, float *OUTPUT);

/**************************************************************/

extern void RM_getReadingAutoscale (int deviceID, unsigned int filterLength,
									float timeout, float *OUTPUT, float *OUTPUT);

/**************************************************************/

extern void SystemConfig_Save (int deviceID,
						float timeout, float* OUTPUT);

extern void SystemConfig_LoadDefault (int deviceID,
						float timeout, float* OUTPUT);

extern void SystemConfig_Get_hardwareVersion (int deviceID,
		float timeout, unsigned int* OUTPUT, float* OUTPUT);

extern void SystemConfig_Set_hardwareVersion (int deviceID,
					unsigned int hardwareVersion, float timeout,
					unsigned int* OUTPUT, float* OUTPUT);

/**************************************************************/

extern void library_version (int deviceID, unsigned int* OUTPUT);
extern void hardware_version (int deviceID, unsigned int* OUTPUT);
extern void firmware_version (int deviceID, unsigned int* OUTPUT);

extern unsigned int make_version (unsigned int major,
				unsigned int minor, unsigned int bugfix);

extern unsigned int major_version_no (unsigned int version);
extern unsigned int minor_version_no (unsigned int version);
extern unsigned int bugfix_version_no (unsigned int version);

/**************************************************************/

extern void VM2_setRange(int deviceID, int range, float timeout,
						unsigned int *OUTPUT, float *OUTPUT);

extern void VM2_getCalibration(int deviceID, unsigned int index, float timeout,
							  unsigned int *OUTPUT, int *OUTPUT, float *OUTPUT,
							  float *OUTPUT);

extern void VM2_setCalibration(int deviceID, unsigned int index, float voltage,
							  float timeout, unsigned int *OUTPUT, int *OUTPUT,
							  float *OUTPUT, float *OUTPUT);

extern void VM2_saveCalibration(int deviceID, float timeout, float *OUTPUT);

extern void VM2_getReading(int deviceID, unsigned int filterLength,
						  float timeout, float *OUTPUT, float *OUTPUT);

extern void VM2_loadDefaultCalibration (int deviceID,
									   float timeout, float *OUTPUT);

/**************************************************************/

extern void VM_setTerminal(int deviceID, int terminal, float timeout,
						unsigned int *OUTPUT, float *OUTPUT);

extern void VM_getTerminal(int deviceID, float timeout,
						unsigned int *OUTPUT, float *OUTPUT);

/**************************************************************/

extern void changeBaud (int deviceID, unsigned int baudRate, float timeout,
						  unsigned int *OUTPUT, float *OUTPUT);


/**************************************************************/
/**************************************************************/