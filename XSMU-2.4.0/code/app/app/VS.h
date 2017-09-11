#ifndef __SMU_VS__
#define __SMU_VS__

#include <stdint.h>
#include "Calibration.h"

namespace smu {

enum VS_Range
{
	VS_RANGE_1mV,
	VS_RANGE_10mV,
	VS_RANGE_100mV,
	VS_RANGE_1V,
	VS_RANGE_10V,
	VS_RANGE_100V
};

VS_Range toVS_Range (uint16_t i);

class VS
{
public:
	VS (void);

public:
	VS_Range range   (void) const { return range_;   }
	float    voltage (void) const { return voltage_; }

public:
	void activate   (void)           { active_ = true;     }
	void deactivate (void)           { active_ = false;    }
	void setRange   (VS_Range range) { range_ = range;     }
	void setVoltage (float voltage)  { voltage_ = voltage; }
	bool active     (void) const     { return active_;     }

public:
	void setCalibration (uint16_t index, int16_t dac, float voltage) {
		calibration_[index].set (dac, voltage);
	}

	int16_t calibration_dac (uint16_t index) const {
		return calibration_[index].first();
	}

	float calibration_voltage (uint16_t index) const {
		return calibration_[index].second();
	}

private:
	float               voltage_;
	VS_Range            range_;
	bool                active_;
	VS_CalibrationTable calibration_;
};
}

#endif
