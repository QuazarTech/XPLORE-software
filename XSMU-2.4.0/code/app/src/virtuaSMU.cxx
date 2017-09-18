#include "../app/virtuaSMU.h"
#include "../../sys/sys/Timer.h"
#include "../../app/app/Exception.h"

#include <cstdio>
#include <string>
#include <iostream>

#define PRINT_DEBUG(x) { \
std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << x << std::endl; }

namespace smu {

SourceMode toSourceMode (unsigned int i)
{
	static const SourceMode values[] = {
		SourceMode::CURRENT,
		SourceMode::VOLTAGE
	};

	return (i <
	sizeof (values) / sizeof (values[0])) ? values[i] : values[0];
}

/************************************************************************/
/************************************************************************/

std::vector <FTDI_DeviceInfo> Driver::scan (void)
{
	std::cout << "libxsmu version: "
		      << MAJOR_VERSION_NO (LIBXSMU_VERSION) << "."
		      << MINOR_VERSION_NO (LIBXSMU_VERSION) << "."
		      << BUGFIX_VERSION_NO (LIBXSMU_VERSION)
			  << std::endl;

	std::vector <FTDI_DeviceInfo> device_list;
	std::vector <FTDI_DeviceInfo>::const_iterator it;
	std::vector <FTDI_DeviceInfo> allDevices = Comm::scan();

	for (it = allDevices.begin(); it != allDevices.end(); ++it)
		if (std::string (it->description()) == "XPLORE SMU")
			device_list.push_back (*it);

		return device_list;
}

Driver::Driver (void)
{
	cs_  = new CS;
	vs_  = new VS;
	cm_  = new CM;
	vm_  = new VM;
	vm2_ = new VM2;
	rm_  = new RM;
	sysconf_ = new SystemConfig;
	versionInfo_ = new VersionInfo;

	comm_ = new Comm;
	comm_->callback (comm_cb, this);
}

Driver::~Driver (void)
{
	close();

	delete comm_;
	delete vm_;
	delete cm_;
	delete vs_;
	delete cs_;
	delete rm_;
	delete sysconf_;
	delete versionInfo_;
}

void Driver::comm_cb (void* user_data, const void* oCB)
{
	reinterpret_cast <
	Driver*> (user_data)->comm_cb (reinterpret_cast <
	const CommCB*> (oCB));
}

void Driver::comm_cb (const CommCB* oCB)
{
	typedef void (Driver::*cb_t) (const CommCB* );

	static const cb_t cbs[] = {
		&Driver::nopCB,
		&Driver::identityCB,
		&Driver::keepAliveCB,
		&Driver::setSourceModeCB,

		&Driver::CS_setRangeCB,
		&Driver::CS_getCalibrationCB,
		&Driver::CS_verifyCalibrationCB,
		&Driver::CS_setCalibrationCB,
		&Driver::CS_saveCalibrationCB,
		&Driver::CS_setCurrentCB,

		&Driver::VS_setRangeCB,
		&Driver::VS_getCalibrationCB,
		&Driver::VS_verifyCalibrationCB,
		&Driver::VS_setCalibrationCB,
		&Driver::VS_saveCalibrationCB,
		&Driver::VS_setVoltageCB,

		&Driver::CM_setRangeCB,
		&Driver::CM_getCalibrationCB,
		&Driver::CM_setCalibrationCB,
		&Driver::CM_saveCalibrationCB,
		&Driver::CM_readCB,

		&Driver::VM_setRangeCB,
		&Driver::VM_getCalibrationCB,
		&Driver::VM_setCalibrationCB,
		&Driver::VM_saveCalibrationCB,
		&Driver::VM_readCB,

		&Driver::CS_loadDefaultCalibrationCB,
		&Driver::VS_loadDefaultCalibrationCB,
		&Driver::CM_loadDefaultCalibrationCB,
		&Driver::VM_loadDefaultCalibrationCB,

		&Driver::RM_readAutoscaleCB,

		&Driver::SystemConfig_GetCB,
		&Driver::SystemConfig_SetCB,
		&Driver::SystemConfig_SaveCB,
		&Driver::SystemConfig_LoadDefaultCB,

		&Driver::VM2_setRangeCB,
		&Driver::VM2_getCalibrationCB,
		&Driver::VM2_setCalibrationCB,
		&Driver::VM2_saveCalibrationCB,
		&Driver::VM2_readCB,
		&Driver::VM2_loadDefaultCalibrationCB,

		&Driver::VM_setTerminalCB,
		&Driver::VM_getTerminalCB,

		&Driver::changeBaudCB,
	};

	if (oCB->code() < sizeof (cbs) / sizeof (cbs[0]))
		 (this->*cbs[oCB->code()]) (oCB);
}

/************************************************************************/
/************************************************************************/

void Driver::nopCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_NOP);
}

/************************************************************************/

