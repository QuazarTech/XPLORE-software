#ifndef __SMU_COMM__
#define __SMU_COMM__

#include "../../sys/sys/QP4.h"
#include "../../sys/sys/FTDI.h"
#include "../../sys/sys/Applet.h"
#include "../../stl/inet"

#include <vector>
#include <stdint.h>
#include <string>
#include <cstddef>

namespace smu{

/************************************************************************/

enum Comm_Opcode
{
	COMM_OPCODE_NOP,                                                //00
	COMM_OPCODE_IDN,                                                //01
	COMM_OPCODE_KEEP_ALIVE,                                         //02
	COMM_OPCODE_SET_SOURCE_MODE,                                    //03
	COMM_OPCODE_CS_SET_RANGE,                                       //04
	COMM_OPCODE_CS_GET_CALIBRATION,                                 //05
	COMM_OPCODE_CS_VERIFY_CALIBRATION,                              //06
	COMM_OPCODE_CS_SET_CALIBRATION,                                 //07
	COMM_OPCODE_CS_SAVE_CALIBRATION,                                //08
	COMM_OPCODE_CS_SET_CURRENT,                                     //09

	COMM_OPCODE_VS_SET_RANGE,                                       //10
	COMM_OPCODE_VS_GET_CALIBRATION,                                 //11
	COMM_OPCODE_VS_VERIFY_CALIBRATION,                              //12
	COMM_OPCODE_VS_SET_CALIBRATION,                                 //13
	COMM_OPCODE_VS_SAVE_CALIBRATION,                                //14
	COMM_OPCODE_VS_SET_VOLTAGE,                                     //15
	COMM_OPCODE_CM_SET_RANGE,                                       //16
	COMM_OPCODE_CM_GET_CALIBRATION,                                 //17
	COMM_OPCODE_CM_SET_CALIBRATION,                                 //18
	COMM_OPCODE_CM_SAVE_CALIBRATION,                                //19

	COMM_OPCODE_CM_READ,                                            //20
	COMM_OPCODE_VM_SET_RANGE,                                       //21
	COMM_OPCODE_VM_GET_CALIBRATION,                                 //22
	COMM_OPCODE_VM_SET_CALIBRATION,                                 //23
	COMM_OPCODE_VM_SAVE_CALIBRATION,                                //24
	COMM_OPCODE_VM_READ,                                            //25
	COMM_OPCODE_CS_LOAD_DEFAULT_CALIBRATION,                        //26
	COMM_OPCODE_VS_LOAD_DEFAULT_CALIBRATION,                        //27
	COMM_OPCODE_CM_LOAD_DEFAULT_CALIBRATION,                        //28
	COMM_OPCODE_VM_LOAD_DEFAULT_CALIBRATION,                        //29

	COMM_OPCODE_RM_READ_AUTOSCALE,                                  //30
	COMM_OPCODE_SYSTEM_CONFIG_GET,                                  //31
	COMM_OPCODE_SYSTEM_CONFIG_SET,                                  //32
	COMM_OPCODE_SYSTEM_CONFIG_SAVE,                                 //33
	COMM_OPCODE_SYSTEM_CONFIG_LOAD_DEFAULT,                         //34
	COMM_OPCODE_VM2_SET_RANGE,                                      //35
	COMM_OPCODE_VM2_GET_CALIBRATION,                                //36
	COMM_OPCODE_VM2_SET_CALIBRATION,                                //37
	COMM_OPCODE_VM2_SAVE_CALIBRATION,                               //38
	COMM_OPCODE_VM2_READ,                                           //39

	COMM_OPCODE_VM2_LOAD_DEFAULT_CALIBRATION,                       //40
	COMM_OPCODE_VM_SET_TERMINAL,                                    //41
	COMM_OPCODE_VM_GET_TERMINAL,                                    //42

	COMM_OPCODE_CHANGE_BAUD,                                        //43
};

enum Comm_SourceMode
{
	COMM_SOURCE_MODE_CURRENT,
	COMM_SOURCE_MODE_VOLTAGE
};

Comm_SourceMode toComm_SourceMode (uint16_t i);

enum Comm_CS_Range
{
	COMM_CS_RANGE_10uA,
	COMM_CS_RANGE_100uA,
	COMM_CS_RANGE_1mA,
	COMM_CS_RANGE_10mA,
	COMM_CS_RANGE_100mA,
};

Comm_CS_Range toComm_CS_Range (uint16_t i);

enum Comm_VS_Range
{
	COMM_VS_RANGE_10V,
	COMM_VS_RANGE_100V
};

Comm_VS_Range toComm_VS_Range (uint16_t i);

enum Comm_CM_Range
{
	COMM_CM_RANGE_10uA,
	COMM_CM_RANGE_100uA,
	COMM_CM_RANGE_1mA,
	COMM_CM_RANGE_10mA,
	COMM_CM_RANGE_100mA
};

Comm_CM_Range toComm_CM_Range (uint16_t i);

enum Comm_VM_Range
{
	COMM_VM_RANGE_1mV,
	COMM_VM_RANGE_10mV,
	COMM_VM_RANGE_100mV,
	COMM_VM_RANGE_1V,
	COMM_VM_RANGE_10V,
	COMM_VM_RANGE_100V
};

Comm_VM_Range toComm_VM_Range (uint16_t i);

enum Comm_VM2_Range
{
	COMM_VM2_RANGE_10V
};

Comm_VM2_Range toComm_VM2_Range (uint16_t i);


enum Comm_VM_Terminal
{
	COMM_VM_TERMINAL_MEASUREMENT,
	COMM_VM_TERMINAL_SOURCE,
};

Comm_VM_Terminal toComm_VM_Terminal (uint16_t i);

/************************************************************************/

class CommPacket
{
protected:
	CommPacket (uint16_t opcode) :
		opcode_  (smu::hton (opcode)),
		reserve_ (0)
	{}

public:
	uint16_t opcode (void) const { return smu::ntoh (opcode_); }

private:
	uint16_t opcode_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_Identity : public CommPacket
{
protected:
	CommPacket_Identity (void) :
		CommPacket (COMM_OPCODE_IDN)
	{}
};

class CommRequest_Identity : public CommPacket_Identity
{
public:
	CommRequest_Identity (void) {}
};

class CommResponse_Identity : public CommPacket_Identity
{
public:
	const char* identity (void) const { return identity_; }

	uint32_t hardware_version (void) const {
		return smu::ntoh (hardware_version_);
	}

