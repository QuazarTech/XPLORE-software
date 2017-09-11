#include "../app/Comm.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

namespace smu {

Comm_SourceMode toComm_SourceMode (uint16_t i)
{
	static const Comm_SourceMode values[] =
	{
		COMM_SOURCE_MODE_CURRENT,
		COMM_SOURCE_MODE_VOLTAGE
	};

	return (i < sizeof (values) / sizeof (values[0])) ?
		values[i] : values[0];
}

Comm_CS_Range toComm_CS_Range (uint16_t i)
{
	static const Comm_CS_Range values[] =
	{
		COMM_CS_RANGE_10uA,
		COMM_CS_RANGE_100uA,
		COMM_CS_RANGE_1mA,
		COMM_CS_RANGE_10mA,
		COMM_CS_RANGE_100mA,
	};

	return (i < sizeof (values) / sizeof (values[0])) ?
		values[i] : values[0];
}

Comm_VS_Range toComm_VS_Range (uint16_t i)
{
	static const Comm_VS_Range values[] =
	{
		COMM_VS_RANGE_10V,
		COMM_VS_RANGE_100V
	};

	return (i < sizeof (values) / sizeof (values[0])) ?
		values[i] : values[0];
}

Comm_CM_Range toComm_CM_Range (uint16_t i)
{
	static const Comm_CM_Range values[] =
	{
		COMM_CM_RANGE_10uA,
		COMM_CM_RANGE_100uA,
		COMM_CM_RANGE_1mA,
		COMM_CM_RANGE_10mA,
		COMM_CM_RANGE_100mA
	};

	return (i < sizeof (values) / sizeof (values[0])) ?
		values[i] : values[0];
}

Comm_VM_Range toComm_VM_Range (uint16_t i)
{
	static const Comm_VM_Range values[] =
	{
		COMM_VM_RANGE_1mV,
		COMM_VM_RANGE_10mV,
		COMM_VM_RANGE_100mV,
		COMM_VM_RANGE_1V,
		COMM_VM_RANGE_10V,
		COMM_VM_RANGE_100V
	};

	return (i < sizeof (values) / sizeof (values[0])) ?
		values[i] : values[0];
}

Comm_VM2_Range toComm_VM2_Range (uint16_t i)
{
	static const Comm_VM2_Range values[] =
	{
		COMM_VM2_RANGE_10V
	};

	return (i < sizeof (values) / sizeof (values[0])) ?
		values[i] : values[0];
}

Comm_VM_Terminal toComm_VM_Terminal (uint16_t i)
{
	static const Comm_VM_Terminal values[] =
	{
		COMM_VM_TERMINAL_MEASUREMENT,
		COMM_VM_TERMINAL_SOURCE,
	};

	return (i < sizeof (values) / sizeof (values[0])) ?
		values[i] : values[0];
}

/******************************************************************/
/******************************************************************/

Comm::Comm (void)
{
	qp4_  = new QP4;
	ftdi_ = new FTDI;
}

Comm::~Comm (void)
{
	if (ftdi_->good()) ftdi_->close();
	delete ftdi_;

	delete qp4_;
}

/**********************************************************************/

vector<FTDI_DeviceInfo> Comm::scan (void)
{
	return FTDI::scan();
}

/*************************************************************************/

void Comm::check (void)
{
	checkReceiveQueue();
}

/******************************************************************/
/******************************************************************/

void Comm::transmit (const QP4_Packet* packet)
{
	ftdi_->write (packet, packet->size());
}

/******************************************************************/
/******************************************************************/

void Comm::checkReceiveQueue (void)
{
	char rxbuf[4096];
	uint32_t rxsize;

	while ((rxsize = ftdi_->read (rxbuf, sizeof (rxbuf))))
		processReceivedData (rxbuf, rxsize);
}

void Comm::processReceivedData (const void* data, uint16_t size)
{
	const uint8_t* src = reinterpret_cast<const uint8_t*> (data);

	if (size) do {

		qp4_->receiver().push_back (*src++);

		if (qp4_->receiver().ready()) {

			pair<const void*, uint16_t> packet =
				qp4_->receiver().data();

			interpret (packet.first, packet.second);
			qp4_->receiver().clear();
		}

	} while (--size);
}

void Comm::interpret (const void* data, uint16_t size)
{
	typedef void (Comm::*cb_t)(const void*, uint16_t);

	static const cb_t cbs[] =
	{
		&Comm::nopCB,
		&Comm::identityCB,
		&Comm::syncCB,
		&Comm::setSourceModeCB,

		&Comm::CS_setRangeCB,
		&Comm::CS_getCalibrationCB,
		&Comm::CS_verifyCalibrationCB,
		&Comm::CS_setCalibrationCB,
		&Comm::CS_saveCalibrationCB,
		&Comm::CS_setCurrentCB,

		&Comm::VS_setRangeCB,
		&Comm::VS_getCalibrationCB,
		&Comm::VS_verifyCalibrationCB,
		&Comm::VS_setCalibrationCB,
		&Comm::VS_saveCalibrationCB,
		&Comm::VS_setVoltageCB,

		&Comm::CM_setRangeCB,
		&Comm::CM_getCalibrationCB,
		&Comm::CM_setCalibrationCB,
		&Comm::CM_saveCalibrationCB,
		&Comm::CM_readCB,

		&Comm::VM_setRangeCB,
		&Comm::VM_getCalibrationCB,
		&Comm::VM_setCalibrationCB,
		&Comm::VM_saveCalibrationCB,
		&Comm::VM_readCB,

		&Comm::CS_loadDefaultCalibrationCB,
		&Comm::VS_loadDefaultCalibrationCB,
		&Comm::CM_loadDefaultCalibrationCB,
		&Comm::VM_loadDefaultCalibrationCB,

		&Comm::RM_readAutoscaleCB,

		&Comm::SystemConfig_GetCB,
		&Comm::SystemConfig_SetCB,
		&Comm::SystemConfig_SaveCB,
		&Comm::SystemConfig_LoadDefaultCB,

		&Comm::VM2_setRangeCB,
		&Comm::VM2_getCalibrationCB,
		&Comm::VM2_setCalibrationCB,
		&Comm::VM2_saveCalibrationCB,
		&Comm::VM2_readCB,
		&Comm::VM2_loadDefaultCalibrationCB,

		&Comm::VM_setTerminalCB,
		&Comm::VM_getTerminalCB,
		
		&Comm::changeBaudCB,
	};

	if (size < sizeof (CommPacket))
		return;

	const CommPacket *packet =
		reinterpret_cast<const CommPacket *> (data);

	if (packet->opcode() < sizeof (cbs) / sizeof (cbs[0]))
		(this->*cbs[packet->opcode()])(data, size);
}

/******************************************************************/

void Comm::nopCB (const void* data, uint16_t size)
{}

void Comm::identityCB (const void* data, uint16_t size)
{
	const CommResponse_Identity* res =
		reinterpret_cast<const CommResponse_Identity*> (data);

	do_callback (new (&callbackObject_) CommCB_Identity (
		res->identity(),
		res->containsVersionInfo (size) ? res->hardware_version() : 0,
		res->containsVersionInfo (size) ? res->firmware_version() : 0));
}

void Comm::syncCB (const void* data, uint16_t size)
{
	do_callback (new (&callbackObject_) CommCB_Sync);
}

void Comm::setSourceModeCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_SetSourceMode))
		return;

	const CommResponse_SetSourceMode* res =
		reinterpret_cast<const CommResponse_SetSourceMode*> (data);

	do_callback (new (&callbackObject_) CommCB_SetSourceMode (res->mode()));
}

