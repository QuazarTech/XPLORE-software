#include "../sys/Applet.h"

using namespace std;

namespace smu {
	
Applet::Applet (void) :
    user_data_ (0),
    cb_ (default_callback)
{}

void Applet::callback (AppletCallback cb, void* user_data)
{
	cb_ = cb;
	user_data_ = user_data;
}

void Applet::do_callback (const void* oCB)
{
	(*cb_)(user_data_, oCB);
}

} // end of namespace smu