void Driver::identityCB (const CommCB* oCB)
{
	const CommCB_Identity* o =
	reinterpret_cast<const CommCB_Identity*> (oCB);

	identity_ = o->identity();
	versionInfo_->hardware_version (o->hardware_version());
	versionInfo_->firmware_version (o->firmware_version());

	ackBits_.set (COMM_CBCODE_IDN);
}

/************************************************************************/

void Driver::keepAliveCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_KEEP_ALIVE);
}

/************************************************************************/

void Driver::setSourceModeCB (const CommCB* oCB)
{
	const CommCB_SetSourceMode* o =
	reinterpret_cast<const CommCB_SetSourceMode*> (oCB);

	switch (o->mode()) {
		case COMM_SOURCE_MODE_CURRENT:
			vs_->deactivate();
			cs_->activate();
			break;

		case COMM_SOURCE_MODE_VOLTAGE:
			cs_->deactivate();
			vs_->activate();
			break;
	}

	ackBits_.set (COMM_CBCODE_SET_SOURCE_MODE);
}

/************************************************************************/
/************************************************************************/

void Driver::CS_setRangeCB (const CommCB* oCB)
{
	const CommCB_CS_SetRange* o =
	reinterpret_cast<const CommCB_CS_SetRange*> (oCB);

	cs_->setRange (toCS_Range (o->range()));
	ackBits_.set (COMM_CBCODE_CS_SET_RANGE);
}

/************************************************************************/

void Driver::CS_getCalibrationCB (const CommCB* oCB)
{
	const CommCB_CS_GetCalibration* o =
	reinterpret_cast<const CommCB_CS_GetCalibration*> (oCB);

	cs_->setCalibration (o->index(), o->dac(), o->current());
	ackBits_.set (COMM_CBCODE_CS_GET_CALIBRATION);
}

/************************************************************************/

void Driver::CS_verifyCalibrationCB (const CommCB* oCB)
{
	const CommCB_CS_VerifyCalibration* o =
	reinterpret_cast<const CommCB_CS_VerifyCalibration*> (oCB);

	cs_->setCalibration (o->index(), o->dac(), o->current());
	ackBits_.set (COMM_CBCODE_CS_VERIFY_CALIBRATION);
}

/************************************************************************/

void Driver::CS_setCalibrationCB (const CommCB* oCB)
{
	const CommCB_CS_SetCalibration* o =
	reinterpret_cast<const CommCB_CS_SetCalibration*> (oCB);

	cs_->setCalibration (o->index(), o->dac(), o->current());
	ackBits_.set (COMM_CBCODE_CS_SET_CALIBRATION);
}

/************************************************************************/

void Driver::CS_saveCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_CS_SAVE_CALIBRATION);
}

/************************************************************************/

void Driver::CS_setCurrentCB (const CommCB* oCB)
{
	const CommCB_CS_SetCurrent* o =
	reinterpret_cast<const CommCB_CS_SetCurrent*> (oCB);

	cs_->setCurrent (o->current());
	ackBits_.set (COMM_CBCODE_CS_SET_CURRENT);
}

/************************************************************************/
/************************************************************************/

void Driver::VS_setRangeCB (const CommCB* oCB)
{
	const CommCB_VS_SetRange* o =
	reinterpret_cast<const CommCB_VS_SetRange*> (oCB);

	vs_->setRange (toVS_Range (o->range()));
	ackBits_.set (COMM_CBCODE_VS_SET_RANGE);
}

/************************************************************************/

void Driver::VS_getCalibrationCB (const CommCB* oCB)
{
	const CommCB_VS_GetCalibration* o =
	reinterpret_cast<const CommCB_VS_GetCalibration*> (oCB);

	vs_->setCalibration (o->index(), o->dac(), o->voltage());
	ackBits_.set (COMM_CBCODE_VS_GET_CALIBRATION);
}

/************************************************************************/

void Driver::VS_verifyCalibrationCB (const CommCB* oCB)
{
	const CommCB_VS_VerifyCalibration* o =
	reinterpret_cast<const CommCB_VS_VerifyCalibration*> (oCB);

	vs_->setCalibration (o->index(), o->dac(), o->voltage());
	ackBits_.set (COMM_CBCODE_VS_VERIFY_CALIBRATION);
}

/************************************************************************/

void Driver::VS_setCalibrationCB (const CommCB* oCB)
{
	const CommCB_VS_SetCalibration* o =
	reinterpret_cast<const CommCB_VS_SetCalibration*> (oCB);

	vs_->setCalibration (o->index(), o->dac(), o->voltage());
	ackBits_.set (COMM_CBCODE_VS_SET_CALIBRATION);
}

/************************************************************************/

void Driver::VS_saveCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_VS_SAVE_CALIBRATION);
}

/************************************************************************/