/******************************************************************/

void Comm::CS_setRangeCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CS_SetRange))
		return;

	const CommResponse_CS_SetRange* res =
		reinterpret_cast<const CommResponse_CS_SetRange*> (data);

	do_callback (new (&callbackObject_) CommCB_CS_SetRange (res->range()));
}

void Comm::CS_getCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CS_GetCalibration))
		return;

	const CommResponse_CS_GetCalibration* res =
		reinterpret_cast <const CommResponse_CS_GetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CS_GetCalibration (res->index(),
								  res->dac(), res->current()));
}

void Comm::CS_verifyCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CS_VerifyCalibration))
		return;

	const CommResponse_CS_VerifyCalibration* res =
		reinterpret_cast <const CommResponse_CS_VerifyCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CS_VerifyCalibration (res->index(),
									 res->dac(), res->current()));
}

void Comm::CS_setCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CS_SetCalibration))
		return;

	const CommResponse_CS_SetCalibration* res =
		reinterpret_cast <const CommResponse_CS_SetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CS_SetCalibration (res->index(),
								  res->dac(), res->current()));
}

void Comm::CS_saveCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CS_SaveCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_CS_SaveCalibration);
}

void Comm::CS_setCurrentCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CS_SetCurrent))
		return;

	const CommResponse_CS_SetCurrent* res =
		reinterpret_cast <const CommResponse_CS_SetCurrent*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CS_SetCurrent (res->current()));
}

