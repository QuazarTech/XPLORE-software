#ifndef __XSMU__
#define __XSMU__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Scans USB bus for XPLORE Source-Meter Unit.
 *
 * \return Number of XPLORE Source-Meter Unit found.
 *
 * This function must be called first. It creates an internal
 * list of XPLORE Source-Meter Unit hardware, whose details,
 * e.g. serial number etc., can then be accessed by other functions,
 * e.g. \ref serialNo.
 *
 */
int scan(void);

/**************************************************************/

/**
 * \brief Returns the serial number of a previously scanned device.
 *
 * \param i Device number.\n
 * It must be less than the total number of devices
 * as returned by \ref scan function.
 *
 * \return Pointer to an C-style internal string
 * containing the serial number of the i-th device.
 *
 * It is mandatory to call \ref scan before calling this function.
 * The returned pointer is gurranted to remain valid only
 * till the next call to \ref scan.
 *
 */
const char *serialNo(int i);

/**************************************************************/

/**
 * \brief Opens a previously scanned device for communication,
 * and queries its identity.
 *
 * \param timeout
 * 		Communication timeout in second.
 *
 * \param ret_goodID
 * 		Returns non-zero if the device responded with
 * 		"XPLORE LOCKIN 1.0".
 *
 * \param ret_timeout
 * 		Remaining time before a communication timeout occurs.
 * 		It should be checked for 0 to confirm a communication timeout.
 *
 * \return Device ID for further communication.
 * It must be passed to all other functions, e.g. \ref setRefGenParameters,
 * \ref setPllParameters etc.
 */
int open_device(const char *serialNo,
				float timeout,
				unsigned int *ret_goodID, float *ret_timeout);

/**************************************************************/

/**
 * \brief Closes a previously opened device.
 *
 * \param deviceID Device ID as returned by \ref open_device.
 */
void close_device(int deviceID);

/**************************************************************/

void setSourceMode(int deviceID,
				   int mode,
				   float timeout,
				   unsigned int *ret_mode, float *ret_timeout);

/**************************************************************/

void CS_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout);

void CS_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_dac,
					   float *ret_current, float *ret_timeout);

void CS_verifyCalibration(int deviceID, unsigned int index,
						  float timeout, unsigned int *ret_index,
						  int *ret_dac, float *ret_current,
						  float *ret_timeout);

void CS_setCalibration(int deviceID, unsigned int index, float current,
					   float timeout, unsigned int *ret_index,
					   int *ret_dac, float *ret_current,
					   float *ret_timeout);

void CS_saveCalibration(int deviceID, float timeout,
						float *ret_timeout);

void CS_setCurrent(int deviceID, float current, float timeout,
				   float *ret_current, float *ret_timeout);

void CS_loadDefaultCalibration (int deviceID,
								float timeout, float *ret_timeout);

/**************************************************************/

void VS_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout);

void VS_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_dac,
					   float *ret_voltage, float *ret_timeout);

void VS_verifyCalibration(int deviceID, unsigned int index,
						  float timeout, unsigned int *ret_index,
						  int *ret_dac, float *ret_voltage,
						  float *ret_timeout);

void VS_setCalibration(int deviceID, unsigned int index, float voltage,
					   float timeout, unsigned int *ret_index,
					   int *ret_dac, float *ret_voltage,
					   float *ret_timeout);

void VS_saveCalibration(int deviceID, float timeout,
						float *ret_timeout);

void VS_setVoltage(int deviceID, float voltage, float timeout,
				   float *ret_voltage, float *ret_timeout);

void VS_loadDefaultCalibration (int deviceID,
								float timeout, float *ret_timeout);

/**************************************************************/

void CM_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout);

void CM_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_adc,
					   float *ret_current, float *ret_timeout);

void CM_setCalibration(int deviceID, unsigned int index, float current,
					   float timeout, unsigned int *ret_index,
					   int *ret_adc, float *ret_current,
					   float *ret_timeout);

void CM_saveCalibration(int deviceID, float timeout,
						float *ret_timeout);

void CM_getReading(int deviceID, unsigned int filterLength,
				   float timeout, float *ret_current,
				   float *ret_timeout);

void CM_loadDefaultCalibration (int deviceID,
								float timeout, float *ret_timeout);

/**************************************************************/

void VM_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout);

void VM_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_adc,
					   float *ret_voltage, float *ret_timeout);

void VM_setCalibration(int deviceID, unsigned int index, float voltage,
					   float timeout, unsigned int *ret_index,
					   int *ret_adc, float *ret_voltage,
					   float *ret_timeout);

void VM_saveCalibration(int deviceID, float timeout,
						float *ret_timeout);

void VM_getReading(int deviceID, unsigned int filterLength,
				   float timeout, float *ret_current,
				   float *ret_timeout);

void VM_loadDefaultCalibration (int deviceID,
								float timeout, float *ret_timeout);

/**************************************************************/

void RM_getReadingAutoscale (int deviceID, unsigned int filterLength,
							 float timeout, float *ret_resistance,
							 float *ret_timeout);

/**************************************************************/

void SystemConfig_Save (int deviceID,
						float timeout,
						float* ret_timeout);

void SystemConfig_LoadDefault (int deviceID,
							   float timeout,
							   float* ret_timeout);

void SystemConfig_Get_hardwareVersion (int deviceID,
		float timeout, unsigned int* ret_hardwareVersion, float* ret_timeout);

void SystemConfig_Set_hardwareVersion (int deviceID,
					unsigned int hardwareVersion, float timeout,
					unsigned int* ret_hardwareVersion, float* ret_timeout);

/**************************************************************/

void library_version  (int deviceID, unsigned int* version);
void hardware_version (int deviceID, unsigned int* version);
void firmware_version (int deviceID, unsigned int* version);

unsigned int make_version (unsigned int major,
						   unsigned int minor, unsigned int bugfix);

unsigned int major_version_no (unsigned int version);
unsigned int minor_version_no (unsigned int version);
unsigned int bugfix_version_no (unsigned int version);

/**************************************************************/

void VM2_setRange(int deviceID, int range, float timeout,
				 unsigned int *ret_range, float *ret_timeout);

void VM2_getCalibration(int deviceID, unsigned int index, float timeout,
					   unsigned int *ret_index, int *ret_adc,
					   float *ret_voltage, float *ret_timeout);

void VM2_setCalibration(int deviceID, unsigned int index, float voltage,
					   float timeout, unsigned int *ret_index,
					   int *ret_adc, float *ret_voltage,
					   float *ret_timeout);

void VM2_saveCalibration(int deviceID, float timeout,
						float *ret_timeout);

void VM2_getReading(int deviceID, unsigned int filterLength,
				   float timeout, float *ret_current,
				   float *ret_timeout);

void VM2_loadDefaultCalibration (int deviceID,
								float timeout, float *ret_timeout);

void VM_setTerminal(int deviceID, int terminal, float timeout,
				 unsigned int *ret_terminal, float *ret_timeout);

void VM_getTerminal(int deviceID, float timeout,
				 unsigned int *ret_terminal, float *ret_timeout);

/**************************************************************/

void changeBaud (int deviceID, unsigned int baudRate, float timeout,
				unsigned int *ret_baudRate, float *ret_timeout);

#ifdef __cplusplus
}				// extern "C"

#endif
#endif
