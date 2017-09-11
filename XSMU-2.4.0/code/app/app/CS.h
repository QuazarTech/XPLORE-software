#ifndef __SMU_CS__
#define __SMU_CS__

#include <stdint.h>
#include "Calibration.h"

namespace smu {

enum CS_Range
{
	CS_RANGE_10uA,
	CS_RANGE_100uA,
	CS_RANGE_1mA,
	CS_RANGE_10mA,
	CS_RANGE_100mA,
};

CS_Range toCS_Range (uint16_t i);

class CS
{
public:
	CS (void);

public:
	CS_Range range   (void) const { return range_;   }
	float    current (void) const { return current_; }

public:
	void activate   (void)           { active_ = true;     }
	void deactivate (void)           { active_ = false;    }
	void setRange   (CS_Range range) { range_ = range;     }
	void setCurrent (float current)  { current_ = current; }
	bool active     (void) const     { return active_;     }

public:
	void setCalibration (uint16_t index, int16_t dac, float current) {
		calibration_[index].set (dac, current);
	}

	int16_t calibration_dac (uint16_t index) const {
		return calibration_[index].first();
	}

	float calibration_current (uint16_t index) const {
		return calibration_[index].second();
	}

private:
	float               current_;
	CS_Range            range_;
	bool                active_;
	CS_CalibrationTable calibration_;
};
} //namespace smu 

#endif