/******************************************************************/

void Comm::VS_setRangeCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VS_SetRange))
		return;

	const CommResponse_VS_SetRange* res =
		reinterpret_cast<const CommResponse_VS_SetRange*> (data);

	do_callback (new (&callbackObject_) CommCB_VS_SetRange (res->range()));
}

void Comm::VS_getCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VS_GetCalibration))
		return;

	const CommResponse_VS_GetCalibration* res =
		reinterpret_cast <const CommResponse_VS_GetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VS_GetCalibration (res->index(),
								  res->dac(), res->voltage()));
}

void Comm::VS_verifyCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VS_VerifyCalibration))
		return;

	const CommResponse_VS_VerifyCalibration* res =
		reinterpret_cast <const CommResponse_VS_VerifyCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VS_VerifyCalibration (res->index(),
									 res->dac(), res->voltage()));
}

void Comm::VS_setCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VS_SetCalibration))
		return;

	const CommResponse_VS_SetCalibration* res =
		reinterpret_cast <const CommResponse_VS_SetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VS_SetCalibration (res->index(),
								  res->dac(), res->voltage()));
}

void Comm::VS_saveCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VS_SaveCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_VS_SaveCalibration);
}

void Comm::VS_setVoltageCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VS_SetVoltage))
		return;

	const CommResponse_VS_SetVoltage* res =
		reinterpret_cast <const CommResponse_VS_SetVoltage*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VS_SetVoltage (res->voltage()));
}

/******************************************************************/

void Comm::CM_setRangeCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CM_SetRange))
		return;

	const CommResponse_CM_SetRange* res =
		reinterpret_cast<const CommResponse_CM_SetRange*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CM_SetRange (res->range()));
}

void Comm::CM_getCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CM_GetCalibration))
		return;

	const CommResponse_CM_GetCalibration* res =
		reinterpret_cast <const CommResponse_CM_GetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CM_GetCalibration (res->index(),
								  res->adc(), res->current()));
}

void Comm::CM_setCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CM_SetCalibration))
		return;

	const CommResponse_CM_SetCalibration* res =
		reinterpret_cast <const CommResponse_CM_SetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CM_SetCalibration (res->index(),
								  res->adc(), res->current()));
}

void Comm::CM_saveCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CM_SaveCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_CM_SaveCalibration);
}

void Comm::CM_readCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CM_Read))
		return;

	const CommResponse_CM_Read* res =
		reinterpret_cast <const CommResponse_CM_Read*> (data);

	do_callback (new (&callbackObject_)
		CommCB_CM_Read (res->current()));
}

/******************************************************************/

void Comm::VM_setRangeCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_SetRange))
		return;

	const CommResponse_VM_SetRange* res =
		reinterpret_cast<const CommResponse_VM_SetRange*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM_SetRange (res->range()));
}

