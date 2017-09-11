#include "../app/VM2.h"

namespace smu {

VM2_Range toVM2_Range (uint16_t i)
{
	static const VM2_Range ranges[] =
	{
		VM2_RANGE_10V
	};

	return (i < sizeof (ranges) / sizeof (ranges[0])) ?
		ranges[i] : ranges[0];
}

VM2::VM2 (void) :
	range_   (VM2_RANGE_10V),
	voltage_ (0)
{}

} // namespace smu
