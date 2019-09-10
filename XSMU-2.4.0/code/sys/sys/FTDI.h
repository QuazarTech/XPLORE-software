	#ifndef __SMU_FTDI__
#define __SMU_FTDI__

#include <vector>
#include <string>
#include <stdint.h>

namespace smu {

class FTDI_DeviceInfo
{
	public:
	FTDI_DeviceInfo (const char* serialNo, const char* description);

	public:
	const char* serialNo (void) const {return serialNo_.c_str();}
	const char* description (void) const {return description_.c_str();}

	private:
	std::string serialNo_;
	std::string description_;
};
} // end of namespace smu

#if defined(linux) || defined(__linux) || defined(__linux__)

#include <ftdi.h>

namespace smu {

class FTDI
{
public:
	FTDI (void);

public:
	static std::vector<FTDI_DeviceInfo> scan (void);

public:
	void open (const char* serialNo);
	void open (const char* serialNo, int baudrate);
	uint32_t read (void* data, uint32_t size);
	uint32_t write (const void* data, uint32_t size);
	void close (void);

public:
	bool good (void) const;

private:
	ftdi_context* handle_;
	static std::vector<FTDI_DeviceInfo> scan (int vid, int pid);

public:
	void setBaudRate (uint32_t bd);

private:
	void _setBaudrate (uint32_t baudrate);


};
} // end of namespace smu

#endif

#ifdef WIN32

#include <Windows.h>
#include <ftd2xx.h>

namespace smu {

class FTDI
{
	public:
	FTDI (void);

	public:
	std::vector<FTDI_DeviceInfo> scan (void);

	public:
	void open (const char* serialNo);
	void open (const char* serialNo, int baudrate);
	uint32_t read (void* data, uint32_t size);
	uint32_t write (const void* data, uint32_t size);
	void close (void);

	public:
	bool good (void) const;

	private:
	FT_HANDLE handle_;
	void configureTimeouts (void);
};
} // end of namespace smu

#endif // WIN32

#endif