	uint32_t firmware_version (void) const {
		return smu::ntoh (firmware_version_);
	}

public:
	bool containsVersionInfo (uint32_t size) const {

		const uintptr_t min_size =
			(const char*)(&firmware_version_ + 1) - (const char*)this;

		return (size >= min_size);
	}

private:
	char identity_[32];
	uint32_t hardware_version_;
	uint32_t firmware_version_;

private:
	CommResponse_Identity (void);
};

/************************************************************************/

class CommPacket_keepAlive : public CommPacket
{
protected:
	CommPacket_keepAlive (void) :
		CommPacket (COMM_OPCODE_KEEP_ALIVE)
	{}
};

class CommRequest_keepAlive : public CommPacket_keepAlive
{
public:
	CommRequest_keepAlive (uint32_t lease_time_ms) :
		lease_time_ms_ (smu::hton (lease_time_ms))
	{}

private:
	uint32_t lease_time_ms_;
};

class CommResponse_keepAlive : public CommPacket_keepAlive
{
private:
	CommResponse_keepAlive (void);

public:
	uint32_t lease_time_ms (void) const {return smu::ntoh(lease_time_ms_);}

private:
	uint32_t lease_time_ms_;
};

/************************************************************************/

class CommPacket_SetSourceMode : public CommPacket
{
protected:
	CommPacket_SetSourceMode (void) :
		CommPacket (COMM_OPCODE_SET_SOURCE_MODE)
	{}
};

class CommRequest_SetSourceMode : public CommPacket_SetSourceMode
{
public:
	CommRequest_SetSourceMode (Comm_SourceMode mode) :
		mode_ (smu::hton ((uint16_t)(mode))),
		reserve_ (0)
	{}

private:
	uint16_t mode_;
	uint16_t reserve_;
};

class CommResponse_SetSourceMode : public CommPacket_SetSourceMode
{
public:
	Comm_SourceMode mode (void) const {
		return toComm_SourceMode (smu::ntoh (mode_));
	}

private:
	uint16_t mode_;
	uint16_t reserve_;

private:
	CommResponse_SetSourceMode (void);
};

/************************************************************************/
/************************************************************************/

class CommPacket_CS_SetRange : public CommPacket
{
protected:
	CommPacket_CS_SetRange (void) :
		CommPacket (COMM_OPCODE_CS_SET_RANGE)
	{}
};

class CommRequest_CS_SetRange : public CommPacket_CS_SetRange
{
public:
	CommRequest_CS_SetRange (Comm_CS_Range range) :
		range_ (smu::hton ((uint16_t)(range))),
		reserve_ (0)
	{}

private:
	uint16_t range_;
	uint16_t reserve_;
};

class CommResponse_CS_SetRange : public CommPacket_CS_SetRange
{
private:
	CommResponse_CS_SetRange (void);

public:
	Comm_CS_Range range (void) const {
		return toComm_CS_Range (smu::ntoh (range_));
	}

private:
	uint16_t range_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_CS_GetCalibration : public CommPacket
{
protected:
	CommPacket_CS_GetCalibration (void) :
		CommPacket (COMM_OPCODE_CS_GET_CALIBRATION)
	{}
};

class CommRequest_CS_GetCalibration : public CommPacket_CS_GetCalibration
{
public:
	CommRequest_CS_GetCalibration (uint16_t index) :
		index_ (smu::hton (index))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
};

class CommResponse_CS_GetCalibration : public CommPacket_CS_GetCalibration
{
private:
	CommResponse_CS_GetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int16_t  dac     (void) const { return smu::ntoh (dac_);     }
	float    current (void) const { return smu::ntoh (current_); }

private:
	uint16_t index_;
	int16_t dac_;
	float current_;
};

/************************************************************************/

class CommPacket_CS_VerifyCalibration : public CommPacket
{
protected:
	CommPacket_CS_VerifyCalibration (void) :
		CommPacket (COMM_OPCODE_CS_VERIFY_CALIBRATION)
	{}
};

class CommRequest_CS_VerifyCalibration :
	public CommPacket_CS_VerifyCalibration
{
public:
	CommRequest_CS_VerifyCalibration (uint16_t index) :
		index_ (smu::hton (index))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
};

class CommResponse_CS_VerifyCalibration :
	public CommPacket_CS_VerifyCalibration
{
private:
	CommResponse_CS_VerifyCalibration (void);

public:
	uint16_t index (void) const { return smu::ntoh (index_);     }
	int16_t dac    (void) const { return smu::ntoh (dac_);       }
	float current  (void) const { return smu::ntoh (current_);   }

private:
	uint16_t index_;
	int16_t dac_;
	float current_;
};

/************************************************************************/

class CommPacket_CS_SetCalibration : public CommPacket
{
protected:
	CommPacket_CS_SetCalibration (void) :
		CommPacket (COMM_OPCODE_CS_SET_CALIBRATION)
	{}
};

class CommRequest_CS_SetCalibration : public CommPacket_CS_SetCalibration
{
public:
	CommRequest_CS_SetCalibration (uint16_t index, float current) :
		index_   (smu::hton (index)),
		reserve_ (0),
		current_ (smu::hton (current))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
	float    current_;
};

class CommResponse_CS_SetCalibration : public CommPacket_CS_SetCalibration
{
private:
	CommResponse_CS_SetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int16_t  dac     (void) const { return smu::ntoh (dac_);     }
	float    current (void) const { return smu::ntoh (current_); }

private:
	uint16_t index_;
	int16_t  dac_;
	float    current_;
};

/************************************************************************/

class CommPacket_CS_SaveCalibration : public CommPacket
{
protected:
	CommPacket_CS_SaveCalibration (void) :
		CommPacket (COMM_OPCODE_CS_SAVE_CALIBRATION)
	{}
};

class CommRequest_CS_SaveCalibration : public CommPacket_CS_SaveCalibration
{
public:
	CommRequest_CS_SaveCalibration (void) {}
};

class CommResponse_CS_SaveCalibration : public CommPacket_CS_SaveCalibration
{
private:
	CommResponse_CS_SaveCalibration (void);
};

/************************************************************************/

class CommPacket_CS_SetCurrent : public CommPacket
{
protected:
	CommPacket_CS_SetCurrent (void) :
		CommPacket (COMM_OPCODE_CS_SET_CURRENT)
	{}
};

class CommRequest_CS_SetCurrent : public CommPacket_CS_SetCurrent
{
public:
	CommRequest_CS_SetCurrent (float current) :
		current_ (smu::hton (current))
	{}

private:
	float current_;
};

class CommResponse_CS_SetCurrent : public CommPacket_CS_SetCurrent
{
private:
	CommResponse_CS_SetCurrent (void);

public:
	float current (void) const { return smu::ntoh (current_); }

private:
	float current_;
};

/************************************************************************/
/************************************************************************/

class CommPacket_VS_SetRange : public CommPacket
{
protected:
	CommPacket_VS_SetRange (void) :
		CommPacket (COMM_OPCODE_VS_SET_RANGE)
	{}
};

class CommRequest_VS_SetRange : public CommPacket_VS_SetRange
{
public:
	CommRequest_VS_SetRange (Comm_VS_Range range) :
		range_ (smu::hton ((uint16_t)(range))),
		reserve_ (0)
	{}

private:
	uint16_t range_;
	uint16_t reserve_;
};

class CommResponse_VS_SetRange : public CommPacket_VS_SetRange
{
private:
	CommResponse_VS_SetRange (void);

public:
	Comm_VS_Range range (void) const {
		return toComm_VS_Range (smu::ntoh (range_));
	}

private:
	uint16_t range_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_VS_GetCalibration : public CommPacket
{
protected:
	CommPacket_VS_GetCalibration (void) :
		CommPacket (COMM_OPCODE_VS_GET_CALIBRATION)
	{}
};

class CommRequest_VS_GetCalibration : public CommPacket_VS_GetCalibration
{
public:
	CommRequest_VS_GetCalibration (uint16_t index) :
		index_ (smu::hton (index))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
};

class CommResponse_VS_GetCalibration : public CommPacket_VS_GetCalibration
{
private:
	CommResponse_VS_GetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int16_t  dac     (void) const { return smu::ntoh (dac_);     }
	float    voltage (void) const { return smu::ntoh (voltage_); }

private:
	uint16_t index_;
	int16_t dac_;
	float voltage_;
};

/************************************************************************/

class CommPacket_VS_VerifyCalibration : public CommPacket
{
protected:
	CommPacket_VS_VerifyCalibration (void) :
		CommPacket (COMM_OPCODE_VS_VERIFY_CALIBRATION)
	{}
};

class CommRequest_VS_VerifyCalibration :
	public CommPacket_VS_VerifyCalibration
{
public:
	CommRequest_VS_VerifyCalibration (uint16_t index) :
		index_ (smu::hton (index))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
};

class CommResponse_VS_VerifyCalibration :
	public CommPacket_VS_VerifyCalibration
{
private:
	CommResponse_VS_VerifyCalibration (void);

public:
	uint16_t index (void) const { return smu::ntoh (index_);     }
	int16_t dac    (void) const { return smu::ntoh (dac_);       }
	float voltage  (void) const { return smu::ntoh (voltage_);   }

private:
	uint16_t index_;
	int16_t dac_;
	float voltage_;
};

/************************************************************************/

class CommPacket_VS_SetCalibration : public CommPacket
{
protected:
	CommPacket_VS_SetCalibration (void) :
		CommPacket (COMM_OPCODE_VS_SET_CALIBRATION)
	{}
};

class CommRequest_VS_SetCalibration : public CommPacket_VS_SetCalibration
{
public:
	CommRequest_VS_SetCalibration (uint16_t index, float voltage) :
		index_   (smu::hton (index)),
		reserve_ (0),
		voltage_ (smu::hton (voltage))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
	float    voltage_;
};

class CommResponse_VS_SetCalibration : public CommPacket_VS_SetCalibration
{
private:
	CommResponse_VS_SetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int16_t  dac     (void) const { return smu::ntoh (dac_);     }
	float    voltage (void) const { return smu::ntoh (voltage_); }

private:
	uint16_t index_;
	int16_t  dac_;
	float    voltage_;
};

/************************************************************************/

class CommPacket_VS_SaveCalibration : public CommPacket
{
protected:
	CommPacket_VS_SaveCalibration (void) :
		CommPacket (COMM_OPCODE_VS_SAVE_CALIBRATION)
	{}
};

class CommRequest_VS_SaveCalibration : public CommPacket_VS_SaveCalibration
{
public:
	CommRequest_VS_SaveCalibration (void) {}
};

class CommResponse_VS_SaveCalibration : public CommPacket_VS_SaveCalibration
{
private:
	CommResponse_VS_SaveCalibration (void);
};

/************************************************************************/

class CommPacket_VS_SetVoltage : public CommPacket
{
protected:
	CommPacket_VS_SetVoltage (void) :
		CommPacket (COMM_OPCODE_VS_SET_VOLTAGE)
	{}
};

class CommRequest_VS_SetVoltage : public CommPacket_VS_SetVoltage
{
public:
	CommRequest_VS_SetVoltage (float voltage) :
		voltage_ (smu::hton (voltage))
	{}

private:
	float voltage_;
};

class CommResponse_VS_SetVoltage : public CommPacket_VS_SetVoltage
{
private:
	CommResponse_VS_SetVoltage (void);

public:
	float voltage (void) const { return smu::ntoh (voltage_); }

private:
	float voltage_;
};

/************************************************************************/
/************************************************************************/

class CommPacket_CM_SetRange : public CommPacket
{
protected:
	CommPacket_CM_SetRange (void) :
		CommPacket (COMM_OPCODE_CM_SET_RANGE)
	{}
};

class CommRequest_CM_SetRange : public CommPacket_CM_SetRange
{
public:
	CommRequest_CM_SetRange (Comm_CM_Range range) :
		range_   (smu::hton ((uint16_t)(range))),
		reserve_ (0)
	{}

private:
	uint16_t range_;
	uint16_t reserve_;
};

class CommResponse_CM_SetRange : public CommPacket_CM_SetRange
{
private:
	CommResponse_CM_SetRange (void);

public:
	Comm_CM_Range range (void) const {
		return toComm_CM_Range (smu::ntoh (range_));
	}

private:
	uint16_t range_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_CM_GetCalibration : public CommPacket
{
protected:
	CommPacket_CM_GetCalibration (void) :
		CommPacket (COMM_OPCODE_CM_GET_CALIBRATION)
	{}
};

class CommRequest_CM_GetCalibration : public CommPacket_CM_GetCalibration
{
public:
	CommRequest_CM_GetCalibration (uint16_t index) :
		index_   (smu::hton (index)),
		reserve_ (0)
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
};

class CommResponse_CM_GetCalibration : public CommPacket_CM_GetCalibration
{
private:
	CommResponse_CM_GetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int32_t  adc     (void) const { return smu::ntoh (adc_);     }
	float    current (void) const { return smu::ntoh (current_); }

private:
	uint16_t index_;
	uint16_t reserve_;
	int32_t  adc_;
	float    current_;
};

/************************************************************************/

class CommPacket_CM_SetCalibration : public CommPacket
{
protected:
	CommPacket_CM_SetCalibration (void) :
		CommPacket (COMM_OPCODE_CM_SET_CALIBRATION)
	{}
};

class CommRequest_CM_SetCalibration : public CommPacket_CM_SetCalibration
{
public:
	CommRequest_CM_SetCalibration (uint16_t index, float current) :
		index_   (smu::hton (index)),
		reserve_ (0),
		current_ (smu::hton (current))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
	float    current_;
};

class CommResponse_CM_SetCalibration : public CommPacket_CM_SetCalibration
{
private:
	CommResponse_CM_SetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int32_t  adc     (void) const { return smu::ntoh (adc_);     }
	float    current (void) const { return smu::ntoh (current_); }

private:
	uint16_t index_;
	uint16_t reserve_;
	int32_t  adc_;
	float    current_;
};

/************************************************************************/

class CommPacket_CM_SaveCalibration : public CommPacket
{
protected:
	CommPacket_CM_SaveCalibration (void) :
		CommPacket (COMM_OPCODE_CM_SAVE_CALIBRATION)
	{}
};

class CommRequest_CM_SaveCalibration : public CommPacket_CM_SaveCalibration
{
public:
	CommRequest_CM_SaveCalibration (void) {}
};

class CommResponse_CM_SaveCalibration : public CommPacket_CM_SaveCalibration
{
private:
	CommResponse_CM_SaveCalibration (void);
};

/************************************************************************/

class CommPacket_CM_Read : public CommPacket
{
protected:
	CommPacket_CM_Read (void) :
		CommPacket (COMM_OPCODE_CM_READ)
	{}
};

class CommRequest_CM_Read : public CommPacket_CM_Read
{
public:
	CommRequest_CM_Read (uint16_t filterLength) :
		filterLength_ ( smu::hton (filterLength)),
		reserve_      (0)
	{}

private:
	uint16_t filterLength_;
	uint16_t reserve_;
};

class CommResponse_CM_Read : public CommPacket_CM_Read
{
private:
	CommResponse_CM_Read (void);

public:
	float current (void) const { return smu::ntoh (current_); }

private:
	float current_;
};

/************************************************************************/
/************************************************************************/

class CommPacket_VM_SetRange : public CommPacket
{
protected:
	CommPacket_VM_SetRange (void) :
		CommPacket (COMM_OPCODE_VM_SET_RANGE)
	{}
};

class CommRequest_VM_SetRange : public CommPacket_VM_SetRange
{
public:
	CommRequest_VM_SetRange (Comm_VM_Range range) :
		range_   (smu::hton ((uint16_t)(range))),
		reserve_ (0)
	{}

private:
	uint16_t range_;
	uint16_t reserve_;
};

class CommResponse_VM_SetRange : public CommPacket_VM_SetRange
{
private:
	CommResponse_VM_SetRange (void);

public:
	Comm_VM_Range range (void) const {
		return toComm_VM_Range (smu::ntoh (range_));
	}

private:
	uint16_t range_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_VM_GetCalibration : public CommPacket
{
protected:
	CommPacket_VM_GetCalibration (void) :
		CommPacket (COMM_OPCODE_VM_GET_CALIBRATION)
	{}
};

class CommRequest_VM_GetCalibration : public CommPacket_VM_GetCalibration
{
public:
	CommRequest_VM_GetCalibration (uint16_t index) :
		index_   (smu::hton (index)),
		reserve_ (0)
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
};

class CommResponse_VM_GetCalibration : public CommPacket_VM_GetCalibration
{
private:
	CommResponse_VM_GetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int32_t  adc     (void) const { return smu::ntoh (adc_);     }
	float    voltage (void) const { return smu::ntoh (voltage_); }

private:
	uint16_t index_;
	uint16_t reserve_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommPacket_VM_SetCalibration : public CommPacket
{
protected:
	CommPacket_VM_SetCalibration (void) :
		CommPacket (COMM_OPCODE_VM_SET_CALIBRATION)
	{}
};

class CommRequest_VM_SetCalibration : public CommPacket_VM_SetCalibration
{
public:
	CommRequest_VM_SetCalibration (uint16_t index, float voltage) :
		index_   (smu::hton (index)),
		reserve_ (0),
		voltage_ (smu::hton (voltage))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
	float    voltage_;
};

class CommResponse_VM_SetCalibration : public CommPacket_VM_SetCalibration
{
private:
	CommResponse_VM_SetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int32_t  adc     (void) const { return smu::ntoh (adc_);     }
	float    voltage (void) const { return smu::ntoh (voltage_); }

private:
	uint16_t index_;
	uint16_t reserve_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommPacket_VM_SaveCalibration : public CommPacket
{
protected:
	CommPacket_VM_SaveCalibration (void) :
		CommPacket (COMM_OPCODE_VM_SAVE_CALIBRATION)
	{}
};

class CommRequest_VM_SaveCalibration : public CommPacket_VM_SaveCalibration
{
public:
	CommRequest_VM_SaveCalibration (void) {}
};

class CommResponse_VM_SaveCalibration : public CommPacket_VM_SaveCalibration
{
private:
	CommResponse_VM_SaveCalibration (void);
};

/************************************************************************/

class CommPacket_VM_Read : public CommPacket
{
protected:
	CommPacket_VM_Read (void) :
		CommPacket (COMM_OPCODE_VM_READ)
	{}
};

class CommRequest_VM_Read : public CommPacket_VM_Read
{
public:
	CommRequest_VM_Read (uint16_t filterLength) :
		filterLength_ ( smu::hton (filterLength)),
		reserve_      (0)
	{}

private:
	uint16_t filterLength_;
	uint16_t reserve_;
};

class CommResponse_VM_Read : public CommPacket_VM_Read
{
private:
	CommResponse_VM_Read (void);

public:
	float voltage (void) const { return smu::ntoh (voltage_); }

private:
	float voltage_;
};

/************************************************************************/

class CommPacket_CS_LoadDefaultCalibration : public CommPacket
{
protected:
	CommPacket_CS_LoadDefaultCalibration (void) :
		CommPacket (COMM_OPCODE_CS_LOAD_DEFAULT_CALIBRATION)
	{}
};

class CommRequest_CS_LoadDefaultCalibration : public CommPacket_CS_LoadDefaultCalibration
{
public:
	CommRequest_CS_LoadDefaultCalibration (void) {}
};

class CommResponse_CS_LoadDefaultCalibration : public CommPacket_CS_LoadDefaultCalibration
{
private:
	CommResponse_CS_LoadDefaultCalibration (void);
};

/************************************************************************/

class CommPacket_VS_LoadDefaultCalibration : public CommPacket
{
protected:
	CommPacket_VS_LoadDefaultCalibration (void) :
		CommPacket (COMM_OPCODE_VS_LOAD_DEFAULT_CALIBRATION)
	{}
};

class CommRequest_VS_LoadDefaultCalibration : public CommPacket_VS_LoadDefaultCalibration
{
public:
	CommRequest_VS_LoadDefaultCalibration (void) {}
};

class CommResponse_VS_LoadDefaultCalibration : public CommPacket_VS_LoadDefaultCalibration
{
private:
	CommResponse_VS_LoadDefaultCalibration (void);
};

/************************************************************************/

class CommPacket_CM_LoadDefaultCalibration : public CommPacket
{
protected:
	CommPacket_CM_LoadDefaultCalibration (void) :
		CommPacket (COMM_OPCODE_CM_LOAD_DEFAULT_CALIBRATION)
	{}
};

class CommRequest_CM_LoadDefaultCalibration : public CommPacket_CM_LoadDefaultCalibration
{
public:
	CommRequest_CM_LoadDefaultCalibration (void) {}
};

class CommResponse_CM_LoadDefaultCalibration : public CommPacket_CM_LoadDefaultCalibration
{
private:
	CommResponse_CM_LoadDefaultCalibration (void);
};

/************************************************************************/

class CommPacket_VM_LoadDefaultCalibration : public CommPacket
{
protected:
	CommPacket_VM_LoadDefaultCalibration (void) :
		CommPacket (COMM_OPCODE_VM_LOAD_DEFAULT_CALIBRATION)
	{}
};

class CommRequest_VM_LoadDefaultCalibration : public CommPacket_VM_LoadDefaultCalibration
{
public:
	CommRequest_VM_LoadDefaultCalibration (void) {}
};

class CommResponse_VM_LoadDefaultCalibration : public CommPacket_VM_LoadDefaultCalibration
{
private:
	CommResponse_VM_LoadDefaultCalibration (void);
};

/************************************************************************/
/************************************************************************/

class CommPacket_RM_ReadAutoscale : public CommPacket
{
protected:
	CommPacket_RM_ReadAutoscale (void) :
		CommPacket (COMM_OPCODE_RM_READ_AUTOSCALE)
	{}
};

class CommRequest_RM_ReadAutoscale : public CommPacket_RM_ReadAutoscale
{
public:
	CommRequest_RM_ReadAutoscale (uint16_t filterLength) :
		filterLength_ ( smu::hton (filterLength)),
		reserve_      (0)
	{}

private:
	uint16_t filterLength_;
	uint16_t reserve_;
};

class CommResponse_RM_ReadAutoscale : public CommPacket_RM_ReadAutoscale
{
private:
	CommResponse_RM_ReadAutoscale (void);

public:
	float resistance (void) const { return smu::ntoh (resistance_); }

private:
	float resistance_;
};

/************************************************************************/
/************************************************************************/

enum SystemConfigParamID
{
	COMM_SYSTEM_CONFIG_PARAM_ID_HW_BOARD_NO = 0,
	COMM_SYSTEM_CONFIG_PARAM_ID_HW_BOM_NO,
	COMM_SYSTEM_CONFIG_PARAM_ID_HW_BUGFIX_NO,
};

class CommPacket_SystemConfig_Get : public CommPacket
{
protected:
	CommPacket_SystemConfig_Get (void) :
		CommPacket (COMM_OPCODE_SYSTEM_CONFIG_GET)
	{}
};

class CommRequest_SystemConfig_Get : public CommPacket_SystemConfig_Get
{
public:
	CommRequest_SystemConfig_Get (int16_t paramID) :
		paramID_ (smu::hton (paramID)),
		reserve_ (smu::hton (0))
	{}

private:
	int16_t paramID_;
	int16_t reserve_;
};

class CommResponse_SystemConfig_Get : public CommPacket_SystemConfig_Get
{
public:
	int16_t paramID (void) const { return smu::ntoh (paramID_); }
	int16_t value   (void) const { return smu::ntoh (value_);   }

private:
	int16_t paramID_;
	int16_t value_;

private:
	CommResponse_SystemConfig_Get (void);
};

/************************************************************************/

class CommPacket_SystemConfig_Set : public CommPacket
{
protected:
	CommPacket_SystemConfig_Set (void) :
		CommPacket (COMM_OPCODE_SYSTEM_CONFIG_SET)
	{}
};

class CommRequest_SystemConfig_Set : public CommPacket_SystemConfig_Set
{
public:
	CommRequest_SystemConfig_Set (int16_t paramID, int16_t value) :
		paramID_ (smu::hton (paramID)),
		value_   (smu::hton (value))
	{}

private:
	int16_t paramID_;
	int16_t value_;
};

class CommResponse_SystemConfig_Set : public CommPacket_SystemConfig_Set
{
public:
	int16_t paramID (void) const { return smu::ntoh (paramID_); }
	int16_t value   (void) const { return smu::ntoh (value_);   }

private:
	int16_t paramID_;
	int16_t value_;

private:
	CommResponse_SystemConfig_Set (void);
};

/************************************************************************/

class CommPacket_SystemConfig_Save : public CommPacket
{
protected:
	CommPacket_SystemConfig_Save (void) :
		CommPacket (COMM_OPCODE_SYSTEM_CONFIG_SAVE)
	{}
};

class CommRequest_SystemConfig_Save : public CommPacket_SystemConfig_Save
{
public:
	CommRequest_SystemConfig_Save (void) {}
};

class CommResponse_SystemConfig_Save : public CommPacket_SystemConfig_Save
{
private:
	CommResponse_SystemConfig_Save (void);
};

/************************************************************************/

class CommPacket_SystemConfig_LoadDefault : public CommPacket
{
protected:
	CommPacket_SystemConfig_LoadDefault (void) :
		CommPacket (COMM_OPCODE_SYSTEM_CONFIG_LOAD_DEFAULT)
	{}
};

class CommRequest_SystemConfig_LoadDefault :
	public CommPacket_SystemConfig_LoadDefault
{
public:
	CommRequest_SystemConfig_LoadDefault (void) {}
};

class CommResponse_SystemConfig_LoadDefault :
	public CommPacket_SystemConfig_LoadDefault
{
private:
	CommResponse_SystemConfig_LoadDefault (void);
};

/************************************************************************/
/************************************************************************/

class CommPacket_VM2_SetRange : public CommPacket
{
protected:
	CommPacket_VM2_SetRange (void) :
		CommPacket (COMM_OPCODE_VM2_SET_RANGE)
	{}
};

class CommRequest_VM2_SetRange : public CommPacket_VM2_SetRange
{
public:
	CommRequest_VM2_SetRange (Comm_VM2_Range range) :
		range_   (smu::hton ((uint16_t)(range))),
		reserve_ (0)
	{}

private:
	uint16_t range_;
	uint16_t reserve_;
};

class CommResponse_VM2_SetRange : public CommPacket_VM2_SetRange
{
private:
	CommResponse_VM2_SetRange (void);

public:
	Comm_VM2_Range range (void) const {
		return toComm_VM2_Range (smu::ntoh (range_));
	}

private:
	uint16_t range_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_VM2_GetCalibration : public CommPacket
{
protected:
	CommPacket_VM2_GetCalibration (void) :
		CommPacket (COMM_OPCODE_VM2_GET_CALIBRATION)
	{}
};

class CommRequest_VM2_GetCalibration : public CommPacket_VM2_GetCalibration
{
public:
	CommRequest_VM2_GetCalibration (uint16_t index) :
		index_   (smu::hton (index)),
		reserve_ (0)
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
};

class CommResponse_VM2_GetCalibration : public CommPacket_VM2_GetCalibration
{
private:
	CommResponse_VM2_GetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int32_t  adc     (void) const { return smu::ntoh (adc_);     }
	float    voltage (void) const { return smu::ntoh (voltage_); }

private:
	uint16_t index_;
	uint16_t reserve_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommPacket_VM2_SetCalibration : public CommPacket
{
protected:
	CommPacket_VM2_SetCalibration (void) :
		CommPacket (COMM_OPCODE_VM2_SET_CALIBRATION)
	{}
};

class CommRequest_VM2_SetCalibration : public CommPacket_VM2_SetCalibration
{
public:
	CommRequest_VM2_SetCalibration (uint16_t index, float voltage) :
		index_   (smu::hton (index)),
		reserve_ (0),
		voltage_ (smu::hton (voltage))
	{}

private:
	uint16_t index_;
	uint16_t reserve_;
	float    voltage_;
};

class CommResponse_VM2_SetCalibration : public CommPacket_VM2_SetCalibration
{
private:
	CommResponse_VM2_SetCalibration (void);

public:
	uint16_t index   (void) const { return smu::ntoh (index_);   }
	int32_t  adc     (void) const { return smu::ntoh (adc_);     }
	float    voltage (void) const { return smu::ntoh (voltage_); }

private:
	uint16_t index_;
	uint16_t reserve_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommPacket_VM2_SaveCalibration : public CommPacket
{
protected:
	CommPacket_VM2_SaveCalibration (void) :
		CommPacket (COMM_OPCODE_VM2_SAVE_CALIBRATION)
	{}
};

class CommRequest_VM2_SaveCalibration : public CommPacket_VM2_SaveCalibration
{
public:
	CommRequest_VM2_SaveCalibration (void) {}
};

class CommResponse_VM2_SaveCalibration : public CommPacket_VM2_SaveCalibration
{
private:
	CommResponse_VM2_SaveCalibration (void);
};

/************************************************************************/

class CommPacket_VM2_Read : public CommPacket
{
protected:
	CommPacket_VM2_Read (void) :
		CommPacket (COMM_OPCODE_VM2_READ)
	{}
};

class CommRequest_VM2_Read : public CommPacket_VM2_Read
{
public:
	CommRequest_VM2_Read (uint16_t filterLength) :
		filterLength_ ( smu::hton (filterLength)),
		reserve_      (0)
	{}

private:
	uint16_t filterLength_;
	uint16_t reserve_;
};

class CommResponse_VM2_Read : public CommPacket_VM2_Read
{
private:
	CommResponse_VM2_Read (void);

public:
	float voltage (void) const { return smu::ntoh (voltage_); }

private:
	float voltage_;
};

/************************************************************************/

class CommPacket_VM2_LoadDefaultCalibration : public CommPacket
{
protected:
	CommPacket_VM2_LoadDefaultCalibration (void) :
		CommPacket (COMM_OPCODE_VM2_LOAD_DEFAULT_CALIBRATION)
	{}
};

class CommRequest_VM2_LoadDefaultCalibration : public CommPacket_VM2_LoadDefaultCalibration
{
public:
	CommRequest_VM2_LoadDefaultCalibration (void) {}
};

class CommResponse_VM2_LoadDefaultCalibration : public CommPacket_VM2_LoadDefaultCalibration
{
private:
	CommResponse_VM2_LoadDefaultCalibration (void);
};

/************************************************************************/

class CommPacket_VM_SetTerminal : public CommPacket
{
protected:
	CommPacket_VM_SetTerminal (void) :
		CommPacket (COMM_OPCODE_VM_SET_TERMINAL)
	{}
};

class CommRequest_VM_SetTerminal : public CommPacket_VM_SetTerminal
{
public:
	CommRequest_VM_SetTerminal (Comm_VM_Terminal terminal) :
		terminal_   (smu::hton ((uint16_t)(terminal))),
		reserve_ (0)
	{}

private:
	uint16_t terminal_;
	uint16_t reserve_;
};

class CommResponse_VM_SetTerminal : public CommPacket_VM_SetTerminal
{
private:
	CommResponse_VM_SetTerminal (void);

public:
	Comm_VM_Terminal terminal (void) const {
		return toComm_VM_Terminal (smu::ntoh (terminal_));
	}

private:
	uint16_t terminal_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_VM_GetTerminal : public CommPacket
{
protected:
	CommPacket_VM_GetTerminal (void) :
		CommPacket (COMM_OPCODE_VM_GET_TERMINAL)
	{}
};

class CommRequest_VM_GetTerminal : public CommPacket_VM_GetTerminal
{
public:
	CommRequest_VM_GetTerminal (void)
	{}
};

class CommResponse_VM_GetTerminal : public CommPacket_VM_GetTerminal
{
private:
	CommResponse_VM_GetTerminal (void);

public:
	Comm_VM_Terminal terminal (void) const {
		return toComm_VM_Terminal (smu::ntoh (terminal_));
	}

private:
	uint16_t terminal_;
	uint16_t reserve_;
};

/************************************************************************/

class CommPacket_changeBaud : public CommPacket
{
protected:
	CommPacket_changeBaud (void) :
		CommPacket (COMM_OPCODE_CHANGE_BAUD)
	{}
};

class CommRequest_changeBaud : public CommPacket_changeBaud
{
public:
	CommRequest_changeBaud (uint32_t baudRate) :
		baudRate_ (smu::hton (baudRate))
	{}

private:
	uint32_t baudRate_;
};

class CommResponse_changeBaud : public CommPacket_changeBaud
{
private:
	CommResponse_changeBaud (void);

public:
	uint32_t baudRate (void) const {return smu::ntoh(baudRate_);}

private:
	uint32_t baudRate_;
};
/************************************************************************/
/************************************************************************/

enum Comm_CallbackCode
{
	COMM_CBCODE_NOP,                                          //00
	COMM_CBCODE_IDN,                                          //01
	COMM_CBCODE_KEEP_ALIVE,                                   //02

