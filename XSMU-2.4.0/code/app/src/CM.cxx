#include "../app/CM.h"

namespace smu {

CM_Range toCM_Range (uint16_t i)
{
	static const CM_Range ranges[] =
	{
		CM_RANGE_10uA,
		CM_RANGE_100uA,
		CM_RANGE_1mA,
		CM_RANGE_10mA,
		CM_RANGE_100mA,
	};

	return (i < sizeof (ranges) / sizeof (ranges[0])) ?
		ranges[i] : ranges[0];
}

CM::CM (void) :
	range_   (CM_RANGE_10uA),
	current_ (0)
{}

} // namespace smu
