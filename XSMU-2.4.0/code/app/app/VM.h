#ifndef __SMU_VM__
#define __SMU_VM__

#include <stdint.h>
#include "Calibration.h"

namespace smu {

enum VM_Range
{
	VM_RANGE_1mV,
	VM_RANGE_10mV,
	VM_RANGE_100mV,
	VM_RANGE_1V,
	VM_RANGE_10V,
	VM_RANGE_100V
};

VM_Range toVM_Range (uint16_t i);

enum VM_Terminal
{
	VM_TERMINAL_MEASUREMENT,
	VM_TERMINAL_SOURCE,
};

VM_Terminal toVM_Terminal (uint16_t i);

class VM
{
public:
	VM (void);

public:
	VM_Range range   (void) const { return range_;   }
	float    voltage (void) const { return voltage_; }

public:
	void setRange   (VM_Range range) { range_ = range;     }
	void setVoltage (float voltage)  { voltage_ = voltage; }

public:
	void setCalibration (uint16_t index, int32_t adc, float voltage) {
		calibration_[index].set (adc, voltage);
	}

	int32_t calibration_adc (uint16_t index) const {
		return calibration_[index].first();
	}

	float calibration_voltage (uint16_t index) const {
		return calibration_[index].second();
	}


public:
	void setActiveCalibration (uint16_t index, int32_t adc, float voltage) {
		activeCalibration_[index].set (adc, voltage);
	}

	int32_t activeCalibration_adc (uint16_t index) const {
		return activeCalibration_[index].first();
	}

	float activeCalibration_voltage (uint16_t index) const {
		return activeCalibration_[index].second();
	}

public:
	VM_Terminal terminal (void) const { return terminal_;   }
	void        setTerminal (VM_Terminal __terminal) { terminal_ = __terminal;}

private:
	VM_Range            range_;
	float               voltage_;
	VM_CalibrationTable calibration_;
	VM_CalibrationTable activeCalibration_;
	VM_Terminal         terminal_;
};
} //namespace smu

#endif
