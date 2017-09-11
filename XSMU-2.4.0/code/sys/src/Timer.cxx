#include "../sys/Timer.h"

#ifdef WIN32
#include <Windows.h>

namespace smu {
	
double Timer::get (void)
{
	FILETIME fileTime;
	GetSystemTimeAsFileTime (&fileTime);

	ULARGE_INTEGER iTime;
	iTime.LowPart = fileTime.dwLowDateTime;
	iTime.HighPart = fileTime.dwHighDateTime;

	return iTime.QuadPart * 100e-9 - 11644473600;
	// Windows keeps time since 1 Jan, 1601.
	// Linux keeps it from 1 Jan, 1970.
	// 11644473600 seconds are there between these two dates.
}

void Timer::sleep (double duration)
{
	Sleep ((DWORD)(duration * 1e3));
}
}
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#include <time.h>
#include <sys/time.h>

namespace smu {
double Timer::get (void)
{
	struct timeval tval;
	gettimeofday (&tval, 0);
	return tval.tv_sec + 1e-6 * tval.tv_usec;
}

void Timer::sleep (double duration)
{
	const time_t sec  = (time_t)duration;
	const long   nsec = (duration - sec) * 1e9;

	struct timespec req = {sec, nsec};
	nanosleep (&req, 0);
}
} // end of namespace smu

#endif