void Comm::VM_getCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_GetCalibration))
		return;

	const CommResponse_VM_GetCalibration* res =
		reinterpret_cast <const CommResponse_VM_GetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM_GetCalibration (res->index(),
								  res->adc(), res->voltage()));
}

void Comm::VM_setCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_SetCalibration))
		return;

	const CommResponse_VM_SetCalibration* res =
		reinterpret_cast <const CommResponse_VM_SetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM_SetCalibration (res->index(),
								  res->adc(), res->voltage()));
}

void Comm::VM_saveCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_SaveCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_VM_SaveCalibration);
}

void Comm::VM_readCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_Read))
		return;

	const CommResponse_VM_Read* res =
		reinterpret_cast <const CommResponse_VM_Read*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM_Read (res->voltage()));
}

/******************************************************************/

void Comm::CS_loadDefaultCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CS_LoadDefaultCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_CS_LoadDefaultCalibration);
}

void Comm::VS_loadDefaultCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VS_LoadDefaultCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_VS_LoadDefaultCalibration);
}

void Comm::CM_loadDefaultCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_CM_LoadDefaultCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_CM_LoadDefaultCalibration);
}

void Comm::VM_loadDefaultCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_LoadDefaultCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_VM_LoadDefaultCalibration);
}

/******************************************************************/
/******************************************************************/

void Comm::RM_readAutoscaleCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_RM_ReadAutoscale))
		return;

	const CommResponse_RM_ReadAutoscale* res =
		reinterpret_cast <const CommResponse_RM_ReadAutoscale*> (data);

	do_callback (new (&callbackObject_)
		CommCB_RM_ReadAutoscale (res->resistance()));
}

/******************************************************************/
/******************************************************************/

void Comm::SystemConfig_GetCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_SystemConfig_Get))
		return;

	const CommResponse_SystemConfig_Get* res =
		reinterpret_cast <const CommResponse_SystemConfig_Get*> (data);

	do_callback (new (&callbackObject_)
		CommCB_SystemConfig_Get (res->paramID(), res->value()));
}

void Comm::SystemConfig_SetCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_SystemConfig_Set))
		return;

	const CommResponse_SystemConfig_Set* res =
		reinterpret_cast <const CommResponse_SystemConfig_Set*> (data);

	do_callback (new (&callbackObject_)
		CommCB_SystemConfig_Set (res->paramID(), res->value()));
}

void Comm::SystemConfig_SaveCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_SystemConfig_Save))
		return;

	do_callback (new (&callbackObject_) CommCB_SystemConfig_Save);
}

void Comm::SystemConfig_LoadDefaultCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_SystemConfig_LoadDefault))
		return;

	do_callback (new (&callbackObject_) CommCB_SystemConfig_LoadDefault);
}

/******************************************************************/
/******************************************************************/

void Comm::VM2_setRangeCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM2_SetRange))
		return;

	const CommResponse_VM2_SetRange* res =
		reinterpret_cast<const CommResponse_VM2_SetRange*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM2_SetRange (res->range()));
}

void Comm::VM2_getCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM2_GetCalibration))
		return;

	const CommResponse_VM2_GetCalibration* res =
		reinterpret_cast <const CommResponse_VM2_GetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM2_GetCalibration (res->index(),
								  res->adc(), res->voltage()));
}

void Comm::VM2_setCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM2_SetCalibration))
		return;

	const CommResponse_VM2_SetCalibration* res =
		reinterpret_cast <const CommResponse_VM2_SetCalibration*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM2_SetCalibration (res->index(),
								  res->adc(), res->voltage()));
}

void Comm::VM2_saveCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM2_SaveCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_VM2_SaveCalibration);
}

void Comm::VM2_readCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM2_Read))
		return;

	const CommResponse_VM2_Read* res =
		reinterpret_cast <const CommResponse_VM2_Read*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM2_Read (res->voltage()));
}