	COMM_CBCODE_SET_SOURCE_MODE,                              //03

	COMM_CBCODE_CS_SET_RANGE,                                 //04
	COMM_CBCODE_CS_GET_CALIBRATION,                           //05
	COMM_CBCODE_CS_VERIFY_CALIBRATION,                        //06
	COMM_CBCODE_CS_SET_CALIBRATION,                           //07
	COMM_CBCODE_CS_SAVE_CALIBRATION,                          //08
	COMM_CBCODE_CS_SET_CURRENT,                               //09

	COMM_CBCODE_VS_SET_RANGE,                                 //10
	COMM_CBCODE_VS_GET_CALIBRATION,                           //11
	COMM_CBCODE_VS_VERIFY_CALIBRATION,                        //12
	COMM_CBCODE_VS_SET_CALIBRATION,                           //13
	COMM_CBCODE_VS_SAVE_CALIBRATION,                          //14
	COMM_CBCODE_VS_SET_VOLTAGE,                               //15

	COMM_CBCODE_CM_SET_RANGE,                                 //16
	COMM_CBCODE_CM_GET_CALIBRATION,                           //17
	COMM_CBCODE_CM_SET_CALIBRATION,                           //18
	COMM_CBCODE_CM_SAVE_CALIBRATION,                          //19
	COMM_CBCODE_CM_READ,                                      //20

