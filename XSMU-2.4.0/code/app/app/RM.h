#ifndef __SMU_RM__
#define __SMU_RM__

namespace smu {

class RM
{
public:
	void setResistance (float resistance) { resistance_ = resistance; }
	float resistance (void) const         { return resistance_; }

private:
	float resistance_;
};
} //namespace smu 

#endif