void Driver::VS_setVoltageCB (const CommCB* oCB)
{
	const CommCB_VS_SetVoltage* o =
	reinterpret_cast<const CommCB_VS_SetVoltage*> (oCB);

	vs_->setVoltage (o->voltage());
	ackBits_.set (COMM_CBCODE_VS_SET_VOLTAGE);

}

/************************************************************************/
/************************************************************************/

void Driver::CM_setRangeCB (const CommCB* oCB)
{
	const CommCB_CM_SetRange* o =
	reinterpret_cast<const CommCB_CM_SetRange*> (oCB);

	cm_->setRange (toCM_Range (o->range()));
	ackBits_.set (COMM_CBCODE_CM_SET_RANGE);
}

/************************************************************************/

void Driver::CM_getCalibrationCB (const CommCB* oCB)
{
	const CommCB_CM_GetCalibration* o =
	reinterpret_cast<const CommCB_CM_GetCalibration*> (oCB);

	cm_->setCalibration (o->index(), o->adc(), o->current());
	ackBits_.set (COMM_CBCODE_CM_GET_CALIBRATION);
}

/************************************************************************/

void Driver::CM_setCalibrationCB (const CommCB* oCB)
{
	const CommCB_CM_SetCalibration* o =
	reinterpret_cast<const CommCB_CM_SetCalibration*> (oCB);

	cm_->setCalibration (o->index(), o->adc(), o->current());
	ackBits_.set (COMM_CBCODE_CM_SET_CALIBRATION);
}

/************************************************************************/

void Driver::CM_saveCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_CM_SAVE_CALIBRATION);
}

/************************************************************************/

void Driver::CM_readCB (const CommCB* oCB)
{
	const CommCB_CM_Read* o =
	reinterpret_cast<const CommCB_CM_Read*> (oCB);

	cm_->setCurrent (o->current());
	ackBits_.set (COMM_CBCODE_CM_READ);
}

/************************************************************************/
/************************************************************************/

void Driver::VM_setRangeCB (const CommCB* oCB)
{
	const CommCB_VM_SetRange* o =
	reinterpret_cast<const CommCB_VM_SetRange*> (oCB);

	vm_->setRange (toVM_Range (o->range()));
	ackBits_.set (COMM_CBCODE_VM_SET_RANGE);
}

/************************************************************************/

void Driver::VM_getCalibrationCB (const CommCB* oCB)
{
	const CommCB_VM_GetCalibration* o =
	reinterpret_cast<const CommCB_VM_GetCalibration*> (oCB);

	vm_->setCalibration (o->index(), o->adc(), o->voltage());
	ackBits_.set (COMM_CBCODE_VM_GET_CALIBRATION);
}

/************************************************************************/

void Driver::VM_setCalibrationCB (const CommCB* oCB)
{
	const CommCB_VM_SetCalibration* o =
	reinterpret_cast<const CommCB_VM_SetCalibration*> (oCB);

	vm_->setCalibration (o->index(), o->adc(), o->voltage());
	ackBits_.set (COMM_CBCODE_VM_SET_CALIBRATION);
}

/************************************************************************/

void Driver::VM_saveCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_VM_SAVE_CALIBRATION);
}

/************************************************************************/

void Driver::VM_readCB (const CommCB* oCB)
{
	const CommCB_VM_Read* o =
	reinterpret_cast<const CommCB_VM_Read*> (oCB);

	vm_->setVoltage (o->voltage());
	ackBits_.set (COMM_CBCODE_VM_READ);
}

/************************************************************************/
/************************************************************************/

void Driver::CS_loadDefaultCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_CS_LOAD_DEFAULT_CALIBRATION);
}

/************************************************************************/

void Driver::VS_loadDefaultCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_VS_LOAD_DEFAULT_CALIBRATION);
}

/************************************************************************/

void Driver::CM_loadDefaultCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_CM_LOAD_DEFAULT_CALIBRATION);
}

/************************************************************************/

void Driver::VM_loadDefaultCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_VM_LOAD_DEFAULT_CALIBRATION);
}

/************************************************************************/
/************************************************************************/

void Driver::RM_readAutoscaleCB (const CommCB* oCB)
{
	const CommCB_RM_ReadAutoscale* o =
	reinterpret_cast<const CommCB_RM_ReadAutoscale*> (oCB);

	rm_->setResistance (o->resistance());
	ackBits_.set (COMM_CBCODE_RM_READ_AUTOSCALE);
}

/************************************************************************/
/************************************************************************/

void Driver::SystemConfig_GetCB (const CommCB* oCB)
{
	const CommCB_SystemConfig_Get* o =
		reinterpret_cast <const CommCB_SystemConfig_Get*> (oCB);

	sysconf_->set (o->paramID(), o->value());
	ackBits_.set (COMM_CBCODE_SYSTEM_CONFIG_GET);
}