void Comm::VM2_loadDefaultCalibrationCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM2_LoadDefaultCalibration))
		return;

	do_callback (new (&callbackObject_) CommCB_VM2_LoadDefaultCalibration);
}

/******************************************************************/

void Comm::VM_setTerminalCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_SetTerminal))
		return;

	const CommResponse_VM_SetTerminal* res =
		reinterpret_cast<const CommResponse_VM_SetTerminal*> (data);

	do_callback (new (&callbackObject_)
		CommCB_VM_SetTerminal (res->terminal()));
}


void Comm::VM_getTerminalCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_VM_GetTerminal))
		return;

	const CommResponse_VM_GetTerminal* res =
		reinterpret_cast<const CommResponse_VM_GetTerminal*> (data);

	do_callback (new (&callbackObject_) CommCB_VM_GetTerminal (res->terminal()));
}

void Comm::changeBaudCB (const void* data, uint16_t size)
{
	if (size < sizeof (CommResponse_changeBaud))
		return;

	const CommResponse_changeBaud* res =
		reinterpret_cast<const CommResponse_changeBaud*> (data);
		
	do_callback (new (&callbackObject_) CommCB_changeBaud (res->baudRate()));
}

/******************************************************************/
/******************************************************************/

void Comm::open (const char* serialNo)
{
	ftdi_->open (serialNo, 9600);
}

void Comm::close (void)
{
	ftdi_->close();
}

/******************************************************************/
/******************************************************************/

void Comm::transmitIdentify (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (sizeof (CommRequest_Identity));

	new (req->body()) CommRequest_Identity;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmitSync (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (sizeof (CommRequest_Synchronize));

	new (req->body()) CommRequest_Synchronize;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmitSourceMode (Comm_SourceMode mode)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_SetSourceMode));

	new (req->body())
		CommRequest_SetSourceMode (mode);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_CS_setRange (Comm_CS_Range range)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CS_SetRange));

	new (req->body()) CommRequest_CS_SetRange (range);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CS_getCalibration (uint16_t index)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CS_GetCalibration));

	new (req->body())
		CommRequest_CS_GetCalibration (index);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CS_verifyCalibration (uint16_t index)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CS_VerifyCalibration));

	new (req->body())
		CommRequest_CS_VerifyCalibration (index);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CS_setCalibration (uint16_t index,
									   float current)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CS_SetCalibration));

	new (req->body())
		CommRequest_CS_SetCalibration (index, current);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CS_saveCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CS_SaveCalibration));

	new (req->body())
		CommRequest_CS_SaveCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CS_setCurrent (float current)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CS_SetCurrent));

	new (req->body())
		CommRequest_CS_SetCurrent (current);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_VS_setRange (Comm_VS_Range range)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VS_SetRange));

	new (req->body())
		CommRequest_VS_SetRange (range);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VS_getCalibration (uint16_t index)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VS_GetCalibration));

	new (req->body())
		CommRequest_VS_GetCalibration (index);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VS_verifyCalibration (uint16_t index)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VS_VerifyCalibration));

	new (req->body())
		CommRequest_VS_VerifyCalibration (index);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VS_setCalibration (uint16_t index,
									   float voltage)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VS_SetCalibration));

	new (req->body())
		CommRequest_VS_SetCalibration (index, voltage);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VS_saveCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VS_SaveCalibration));

	new (req->body())
		CommRequest_VS_SaveCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VS_setVoltage (float voltage)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VS_SetVoltage));

	new (req->body())
		CommRequest_VS_SetVoltage (voltage);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_CM_setRange (Comm_CM_Range range)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CM_SetRange));

	new (req->body())
		CommRequest_CM_SetRange (range);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CM_getCalibration (uint16_t index)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CM_GetCalibration));

	new (req->body())
		CommRequest_CM_GetCalibration (index);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CM_setCalibration (uint16_t index,
									   float current)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CM_SetCalibration));

	new (req->body())
		CommRequest_CM_SetCalibration (index, current);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CM_saveCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CM_SaveCalibration));

	new (req->body())
		CommRequest_CM_SaveCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_CM_read (uint16_t filterLength)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CM_Read));

	new (req->body())
		CommRequest_CM_Read (filterLength);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/
