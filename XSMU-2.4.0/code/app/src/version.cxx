#include "../app/version.h"

namespace smu {
VersionInfo::VersionInfo (void) :
	hardware_version_ (0),
	firmware_version_ (0),
	libxsmu_version_ (LIBXSMU_VERSION)
{}

void VersionInfo::clear (void)
{
	hardware_version_ = 0;
	firmware_version_ = 0;
}

} // namespace smu
