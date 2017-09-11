#include "../app/CS.h"

namespace smu {

CS_Range toCS_Range (uint16_t i)
{
	static const CS_Range ranges[] =
	{
		CS_RANGE_10uA,
		CS_RANGE_100uA,
		CS_RANGE_1mA,
		CS_RANGE_10mA,
		CS_RANGE_100mA,
	};

	return (i < sizeof (ranges) / sizeof (ranges[0])) ?
		ranges[i] : ranges[0];
}

CS::CS (void) :
	current_ (0.0),
	range_ (CS_RANGE_10uA),
	active_ (false)
{}

} // namespace smu
