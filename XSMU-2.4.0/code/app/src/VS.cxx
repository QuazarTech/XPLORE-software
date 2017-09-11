#include "../app/VS.h"

namespace smu {

VS_Range toVS_Range (uint16_t i)
{
	static const VS_Range ranges[] =
	{
		VS_RANGE_1mV,
		VS_RANGE_10mV,
		VS_RANGE_100mV,
		VS_RANGE_1V,
		VS_RANGE_10V,
		VS_RANGE_100V
	};

	return (i < sizeof (ranges) / sizeof (ranges[0])) ?
		ranges[i] : ranges[0];
}

VS::VS (void) :
	voltage_ (0.0),
	range_ (VS_RANGE_10V),
	active_ (false)
{}

} // namespace smu