void Driver::SystemConfig_SetCB (const CommCB* oCB)
{
	const CommCB_SystemConfig_Set* o =
		reinterpret_cast <const CommCB_SystemConfig_Set*> (oCB);

	sysconf_->set (o->paramID(), o->value());
	ackBits_.set (COMM_CBCODE_SYSTEM_CONFIG_SET);
}

void Driver::SystemConfig_SaveCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_SYSTEM_CONFIG_SAVE);
}

void Driver::SystemConfig_LoadDefaultCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_SYSTEM_CONFIG_LOAD_DEFAULT);
}

/************************************************************************/
/************************************************************************/

void Driver::VM2_setRangeCB (const CommCB* oCB)
{
	const CommCB_VM2_SetRange* o =
	reinterpret_cast<const CommCB_VM2_SetRange*> (oCB);

	vm2_->setRange (toVM2_Range (o->range()));
	ackBits_.set (COMM_CBCODE_VM2_SET_RANGE);
}

/************************************************************************/

void Driver::VM2_getCalibrationCB (const CommCB* oCB)
{
	const CommCB_VM2_GetCalibration* o =
	reinterpret_cast<const CommCB_VM2_GetCalibration*> (oCB);

	vm2_->setCalibration (o->index(), o->adc(), o->voltage());
	ackBits_.set (COMM_CBCODE_VM2_GET_CALIBRATION);
}

/************************************************************************/

void Driver::VM2_setCalibrationCB (const CommCB* oCB)
{
	const CommCB_VM2_SetCalibration* o =
	reinterpret_cast<const CommCB_VM2_SetCalibration*> (oCB);

	vm2_->setCalibration (o->index(), o->adc(), o->voltage());
	ackBits_.set (COMM_CBCODE_VM2_SET_CALIBRATION);
}

/************************************************************************/

void Driver::VM2_saveCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_VM2_SAVE_CALIBRATION);
}

/************************************************************************/

void Driver::VM2_readCB (const CommCB* oCB)
{
	const CommCB_VM2_Read* o =
	reinterpret_cast<const CommCB_VM2_Read*> (oCB);

	vm2_->setVoltage (o->voltage());
	ackBits_.set (COMM_CBCODE_VM2_READ);
}

void Driver::VM2_loadDefaultCalibrationCB (const CommCB* oCB)
{
	ackBits_.set (COMM_CBCODE_VM2_LOAD_DEFAULT_CALIBRATION);
}

/************************************************************************/

void Driver::VM_setTerminalCB (const CommCB* oCB)
{
	const CommCB_VM_SetTerminal* o =
	reinterpret_cast<const CommCB_VM_SetTerminal*> (oCB);

	vm_->setTerminal (toVM_Terminal (o->terminal()));
	ackBits_.set (COMM_CBCODE_VM_SET_TERMINAL);
}


void Driver::VM_getTerminalCB (const CommCB* oCB)
{
	const CommCB_VM_GetTerminal* o =
	reinterpret_cast<const CommCB_VM_GetTerminal*> (oCB);

	vm_->setTerminal (toVM_Terminal (o->terminal()));
	ackBits_.set (COMM_CBCODE_VM_GET_TERMINAL);
}

/************************************************************************/

void Driver::changeBaudCB (const CommCB* oCB)
{
	const CommCB_changeBaud* o =
	reinterpret_cast<const CommCB_changeBaud*> (oCB);

	baudRate_ =  o->baudRate();

	comm_->setBaudRate (baudRate_);
	ackBits_.set (COMM_CBCODE_CHANGE_BAUD);
}

/************************************************************************/
/************************************************************************/

void Driver::open (const char* serialNo, float* timeout)
{
	std::cout << "libxsmu version: "
		 << MAJOR_VERSION_NO (versionInfo_->libxsmu_version()) << "."
		 << MINOR_VERSION_NO (versionInfo_->libxsmu_version()) << "."
		 << BUGFIX_VERSION_NO (versionInfo_->libxsmu_version())
		 << std::endl;

	comm_->open (serialNo);

	identify (timeout);
	if (!goodID()) return;

	std::cout << "Hardware version: "
		 << MAJOR_VERSION_NO (versionInfo_->hardware_version()) << '.'
		 << MINOR_VERSION_NO (versionInfo_->hardware_version()) << '.'
		 << BUGFIX_VERSION_NO (versionInfo_->hardware_version())
		 << std::endl;

	std::cout << "Firmware version: "
		 << MAJOR_VERSION_NO (versionInfo_->firmware_version()) << '.'
		 << MINOR_VERSION_NO (versionInfo_->firmware_version()) << '.'
		 << BUGFIX_VERSION_NO (versionInfo_->firmware_version())
		 << std::endl;

	_thread_future = std::async (&Driver::thread, this);
	PRINT_DEBUG ("Asynch thread launched")
}

