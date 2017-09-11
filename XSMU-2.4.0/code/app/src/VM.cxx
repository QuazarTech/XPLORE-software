#include "../app/VM.h"

namespace smu {

VM_Range toVM_Range (uint16_t i)
{
	static const VM_Range ranges[] =
	{
		VM_RANGE_1mV,
		VM_RANGE_10mV,
		VM_RANGE_100mV,
		VM_RANGE_1V,
		VM_RANGE_10V,
		VM_RANGE_100V
	};

	return (i < sizeof (ranges) / sizeof (ranges[0])) ?
		ranges[i] : ranges[0];
}

VM_Terminal toVM_Terminal (uint16_t i)
{
	static const VM_Terminal terminals[] =
	{
		VM_TERMINAL_MEASUREMENT,
		VM_TERMINAL_SOURCE,
	};

	return (i < sizeof (terminals) / sizeof (terminals[0])) ?
		terminals[i] : terminals[0];
}

VM::VM (void) :
	range_   (VM_RANGE_10V),
	voltage_ (0)
{}

} // namespace smu
