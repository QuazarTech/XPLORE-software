#ifndef __SMU_EXCEPTION__
#define __SMU_EXCEPTION__

#include <stdexcept>

// class XSMU_Error : public std::runtime_error
// {
//
// protected:
// 	XSMU_Error ((std::string) error_msg) :
// 		runtime_error (error_msg)
// 	{}
// };


class NoOperation : public std::runtime_error
{

public:
	NoOperation (void) :
		std::runtime_error ("XSMU Error : No Operation")
	{}
};

#endif