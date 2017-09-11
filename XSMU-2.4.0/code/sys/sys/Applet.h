#ifndef __SMU_APPLET__
#define __SMU_APPLET__

#include <stdint.h>

namespace smu {

typedef void (*AppletCallback)(void* user_data, const void* oCB);

class Applet
{
	public:
	Applet (void);

	public:
	void do_callback (const void* oCB);
	void callback (AppletCallback cb, void* user_data);

	private:
	void* user_data_;
	AppletCallback cb_;
	static void default_callback (void*, const void*) {}
};
} // end of namespace smu
#endif