void Driver::close (void)
{
	try {
		_alive = false;
		_thread_future.get();
	}
	catch (...)
	{}

	comm_->close();
}

bool Driver::waitForResponse (const AckBits & checkBits, float* timeout)
{
	Timer timer;
	double elapsed = 0;
	const double entry = timer.get();

	do {

		comm_->check();
		elapsed = timer.get() - entry;

		if (elapsed >* timeout)
			break;

		/********************************************************
		* Prevents 100% CPU utilization,
		* in case the operation is taking longer than 100ms.
		* *******************************************************/
		if (elapsed > 100e-3)
			timer.sleep (10e-3);

	}
	while ( (ackBits_ & checkBits) != checkBits);

	*timeout = (elapsed >* timeout) ? 0 : (*timeout - elapsed);
	return ( (ackBits_ & checkBits) == checkBits);
}

bool Driver::waitForResponse (uint16_t checkBit, float* timeout)
{
	Timer timer;
	double elapsed = 0;
	const double entry = timer.get();

	do {

		comm_->check();
		elapsed = timer.get() - entry;

		if (elapsed >* timeout)
			break;

		/********************************************************
		* Prevents 100% CPU utilization,
		* in case the operation is taking longer than 100ms.
		* *******************************************************/
		if (elapsed > 100e-3)
			timer.sleep (10e-3);

		if (ackBits_[COMM_CBCODE_NOP])
			throw NoOperation();
	}
	while (!ackBits_[checkBit]);

	*timeout = (elapsed >* timeout) ? 0 : (*timeout - elapsed);
	return ackBits_[checkBit];
}

/************************************************************************/
/************************************************************************/

bool Driver::goodID (void) const
{
	return (identity_ == "XPLORE SMU");
}

void Driver::identify (float* timeout)
{
	identity_.clear();
	versionInfo_->clear();

	ackBits_.reset (COMM_CBCODE_IDN);
	comm_->transmitIdentify();

	waitForResponse (COMM_CBCODE_IDN, timeout);
}

/************************************************************************/

void Driver::keepAlive (uint32_t* lease_time_ms, float* timeout)
{
	auto unique_lock = comm_->lock();
	PRINT_DEBUG ("Lock Acquired")

	ackBits_.reset (COMM_CBCODE_KEEP_ALIVE);
	comm_->transmit_keepAlive (*lease_time_ms);

	waitForResponse (COMM_CBCODE_KEEP_ALIVE, timeout);
}

void Driver::thread (void)
{
	Timer timer;

	uint32_t lease_time_ms = 10000;
	float timeout = 1;

	keepAlive (&lease_time_ms, &timeout);
	PRINT_DEBUG ("Inside thread : Keep Alive sent")
	double last_sent_at = timer.get();
	double elapsed = 0;

	while (_alive)
	{
		elapsed = timer.get() - last_sent_at;
		if (elapsed > lease_time_ms/3)
		{
			timeout = 1;
			keepAlive (&lease_time_ms, &timeout);
			PRINT_DEBUG ("Inside thread : Keep Alive sent")
			last_sent_at = timer.get();
		}

		/********************************************************
		* Prevents 100% CPU utilization,
		* in case the operation is taking longer than 100ms.
		* *******************************************************/
		timer.sleep (10e-3);
	}
}

/************************************************************************/

void Driver::setSourceMode (SourceMode* mode, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_SET_SOURCE_MODE);
	comm_->transmitSourceMode (toComm_SourceMode ((uint16_t)*mode));
	if (waitForResponse (COMM_CBCODE_SET_SOURCE_MODE, timeout)) {

		if (cs_->active())
			*mode = SourceMode::CURRENT;

		if (vs_->active())
			*mode = SourceMode::VOLTAGE;
	}
}

/************************************************************************/
/************************************************************************/

void Driver::CS_setRange (CS_Range* range, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CS_SET_RANGE);
	comm_->transmit_CS_setRange (toComm_CS_Range (*range));

	if (waitForResponse (COMM_CBCODE_CS_SET_RANGE, timeout))
		*range = (cs_->range());
}

/************************************************************************/

void Driver::CS_getCalibration (uint16_t* index, int16_t* dac,
								  float* current, float* timeout)
{
	ackBits_.reset (COMM_CBCODE_CS_GET_CALIBRATION);
	comm_->transmit_CS_getCalibration (*index);

	if (waitForResponse (COMM_CBCODE_CS_GET_CALIBRATION, timeout)) {

		*dac = cs_->calibration_dac (*index);
		*current = cs_->calibration_current (*index);
	}
}

/************************************************************************/

void Driver::CS_verifyCalibration (uint16_t* index, int16_t* dac,
									 float* current, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CS_VERIFY_CALIBRATION);
	comm_->transmit_CS_verifyCalibration (*index);

	if (waitForResponse (COMM_CBCODE_CS_VERIFY_CALIBRATION, timeout)) {

		*dac = cs_->calibration_dac (*index);
		*current = cs_->calibration_current (*index);
	}
}