	COMM_CBCODE_VM_SET_RANGE,                                 //21
	COMM_CBCODE_VM_GET_CALIBRATION,                           //22
	COMM_CBCODE_VM_SET_CALIBRATION,                           //23
	COMM_CBCODE_VM_SAVE_CALIBRATION,                          //24
	COMM_CBCODE_VM_READ,                                      //25

	COMM_CBCODE_CS_LOAD_DEFAULT_CALIBRATION,                  //26
	COMM_CBCODE_VS_LOAD_DEFAULT_CALIBRATION,                  //27
	COMM_CBCODE_CM_LOAD_DEFAULT_CALIBRATION,                  //28
	COMM_CBCODE_VM_LOAD_DEFAULT_CALIBRATION,                  //29

	COMM_CBCODE_RM_READ_AUTOSCALE,                            //30

	COMM_CBCODE_SYSTEM_CONFIG_GET,                            //31
	COMM_CBCODE_SYSTEM_CONFIG_SET,                            //32
	COMM_CBCODE_SYSTEM_CONFIG_SAVE,                           //33
	COMM_CBCODE_SYSTEM_CONFIG_LOAD_DEFAULT,                   //34

	COMM_CBCODE_VM2_SET_RANGE,                                //35
	COMM_CBCODE_VM2_GET_CALIBRATION,                          //36
	COMM_CBCODE_VM2_SET_CALIBRATION,                          //37
	COMM_CBCODE_VM2_SAVE_CALIBRATION,                         //38
	COMM_CBCODE_VM2_READ,                                     //39
	COMM_CBCODE_VM2_LOAD_DEFAULT_CALIBRATION,                 //40

