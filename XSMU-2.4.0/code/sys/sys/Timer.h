#ifndef __SMU_TIMER__
#define __SMU_TIMER__

namespace smu {

class Timer
{
public:
	static double get (void);
	static void sleep (double duration);
};
} // end of namespace smu

#endif