/************************************************************************/

void Driver::CS_setCalibration (uint16_t* index, int16_t* dac,
								  float* current, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CS_SET_CALIBRATION);
	comm_->transmit_CS_setCalibration (*index,* current);

	if (waitForResponse (COMM_CBCODE_CS_SET_CALIBRATION, timeout)) {

		*dac = cs_->calibration_dac (*index);
		*current = cs_->calibration_current (*index);
	}
}

/************************************************************************/

void Driver::CS_saveCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CS_SAVE_CALIBRATION);
	comm_->transmit_CS_saveCalibration();
	waitForResponse (COMM_CBCODE_CS_SAVE_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::CS_setCurrent (float* current, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CS_SET_CURRENT);
	comm_->transmit_CS_setCurrent (*current);

	if (waitForResponse (COMM_CBCODE_CS_SET_CURRENT, timeout))
		*current = cs_->current();
}

/************************************************************************/
/************************************************************************/

void Driver::VS_setRange (VS_Range* range, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VS_SET_RANGE);
	comm_->transmit_VS_setRange (toComm_VS_Range (*range));

	if (waitForResponse (COMM_CBCODE_VS_SET_RANGE, timeout))
		*range = (vs_->range());
}

/************************************************************************/

void Driver::VS_getCalibration (uint16_t* index, int16_t* dac,
								  float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VS_GET_CALIBRATION);
	comm_->transmit_VS_getCalibration (*index);

	if (waitForResponse (COMM_CBCODE_VS_GET_CALIBRATION, timeout)) {

		*dac = vs_->calibration_dac (*index);
		*voltage = vs_->calibration_voltage (*index);
	}
}

/************************************************************************/

void Driver::VS_verifyCalibration (uint16_t* index, int16_t* dac,
									 float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VS_VERIFY_CALIBRATION);
	comm_->transmit_VS_verifyCalibration (*index);

	if (waitForResponse (COMM_CBCODE_VS_VERIFY_CALIBRATION, timeout)) {

		*dac = vs_->calibration_dac (*index);
		*voltage = vs_->calibration_voltage (*index);
	}
}

/************************************************************************/

void Driver::VS_setCalibration (uint16_t* index, int16_t* dac,
								  float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VS_SET_CALIBRATION);
	comm_->transmit_VS_setCalibration (*index,* voltage);

	if (waitForResponse (COMM_CBCODE_VS_SET_CALIBRATION, timeout)) {

		*dac = vs_->calibration_dac (*index);
		*voltage = vs_->calibration_voltage (*index);
	}
}

/************************************************************************/

void Driver::VS_saveCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VS_SAVE_CALIBRATION);
	comm_->transmit_VS_saveCalibration();
	waitForResponse (COMM_CBCODE_VS_SAVE_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::VS_setVoltage (float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();
	PRINT_DEBUG ("Lock Acquired")

	ackBits_.reset (COMM_CBCODE_VS_SET_VOLTAGE);
	comm_->transmit_VS_setVoltage (*voltage);

	if (waitForResponse (COMM_CBCODE_VS_SET_VOLTAGE, timeout))
		*voltage = vs_->voltage();
}

/************************************************************************/
/************************************************************************/

void Driver::CM_setRange (CM_Range* range, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CM_SET_RANGE);
	comm_->transmit_CM_setRange (toComm_CM_Range (*range));

	if (waitForResponse (COMM_CBCODE_CM_SET_RANGE, timeout))
		*range = (cm_->range());
}

/************************************************************************/

void Driver::CM_getCalibration (uint16_t* index, int32_t* adc,
								  float* current, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CM_GET_CALIBRATION);
	comm_->transmit_CM_getCalibration (*index);

	if (waitForResponse (COMM_CBCODE_CM_GET_CALIBRATION, timeout)) {

		*adc = cm_->calibration_adc (*index);
		*current = cm_->calibration_current (*index);
	}
}

/************************************************************************/

void Driver::CM_setCalibration (uint16_t* index, int32_t* adc,
								  float* current, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CM_SET_CALIBRATION);
	comm_->transmit_CM_setCalibration (*index,* current);

	if (waitForResponse (COMM_CBCODE_CM_SET_CALIBRATION, timeout)) {

		*adc = cm_->calibration_adc (*index);
		*current = cm_->calibration_current (*index);
	}
}

/************************************************************************/

void Driver::CM_saveCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CM_SAVE_CALIBRATION);
	comm_->transmit_CM_saveCalibration();
	waitForResponse (COMM_CBCODE_CM_SAVE_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::CM_read (uint16_t* filterLength, float* current,
						float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CM_READ);
	comm_->transmit_CM_read (*filterLength);
	if (waitForResponse (COMM_CBCODE_CM_READ, timeout)) {

		*current = cm_->current();
	}
}