	COMM_CBCODE_VM_SET_TERMINAL,                              //41
	COMM_CBCODE_VM_GET_TERMINAL,                              //42

	COMM_CBCODE_CHANGE_BAUD,                                  //43
};

/************************************************************************/
/************************************************************************/

class CommCB
{
protected:
	CommCB (Comm_CallbackCode code) :
		code_ (code)
	{}

public:
	Comm_CallbackCode code (void) const {return code_;}

private:
	Comm_CallbackCode code_;
};

/************************************************************************/

class CommCB_Identity : public CommCB
{
public:
	CommCB_Identity (const char* identity,
					 uint32_t hardware_version,
					 uint32_t firmware_version) :
		CommCB (COMM_CBCODE_IDN),
		identity_ (identity),
		hardware_version_ (hardware_version),
		firmware_version_ (firmware_version)
	{}

public:
	const char* identity (void) const { return identity_.c_str(); }
	uint32_t hardware_version (void) const { return hardware_version_; }
	uint32_t firmware_version (void) const { return firmware_version_; }

private:
	std::string identity_;
	uint32_t hardware_version_;
	uint32_t firmware_version_;
};

/************************************************************************/

class CommCB_keepAlive : public CommCB
{
public:
	CommCB_keepAlive (uint32_t lease_time_ms) :
		CommCB (COMM_CBCODE_KEEP_ALIVE),
		lease_time_ms_ (lease_time_ms)
	{}

public:
	uint32_t lease_time_ms (void) const {return lease_time_ms_;}

private:
	uint32_t lease_time_ms_;
};

/************************************************************************/

class CommCB_SetSourceMode : public CommCB
{
public:
	CommCB_SetSourceMode (Comm_SourceMode mode) :
		CommCB (COMM_CBCODE_SET_SOURCE_MODE),
		mode_ (mode)
	{}

public:
	Comm_SourceMode mode (void) const { return mode_; }

private:
	Comm_SourceMode mode_;
};

/************************************************************************/
/************************************************************************/

class CommCB_CS_SetRange : public CommCB
{
public:
	CommCB_CS_SetRange (Comm_CS_Range range) :
		CommCB (COMM_CBCODE_CS_SET_RANGE),
		range_ (range)
	{}

public:
	Comm_CS_Range range (void) const { return range_; }

private:
	Comm_CS_Range range_;
};

/************************************************************************/

class CommCB_CS_GetCalibration : public CommCB
{
public:
	CommCB_CS_GetCalibration (uint16_t index, int16_t dac, float current) :
		CommCB   (COMM_CBCODE_CS_GET_CALIBRATION),
		index_   (index),
		dac_     (dac),
		current_ (current)
	{}

public:
	uint16_t index   (void) const { return index_;   }
	int16_t  dac     (void) const { return dac_;     }
	float    current (void) const { return current_; }

private:
	uint16_t index_;
	int16_t  dac_;
	float    current_;
};

/************************************************************************/

class CommCB_CS_VerifyCalibration : public CommCB
{
public:
	CommCB_CS_VerifyCalibration (uint16_t index, int16_t dac, float current) :
		CommCB   (COMM_CBCODE_CS_VERIFY_CALIBRATION),
		index_   (index),
		dac_     (dac),
		current_ (current)
	{}

public:
	uint16_t index   (void) const { return index_;   }
	int16_t  dac     (void) const { return dac_;     }
	float    current (void) const { return current_; }

private:
	uint16_t index_;
	int16_t  dac_;
	float    current_;
};

/************************************************************************/

class CommCB_CS_SetCalibration : public CommCB
{
public:
	CommCB_CS_SetCalibration (uint16_t index, int16_t dac, float current) :
		CommCB   (COMM_CBCODE_CS_SET_CALIBRATION),
		index_   (index),
		dac_     (dac),
		current_ (current)
	{}

public:
	uint16_t index   (void) const { return index_;   }
	int16_t  dac     (void) const { return dac_;     }
	float    current (void) const { return current_; }

private:
	uint16_t index_;
	int16_t  dac_;
	float    current_;
};

/************************************************************************/

class CommCB_CS_SaveCalibration : public CommCB
{
public:
	CommCB_CS_SaveCalibration (void) :
		CommCB (COMM_CBCODE_CS_SAVE_CALIBRATION)
	{}
};

class CommCB_CS_SetCurrent : public CommCB
{
public:
	CommCB_CS_SetCurrent (float current) :
		CommCB (COMM_CBCODE_CS_SET_CURRENT),
		current_ (current)
	{}

public:
	float current (void) const { return current_; }

private:
	float current_;
};

/************************************************************************/
/************************************************************************/

class CommCB_VS_SetRange : public CommCB
{
public:
	CommCB_VS_SetRange (Comm_VS_Range range) :
		CommCB (COMM_CBCODE_VS_SET_RANGE),
		range_ (range)
	{}

public:
	Comm_VS_Range range (void) const { return range_; }

private:
	Comm_VS_Range range_;
};

/************************************************************************/

class CommCB_VS_GetCalibration : public CommCB
{
public:
	CommCB_VS_GetCalibration (uint16_t index, int16_t dac, float voltage) :
		CommCB   (COMM_CBCODE_VS_GET_CALIBRATION),
		index_   (index),
		dac_     (dac),
		voltage_ (voltage)
	{}

public:
	uint16_t index   (void) const { return index_;   }
	int16_t  dac     (void) const { return dac_;     }
	float    voltage (void) const { return voltage_; }

private:
	uint16_t index_;
	int16_t  dac_;
	float    voltage_;
};

/************************************************************************/

class CommCB_VS_VerifyCalibration : public CommCB
{
public:
	CommCB_VS_VerifyCalibration (uint16_t index, int16_t dac, float voltage) :
		CommCB   (COMM_CBCODE_VS_VERIFY_CALIBRATION),
		index_   (index),
		dac_     (dac),
		voltage_ (voltage)
	{}

public:
	uint16_t index   (void) const { return index_;   }
	int16_t  dac     (void) const { return dac_;     }
	float    voltage (void) const { return voltage_; }

private:
	uint16_t index_;
	int16_t  dac_;
	float    voltage_;
};

/************************************************************************/

class CommCB_VS_SetCalibration : public CommCB
{
public:
	CommCB_VS_SetCalibration (uint16_t index, int16_t dac, float voltage) :
		CommCB   (COMM_CBCODE_VS_SET_CALIBRATION),
		index_   (index),
		dac_     (dac),
		voltage_ (voltage)
	{}

public:
	uint16_t index   (void) const { return index_;   }
	int16_t  dac     (void) const { return dac_;     }
	float    voltage (void) const { return voltage_; }

private:
	uint16_t index_;
	int16_t  dac_;
	float    voltage_;
};

/************************************************************************/

class CommCB_VS_SaveCalibration : public CommCB
{
public:
	CommCB_VS_SaveCalibration (void) :
		CommCB (COMM_CBCODE_VS_SAVE_CALIBRATION)
	{}
};

class CommCB_VS_SetVoltage : public CommCB
{
public:
	CommCB_VS_SetVoltage (float voltage) :
		CommCB (COMM_CBCODE_VS_SET_VOLTAGE),
		voltage_ (voltage)
	{}

public:
	float voltage (void) const { return voltage_; }

private:
	float voltage_;
};

/************************************************************************/
/************************************************************************/

class CommCB_CM_SetRange : public CommCB
{
public:
	CommCB_CM_SetRange (Comm_CM_Range range) :
		CommCB (COMM_CBCODE_CM_SET_RANGE),
		range_ (range)
	{}

public:
	Comm_CM_Range range (void) const { return range_; }

private:
	Comm_CM_Range range_;
};

/************************************************************************/

class CommCB_CM_GetCalibration : public CommCB
{
public:
	CommCB_CM_GetCalibration (uint16_t index, int32_t adc, float current) :
		CommCB   (COMM_CBCODE_CM_GET_CALIBRATION),
		index_   (index),
		adc_     (adc),
		current_ (current)
	{}

public:
	uint16_t index   (void) const { return index_; }
	int32_t  adc     (void) const { return adc_;   }
	float    current (void) const { return current_; }

private:
	uint16_t index_;
	int32_t  adc_;
	float    current_;
};

/************************************************************************/

class CommCB_CM_SetCalibration : public CommCB
{
public:
	CommCB_CM_SetCalibration (uint16_t index, int32_t adc, float current) :
		CommCB   (COMM_CBCODE_CM_SET_CALIBRATION),
		index_   (index),
		adc_     (adc),
		current_ (current)
	{}

public:
	uint16_t index   (void) const { return index_; }
	int32_t  adc     (void) const { return adc_;   }
	float    current (void) const { return current_; }

private:
	uint16_t index_;
	int32_t  adc_;
	float    current_;
};

/************************************************************************/

class CommCB_CM_SaveCalibration : public CommCB
{
public:
	CommCB_CM_SaveCalibration (void) :
		CommCB (COMM_CBCODE_CM_SAVE_CALIBRATION)
	{}
};

/************************************************************************/

class CommCB_CM_Read : public CommCB
{
public:
	CommCB_CM_Read (float current) :
		CommCB (COMM_CBCODE_CM_READ),
		current_ (current)
	{}

public:
	float current (void) const { return current_; }

private:
	float current_;
};

/************************************************************************/
/************************************************************************/

class CommCB_VM_SetRange : public CommCB
{
public:
	CommCB_VM_SetRange (Comm_VM_Range range) :
		CommCB (COMM_CBCODE_VM_SET_RANGE),
		range_ (range)
	{}

public:
	Comm_VM_Range range (void) const { return range_; }

private:
	Comm_VM_Range range_;
};

/************************************************************************/

class CommCB_VM_GetCalibration : public CommCB
{
public:
	CommCB_VM_GetCalibration (uint16_t index, int32_t adc, float voltage) :
		CommCB   (COMM_CBCODE_VM_GET_CALIBRATION),
		index_   (index),
		adc_     (adc),
		voltage_ (voltage)
	{}

public:
	uint16_t index   (void) const { return index_; }
	int32_t  adc     (void) const { return adc_;   }
	float    voltage (void) const { return voltage_; }

private:
	uint16_t index_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommCB_VM_SetCalibration : public CommCB
{
public:
	CommCB_VM_SetCalibration (uint16_t index, int32_t adc, float voltage) :
		CommCB   (COMM_CBCODE_VM_SET_CALIBRATION),
		index_   (index),
		adc_     (adc),
		voltage_ (voltage)
	{}

public:
	uint16_t index   (void) const { return index_; }
	int32_t  adc     (void) const { return adc_;   }
	float    voltage (void) const { return voltage_; }

private:
	uint16_t index_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommCB_VM_SaveCalibration : public CommCB
{
public:
	CommCB_VM_SaveCalibration (void) :
		CommCB (COMM_CBCODE_VM_SAVE_CALIBRATION)
	{}
};

/************************************************************************/

class CommCB_VM_Read : public CommCB
{
public:
	CommCB_VM_Read (float voltage) :
		CommCB (COMM_CBCODE_VM_READ),
		voltage_ (voltage)
	{}

public:
	float voltage (void) const { return voltage_; }

private:
	float voltage_;
};

/************************************************************************/
/************************************************************************/

class CommCB_CS_LoadDefaultCalibration : public CommCB
{
public:
	CommCB_CS_LoadDefaultCalibration (void) :
		CommCB (COMM_CBCODE_CS_LOAD_DEFAULT_CALIBRATION)
	{}
};

/************************************************************************/

class CommCB_VS_LoadDefaultCalibration : public CommCB
{
public:
	CommCB_VS_LoadDefaultCalibration (void) :
		CommCB (COMM_CBCODE_VS_LOAD_DEFAULT_CALIBRATION)
	{}
};

/************************************************************************/

class CommCB_CM_LoadDefaultCalibration : public CommCB
{
public:
	CommCB_CM_LoadDefaultCalibration (void) :
		CommCB (COMM_CBCODE_CM_LOAD_DEFAULT_CALIBRATION)
	{}
};

/************************************************************************/

class CommCB_VM_LoadDefaultCalibration : public CommCB
{
public:
	CommCB_VM_LoadDefaultCalibration (void) :
		CommCB (COMM_CBCODE_VM_LOAD_DEFAULT_CALIBRATION)
	{}
};

/************************************************************************/
/************************************************************************/

class CommCB_RM_ReadAutoscale : public CommCB
{
public:
	CommCB_RM_ReadAutoscale (float resistance) :
		CommCB (COMM_CBCODE_RM_READ_AUTOSCALE),
		resistance_ (resistance)
	{}

public:
	float resistance (void) const { return resistance_; }

private:
	float resistance_;
};

/************************************************************************/
/************************************************************************/

class CommCB_SystemConfig_Get : public CommCB
{
public:
	CommCB_SystemConfig_Get (int16_t paramID, int16_t value) :
		CommCB (COMM_CBCODE_SYSTEM_CONFIG_GET),
		paramID_ (paramID),
		value_ (value)
	{}

public:
	int16_t paramID (void) const { return paramID_; }
	int16_t value   (void) const { return value_;   }

private:
	int16_t paramID_;
	int16_t value_;
};

class CommCB_SystemConfig_Set : public CommCB
{
public:
	CommCB_SystemConfig_Set (int16_t paramID, int16_t value) :
		CommCB (COMM_CBCODE_SYSTEM_CONFIG_SET),
		paramID_ (paramID),
		value_ (value)
	{}

public:
	int16_t paramID (void) const { return paramID_; }
	int16_t value   (void) const { return value_;   }

private:
	int16_t paramID_;
	int16_t value_;
};

class CommCB_SystemConfig_Save : public CommCB
{
public:
	CommCB_SystemConfig_Save (void) :
		CommCB (COMM_CBCODE_SYSTEM_CONFIG_SAVE)
	{}
};

class CommCB_SystemConfig_LoadDefault : public CommCB
{
public:
	CommCB_SystemConfig_LoadDefault (void) :
		CommCB (COMM_CBCODE_SYSTEM_CONFIG_LOAD_DEFAULT)
	{}
};

/************************************************************************/
/************************************************************************/

class CommCB_VM2_SetRange : public CommCB
{
public:
	CommCB_VM2_SetRange (Comm_VM2_Range range) :
		CommCB (COMM_CBCODE_VM2_SET_RANGE),
		range_ (range)
	{}

public:
	Comm_VM2_Range range (void) const { return range_; }

private:
	Comm_VM2_Range range_;
};

/************************************************************************/

class CommCB_VM2_GetCalibration : public CommCB
{
public:
	CommCB_VM2_GetCalibration (uint16_t index, int32_t adc, float voltage) :
		CommCB   (COMM_CBCODE_VM2_GET_CALIBRATION),
		index_   (index),
		adc_     (adc),
		voltage_ (voltage)
	{}

public:
	uint16_t index   (void) const { return index_; }
	int32_t  adc     (void) const { return adc_;   }
	float    voltage (void) const { return voltage_; }

private:
	uint16_t index_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommCB_VM2_SetCalibration : public CommCB
{
public:
	CommCB_VM2_SetCalibration (uint16_t index, int32_t adc, float voltage) :
		CommCB   (COMM_CBCODE_VM2_SET_CALIBRATION),
		index_   (index),
		adc_     (adc),
		voltage_ (voltage)
	{}

public:
	uint16_t index   (void) const { return index_; }
	int32_t  adc     (void) const { return adc_;   }
	float    voltage (void) const { return voltage_; }

private:
	uint16_t index_;
	int32_t  adc_;
	float    voltage_;
};

/************************************************************************/

class CommCB_VM2_SaveCalibration : public CommCB
{
public:
	CommCB_VM2_SaveCalibration (void) :
		CommCB (COMM_CBCODE_VM2_SAVE_CALIBRATION)
	{}
};

/************************************************************************/

class CommCB_VM2_Read : public CommCB
{
public:
	CommCB_VM2_Read (float voltage) :
		CommCB (COMM_CBCODE_VM2_READ),
		voltage_ (voltage)
	{}

public:
	float voltage (void) const { return voltage_; }

private:
	float voltage_;
};

/************************************************************************/

class CommCB_VM2_LoadDefaultCalibration : public CommCB
{
public:
	CommCB_VM2_LoadDefaultCalibration (void) :
		CommCB (COMM_CBCODE_VM2_LOAD_DEFAULT_CALIBRATION)
	{}
};

/************************************************************************/

class CommCB_VM_SetTerminal : public CommCB
{
public:
	CommCB_VM_SetTerminal (Comm_VM_Terminal terminal) :
		CommCB (COMM_CBCODE_VM_SET_TERMINAL),
		terminal_ (terminal)
	{}

public:
	Comm_VM_Terminal terminal (void) const { return terminal_; }

private:
	Comm_VM_Terminal terminal_;
};

/************************************************************************/

class CommCB_VM_GetTerminal : public CommCB
{
public:
	CommCB_VM_GetTerminal (Comm_VM_Terminal terminal) :
		CommCB (COMM_CBCODE_VM_GET_TERMINAL),
		terminal_ (terminal)
	{}

public:
	Comm_VM_Terminal terminal (void) const { return terminal_; }

private:
	Comm_VM_Terminal terminal_;
};

/************************************************************************/

class CommCB_changeBaud : public CommCB
{
public:
	CommCB_changeBaud (uint32_t baudRate) :
		CommCB (COMM_CBCODE_CHANGE_BAUD),
		baudRate_ (baudRate)
	{}

public:
	uint32_t baudRate (void) const {return baudRate_;}

private:
	uint32_t baudRate_;
};
/************************************************************************/
/************************************************************************/

union CommCB_Union
{
	char gen0[sizeof (CommCB)];
	char gen1[sizeof (CommCB_Identity)];
	char gen2[sizeof (CommCB_keepAlive)];
	char gen3[sizeof (CommCB_SetSourceMode)];
	char gen4[sizeof (CommCB_changeBaud)];

	char cs0[sizeof (CommCB_CS_SetRange)];
	char cs1[sizeof (CommCB_CS_GetCalibration)];
	char cs2[sizeof (CommCB_CS_VerifyCalibration)];
	char cs3[sizeof (CommCB_CS_SetCalibration)];
	char cs4[sizeof (CommCB_CS_SaveCalibration)];
	char cs5[sizeof (CommCB_CS_SetCurrent)];
	char cs6[sizeof (CommCB_CS_LoadDefaultCalibration)];

	char vs0[sizeof (CommCB_VS_SetRange)];
	char vs1[sizeof (CommCB_VS_GetCalibration)];
	char vs2[sizeof (CommCB_VS_VerifyCalibration)];
	char vs3[sizeof (CommCB_VS_SetCalibration)];
	char vs4[sizeof (CommCB_VS_SaveCalibration)];
	char vs5[sizeof (CommCB_VS_SetVoltage)];
	char vs6[sizeof (CommCB_VS_LoadDefaultCalibration)];

	char cm0[sizeof (CommCB_CM_SetRange)];
	char cm1[sizeof (CommCB_CM_GetCalibration)];
	char cm2[sizeof (CommCB_CM_SetCalibration)];
	char cm3[sizeof (CommCB_CM_SaveCalibration)];
	char cm4[sizeof (CommCB_CM_Read)];
	char cm5[sizeof (CommCB_CM_LoadDefaultCalibration)];

	char vm0[sizeof (CommCB_VM_SetRange)];
	char vm1[sizeof (CommCB_VM_GetCalibration)];
	char vm2[sizeof (CommCB_VM_SetCalibration)];
	char vm3[sizeof (CommCB_VM_SaveCalibration)];
	char vm4[sizeof (CommCB_VM_Read)];
	char vm5[sizeof (CommCB_VM_LoadDefaultCalibration)];

	char rm0[sizeof (CommCB_RM_ReadAutoscale)];

	char sysconf0[sizeof (CommCB_SystemConfig_Get)];
	char sysconf1[sizeof (CommCB_SystemConfig_Set)];
	char sysconf2[sizeof (CommCB_SystemConfig_Save)];
	char sysconf3[sizeof (CommCB_SystemConfig_LoadDefault)];

	char vm2_0[sizeof (CommCB_VM2_SetRange)];
	char vm2_1[sizeof (CommCB_VM2_GetCalibration)];
	char vm2_2[sizeof (CommCB_VM2_SetCalibration)];
	char vm2_3[sizeof (CommCB_VM2_SaveCalibration)];
	char vm2_4[sizeof (CommCB_VM2_Read)];
	char vm2_5[sizeof (CommCB_VM2_LoadDefaultCalibration)];

	char vm6[sizeof (CommCB_VM_SetTerminal)];
	char vm7[sizeof (CommCB_VM_GetTerminal)];
};

/************************************************************************/
/************************************************************************/

class Comm : public Applet
{
public:
	Comm (void);
	~Comm (void);

public:
	static std::vector<FTDI_DeviceInfo> scan (void);

public:
	void open  (const char* serialNo);
	void close (void);

public:
	void check (void);
	void transmitIdentify (void);
	void transmit_keepAlive (uint32_t lease_time_ms);
	void transmitSourceMode (Comm_SourceMode mode);

	/********************************/

	void transmit_CS_setRange            (Comm_CS_Range range);
	void transmit_CS_getCalibration      (uint16_t index);
	void transmit_CS_verifyCalibration   (uint16_t index);
	void transmit_CS_setCalibration      (uint16_t index, float current);
	void transmit_CS_saveCalibration     (void);
	void transmit_CS_setCurrent          (float current);

	/********************************/

	void transmit_VS_setRange            (Comm_VS_Range range);
	void transmit_VS_getCalibration      (uint16_t index);
	void transmit_VS_verifyCalibration   (uint16_t index);
	void transmit_VS_setCalibration      (uint16_t index, float voltage);
	void transmit_VS_saveCalibration     (void);
	void transmit_VS_setVoltage          (float voltage);

	/********************************/

	void transmit_CM_setRange            (Comm_CM_Range range);
	void transmit_CM_getCalibration      (uint16_t index);
	void transmit_CM_setCalibration      (uint16_t index, float current);
	void transmit_CM_saveCalibration     (void);
	void transmit_CM_read                (uint16_t filterLength);

	/********************************/

	void transmit_VM_setRange            (Comm_VM_Range range);
	void transmit_VM_getCalibration      (uint16_t index);
	void transmit_VM_setCalibration      (uint16_t index, float voltage);
	void transmit_VM_saveCalibration     (void);
	void transmit_VM_read                (uint16_t filterLength);

	/********************************/

	void transmit_CS_loadDefaultCalibration (void);
	void transmit_VS_loadDefaultCalibration (void);
	void transmit_CM_loadDefaultCalibration (void);
	void transmit_VM_loadDefaultCalibration (void);

	/********************************/

	void transmit_RM_readAutoscale (uint16_t filterLength);

	/********************************/

	void transmit_SystemConfig_Get (uint16_t paramID);
	void transmit_SystemConfig_Set (uint16_t paramID, int16_t value);
	void transmit_SystemConfig_Save (void);
	void transmit_SystemConfig_LoadDefault (void);

	/********************************/

	void transmit_VM2_setRange            (Comm_VM2_Range range);
	void transmit_VM2_getCalibration      (uint16_t index);
	void transmit_VM2_setCalibration      (uint16_t index, float voltage);
	void transmit_VM2_saveCalibration     (void);
	void transmit_VM2_read                (uint16_t filterLength);
	void transmit_VM2_loadDefaultCalibration (void);

	/********************************/

	void transmit_VM_setTerminal (Comm_VM_Terminal terminal);
	void transmit_VM_getTerminal (void);

	/********************************/

	void transmit_changeBaud     (uint32_t baudRate);

private:
	QP4* qp4_;
	FTDI* ftdi_;
	CommCB_Union callbackObject_;

private:
	void checkReceiveQueue        (void);

	void processReceivedData      (const void* data, uint16_t size);
	void interpret                (const void* data, uint16_t size);

	void nopCB                    (const void* data, uint16_t size);
	void identityCB               (const void* data, uint16_t size);
	void keepAliveCB              (const void* data, uint16_t size);

	void setSourceModeCB          (const void* data, uint16_t size);

	void CS_setRangeCB            (const void* data, uint16_t size);
	void CS_getCalibrationCB      (const void* data, uint16_t size);
	void CS_verifyCalibrationCB   (const void* data, uint16_t size);
	void CS_setCalibrationCB      (const void* data, uint16_t size);
	void CS_saveCalibrationCB     (const void* data, uint16_t size);
	void CS_setCurrentCB          (const void* data, uint16_t size);

	void VS_setRangeCB            (const void* data, uint16_t size);
	void VS_getCalibrationCB      (const void* data, uint16_t size);
	void VS_verifyCalibrationCB   (const void* data, uint16_t size);
	void VS_setCalibrationCB      (const void* data, uint16_t size);
	void VS_saveCalibrationCB     (const void* data, uint16_t size);
	void VS_setVoltageCB          (const void* data, uint16_t size);

	void CM_setRangeCB            (const void* data, uint16_t size);
	void CM_getCalibrationCB      (const void* data, uint16_t size);
	void CM_setCalibrationCB      (const void* data, uint16_t size);
	void CM_saveCalibrationCB     (const void* data, uint16_t size);
	void CM_readCB                (const void* data, uint16_t size);

	void VM_setRangeCB            (const void* data, uint16_t size);
	void VM_getCalibrationCB      (const void* data, uint16_t size);
	void VM_setCalibrationCB      (const void* data, uint16_t size);
	void VM_saveCalibrationCB     (const void* data, uint16_t size);
	void VM_readCB                (const void* data, uint16_t size);

	void CS_loadDefaultCalibrationCB (const void* data, uint16_t size);
	void VS_loadDefaultCalibrationCB (const void* data, uint16_t size);
	void CM_loadDefaultCalibrationCB (const void* data, uint16_t size);
	void VM_loadDefaultCalibrationCB (const void* data, uint16_t size);

	void RM_readAutoscaleCB (const void* data, uint16_t size);

	void SystemConfig_GetCB         (const void* data, uint16_t size);
	void SystemConfig_SetCB         (const void* data, uint16_t size);
	void SystemConfig_SaveCB        (const void* data, uint16_t size);
	void SystemConfig_LoadDefaultCB (const void* data, uint16_t size);

	void VM2_setRangeCB               (const void* data, uint16_t size);
	void VM2_getCalibrationCB         (const void* data, uint16_t size);
	void VM2_setCalibrationCB         (const void* data, uint16_t size);
	void VM2_saveCalibrationCB        (const void* data, uint16_t size);
	void VM2_readCB                   (const void* data, uint16_t size);
	void VM2_loadDefaultCalibrationCB (const void* data, uint16_t size);

	void VM_setTerminalCB (const void* data, uint16_t size);
	void VM_getTerminalCB (const void* data, uint16_t size);

	void changeBaudCB     (const void* data, uint16_t size);

private:
	void transmit (const QP4_Packet* packet);

public:
	void setBaudRate (uint32_t baudRate);
};

/************************************************************************/
} // namespace smu

#endif
