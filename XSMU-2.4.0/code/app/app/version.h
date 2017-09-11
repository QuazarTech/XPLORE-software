#ifndef __XSMU_VERSION_INFO__
#define __XSMU_VERSION_INFO__

#include <stdint.h>

namespace smu {

inline uint32_t MAKE_VERSION_NO(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x << 16) | (y << 8) | z);
};

inline uint16_t MAJOR_VERSION_NO(uint32_t x) 
{
	return ((x >> 16) & 0xFF);
};

inline uint16_t MINOR_VERSION_NO(uint32_t x) 
{
	return ((x >> 8) & 0xFF);
};

inline uint16_t BUGFIX_VERSION_NO(uint32_t x) 
{
	return ((x >> 0) & 0xFF);
};

const uint32_t LIBXSMU_VERSION = MAKE_VERSION_NO (2, 1, 2);

class VersionInfo
{
public:
	VersionInfo (void);

public:
	uint32_t hardware_version (void) const { return hardware_version_; }
	uint32_t firmware_version (void) const { return firmware_version_; }
	uint32_t libxsmu_version  (void) const { return libxsmu_version_;  }

public:
	void hardware_version (uint32_t ver) { hardware_version_ = ver; }
	void firmware_version (uint32_t ver) { firmware_version_ = ver; }
	void libxsmu_version  (uint32_t ver) { libxsmu_version_  = ver; }

public:
	void clear (void);

private:
	uint32_t hardware_version_;
	uint32_t firmware_version_;
	uint32_t libxsmu_version_;
};
} //namespace smu 

#endif