/************************************************************************/
/************************************************************************/

void Driver::VM_setRange (VM_Range* range, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM_SET_RANGE);
	comm_->transmit_VM_setRange (toComm_VM_Range (*range));

	if (waitForResponse (COMM_CBCODE_VM_SET_RANGE, timeout))
		*range = (vm_->range());
}

/************************************************************************/

void Driver::VM_getCalibration (uint16_t* index, int32_t* adc,
								  float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM_GET_CALIBRATION);
	comm_->transmit_VM_getCalibration (*index);

	if (waitForResponse (COMM_CBCODE_VM_GET_CALIBRATION, timeout)) {

		*adc = vm_->calibration_adc (*index);
		*voltage = vm_->calibration_voltage (*index);
	}
}

/************************************************************************/

void Driver::VM_setCalibration (uint16_t* index, int32_t* adc,
								  float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM_SET_CALIBRATION);
	comm_->transmit_VM_setCalibration (*index,* voltage);

	if (waitForResponse (COMM_CBCODE_VM_SET_CALIBRATION, timeout)) {

		*adc = vm_->calibration_adc (*index);
		*voltage = vm_->calibration_voltage (*index);
	}
}

/************************************************************************/

void Driver::VM_saveCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM_SAVE_CALIBRATION);
	comm_->transmit_VM_saveCalibration();
	waitForResponse (COMM_CBCODE_VM_SAVE_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::VM_read (uint16_t* filterLength, float* voltage,
						float* timeout)
{
	auto unique_lock = comm_->lock();
	
	PRINT_DEBUG ("Lock Acquired")

	ackBits_.reset (COMM_CBCODE_VM_READ);
	comm_->transmit_VM_read (*filterLength);
	if (waitForResponse (COMM_CBCODE_VM_READ, timeout)) {

		*voltage = vm_->voltage();
	}
}

/************************************************************************/
/************************************************************************/

void Driver::CS_loadDefaultCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CS_LOAD_DEFAULT_CALIBRATION);
	comm_->transmit_CS_loadDefaultCalibration();
	waitForResponse (COMM_CBCODE_CS_LOAD_DEFAULT_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::VS_loadDefaultCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VS_LOAD_DEFAULT_CALIBRATION);
	comm_->transmit_VS_loadDefaultCalibration();
	waitForResponse (COMM_CBCODE_VS_LOAD_DEFAULT_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::CM_loadDefaultCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_CM_LOAD_DEFAULT_CALIBRATION);
	comm_->transmit_CM_loadDefaultCalibration();
	waitForResponse (COMM_CBCODE_CM_LOAD_DEFAULT_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::VM_loadDefaultCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM_LOAD_DEFAULT_CALIBRATION);
	comm_->transmit_VM_loadDefaultCalibration();
	waitForResponse (COMM_CBCODE_VM_LOAD_DEFAULT_CALIBRATION, timeout);
}

/************************************************************************/
/************************************************************************/

void Driver::RM_readAutoscale (uint16_t* filterLength,
								  float* resistance,float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_RM_READ_AUTOSCALE);
	comm_->transmit_RM_readAutoscale (*filterLength);
	if (waitForResponse (COMM_CBCODE_RM_READ_AUTOSCALE, timeout)) {
		*resistance = rm_->resistance();
	}
}

/************************************************************************/
/************************************************************************/

void Driver::SystemConfig_Save (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_SAVE);
	comm_->transmit_SystemConfig_Save();
	waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_SAVE, timeout);
}

void Driver::SystemConfig_LoadDefault (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_LOAD_DEFAULT);
	comm_->transmit_SystemConfig_LoadDefault();
	waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_LOAD_DEFAULT, timeout);
}

void Driver::
SystemConfig_Get_hardwareVersion (uint32_t* version, float* timeout)
{
	auto unique_lock = comm_->lock();

	/**** Getting PCB information ****/
	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_GET);
	comm_->transmit_SystemConfig_Get (
		COMM_SYSTEM_CONFIG_PARAM_ID_HW_BOARD_NO);

	if (!waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_GET, timeout))
		return;

	/**** Getting BOM information ****/
	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_GET);
	comm_->transmit_SystemConfig_Get (
		COMM_SYSTEM_CONFIG_PARAM_ID_HW_BOM_NO);

	if (!waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_GET, timeout))
		return;

	/**** Getting bugfix information ****/
	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_GET);
	comm_->transmit_SystemConfig_Get (
		COMM_SYSTEM_CONFIG_PARAM_ID_HW_BUGFIX_NO);

	if (!waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_GET, timeout))
		return;

	/**** Merging all info ****/

	*version = MAKE_VERSION_NO   (sysconf_->hwBoardNo(),
								  sysconf_->hwBomNo(),
								  sysconf_->hwBugfixNo());
}

