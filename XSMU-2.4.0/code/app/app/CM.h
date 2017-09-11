#ifndef __SMU_CM__
#define __SMU_CM__

#include <stdint.h>
#include "Calibration.h"

namespace smu {

enum CM_Range
{
	CM_RANGE_10uA,
	CM_RANGE_100uA,
	CM_RANGE_1mA,
	CM_RANGE_10mA,
	CM_RANGE_100mA,
};

CM_Range toCM_Range (uint16_t i);

class CM
{
public:
	CM (void);

public:
	CM_Range range   (void) const { return range_;   }
	float    current (void) const { return current_; }

public:
	void setRange   (CM_Range range) { range_ = range;     }
	void setCurrent (float current)  { current_ = current; }

public:
	void setCalibration (uint16_t index, int32_t adc, float current) {
		calibration_[index].set (adc, current);
	}

	int32_t calibration_adc (uint16_t index) const {
		return calibration_[index].first();
	}

	float calibration_current (uint16_t index) const {
		return calibration_[index].second();
	}

private:
	CM_Range            range_;
	float               current_;
	CM_CalibrationTable calibration_;
};
} //namespace smu 

#endif
