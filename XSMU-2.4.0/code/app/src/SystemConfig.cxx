#include "../app/SystemConfig.h"
#include <algorithm>

namespace smu {
SystemConfig::SystemConfig (void)
{
	std::fill (values_, values_ + sizeof (values_) / sizeof (values_[0]), 0);
}

void SystemConfig::clear (void)
{
	std::fill (values_, values_ + sizeof (values_) / sizeof (values_[0]), 0);
}

} // namespace smu