/******************************************************************/

void Comm::transmit_VM_setRange (Comm_VM_Range range)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_SetRange));

	new (req->body())
		CommRequest_VM_SetRange (range);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM_getCalibration (uint16_t index)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_GetCalibration));

	new (req->body())
		CommRequest_VM_GetCalibration (index);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM_setCalibration (uint16_t index,
									   float voltage)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_SetCalibration));

	new (req->body())
		CommRequest_VM_SetCalibration (index, voltage);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM_saveCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_SaveCalibration));

	new (req->body())
		CommRequest_VM_SaveCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM_read (uint16_t filterLength)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_Read));

	new (req->body())
		CommRequest_VM_Read (filterLength);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/
/******************************************************************/

void Comm::transmit_CS_loadDefaultCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CS_LoadDefaultCalibration));

	new (req->body())
		CommRequest_CS_LoadDefaultCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_VS_loadDefaultCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VS_LoadDefaultCalibration));

	new (req->body())
		CommRequest_VS_LoadDefaultCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_CM_loadDefaultCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_CM_LoadDefaultCalibration));

	new (req->body())
		CommRequest_CM_LoadDefaultCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_VM_loadDefaultCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_LoadDefaultCalibration));

	new (req->body())
		CommRequest_VM_LoadDefaultCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/
/******************************************************************/

void Comm::transmit_RM_readAutoscale (uint16_t filterLength)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_RM_ReadAutoscale));

	new (req->body())
		CommRequest_RM_ReadAutoscale (filterLength);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/
/******************************************************************/

void Comm::transmit_SystemConfig_Get (uint16_t paramID)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_SystemConfig_Get));

	new (req->body()) CommRequest_SystemConfig_Get (paramID);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_SystemConfig_Set (uint16_t paramID, int16_t value)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_SystemConfig_Set));

	new (req->body()) CommRequest_SystemConfig_Set (paramID, value);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_SystemConfig_Save (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_SystemConfig_Save));

	new (req->body()) CommRequest_SystemConfig_Save;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_SystemConfig_LoadDefault (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_SystemConfig_LoadDefault));

	new (req->body()) CommRequest_SystemConfig_LoadDefault;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_VM2_setRange (Comm_VM2_Range range)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM2_SetRange));

	new (req->body())
		CommRequest_VM2_SetRange (range);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM2_getCalibration (uint16_t index)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM2_GetCalibration));

	new (req->body())
		CommRequest_VM2_GetCalibration (index);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM2_setCalibration (uint16_t index,
									   float voltage)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM2_SetCalibration));

	new (req->body())
		CommRequest_VM2_SetCalibration (index, voltage);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM2_saveCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM2_SaveCalibration));

	new (req->body())
		CommRequest_VM2_SaveCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM2_read (uint16_t filterLength)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM2_Read));

	new (req->body())
		CommRequest_VM2_Read (filterLength);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM2_loadDefaultCalibration (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM2_LoadDefaultCalibration));

	new (req->body())
		CommRequest_VM2_LoadDefaultCalibration;

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_VM_setTerminal (Comm_VM_Terminal terminal)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_SetTerminal));

	new (req->body())
		CommRequest_VM_SetTerminal (terminal);

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

void Comm::transmit_VM_getTerminal (void)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_VM_GetTerminal));

	new (req->body())
		CommRequest_VM_GetTerminal ();

	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}

/******************************************************************/

void Comm::transmit_changeBaud (uint32_t baudRate)
{
	QP4_Packet* req =
		qp4_->transmitter().alloc_packet (
			sizeof (CommRequest_changeBaud));

	new (req->body())
		CommRequest_changeBaud (baudRate);
	
	req->seal();
	transmit (req);
	qp4_->transmitter().free_packet (req);
}
/******************************************************************/
/******************************************************************/
} // namespace smu