void Driver::
SystemConfig_Set_hardwareVersion (uint32_t* version, float* timeout)
{
	auto unique_lock = comm_->lock();

	/**** Setting PCB information ****/
	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_SET);
	comm_->transmit_SystemConfig_Set (
		COMM_SYSTEM_CONFIG_PARAM_ID_HW_BOARD_NO,
		MAJOR_VERSION_NO (*version));

	if (!waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_SET, timeout))
		return;

	/**** Setting BOM information ****/
	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_SET);
	comm_->transmit_SystemConfig_Set (
		COMM_SYSTEM_CONFIG_PARAM_ID_HW_BOM_NO,
		MINOR_VERSION_NO (*version));

	if (!waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_SET, timeout))
		return;

	/**** Setting bugfix information ****/
	ackBits_.reset (COMM_CBCODE_SYSTEM_CONFIG_SET);
	comm_->transmit_SystemConfig_Set (
		COMM_SYSTEM_CONFIG_PARAM_ID_HW_BUGFIX_NO,
		BUGFIX_VERSION_NO (*version));

	if (!waitForResponse (COMM_CBCODE_SYSTEM_CONFIG_SET, timeout))
		return;

	/**** Merging all info ****/

	*version = MAKE_VERSION_NO (sysconf_->hwBoardNo(),
							    sysconf_->hwBomNo(),
							    sysconf_->hwBugfixNo());
}

/************************************************************************/
/************************************************************************/

void Driver::VM2_setRange (VM2_Range* range, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM2_SET_RANGE);
	comm_->transmit_VM2_setRange (toComm_VM2_Range (*range));

	if (waitForResponse (COMM_CBCODE_VM2_SET_RANGE, timeout))
		*range = (vm2_->range());
}

/************************************************************************/

void Driver::VM2_getCalibration (uint16_t* index, int32_t* adc,
								  float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM2_GET_CALIBRATION);
	comm_->transmit_VM2_getCalibration (*index);

	if (waitForResponse (COMM_CBCODE_VM2_GET_CALIBRATION, timeout)) {

		*adc = vm2_->calibration_adc (*index);
		*voltage = vm2_->calibration_voltage (*index);
	}
}

/************************************************************************/

void Driver::VM2_setCalibration (uint16_t* index, int32_t* adc,
								  float* voltage, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM2_SET_CALIBRATION);
	comm_->transmit_VM2_setCalibration (*index,* voltage);

	if (waitForResponse (COMM_CBCODE_VM2_SET_CALIBRATION, timeout)) {

		*adc = vm2_->calibration_adc (*index);
		*voltage = vm2_->calibration_voltage (*index);
	}
}

/************************************************************************/

void Driver::VM2_saveCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM2_SAVE_CALIBRATION);
	comm_->transmit_VM2_saveCalibration();
	waitForResponse (COMM_CBCODE_VM2_SAVE_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::VM2_read (uint16_t* filterLength, float* voltage,
						float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM2_READ);
	comm_->transmit_VM2_read (*filterLength);

	if (waitForResponse (COMM_CBCODE_VM2_READ, timeout))
		*voltage = vm2_->voltage();
}

/************************************************************************/

void Driver::VM2_loadDefaultCalibration (float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM2_LOAD_DEFAULT_CALIBRATION);
	comm_->transmit_VM2_loadDefaultCalibration();
	waitForResponse (COMM_CBCODE_VM2_LOAD_DEFAULT_CALIBRATION, timeout);
}

/************************************************************************/

void Driver::VM_setTerminal (VM_Terminal* terminal, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM_SET_TERMINAL);
	comm_->transmit_VM_setTerminal (toComm_VM_Terminal (*terminal));

	if (waitForResponse (COMM_CBCODE_VM_SET_TERMINAL, timeout))
		*terminal = (vm_->terminal());
}

void Driver::VM_getTerminal (VM_Terminal* terminal, float* timeout)
{
	auto unique_lock = comm_->lock();

	ackBits_.reset (COMM_CBCODE_VM_GET_TERMINAL);
	comm_->transmit_VM_getTerminal ();

	if (waitForResponse (COMM_CBCODE_VM_GET_TERMINAL, timeout))
		*terminal = (vm_->terminal());
}

/************************************************************************/
/************************************************************************/

void Driver::changeBaud (uint32_t* baudRate, float* timeout)
{
	auto unique_lock = comm_->lock();
	PRINT_DEBUG ("Lock Acquired")

	ackBits_.reset (COMM_CBCODE_CHANGE_BAUD);

	comm_->transmit_changeBaud (*baudRate);

	if (waitForResponse (COMM_CBCODE_CHANGE_BAUD, timeout))
		*baudRate = baudRate_;
}

/************************************************************************/
/************************************************************************/
}
