#ifndef __SMU_VM2__
#define __SMU_VM2__

#include <stdint.h>
#include "Calibration.h"

namespace smu {

enum VM2_Range
{
	VM2_RANGE_10V
};

VM2_Range toVM2_Range (uint16_t i);

class VM2
{
public:
	VM2 (void);

public:
	VM2_Range range   (void) const { return range_;   }
	float    voltage  (void) const { return voltage_; }

public:
	void setRange   (VM2_Range range) { range_ = range;     }
	void setVoltage (float voltage)   { voltage_ = voltage; }

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

private:
	VM2_Range range_;
	float voltage_;
	VM_CalibrationTable    calibration_;
};
} //namespace smu 

#endif
