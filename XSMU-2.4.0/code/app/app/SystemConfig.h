#ifndef __SMU_SYSTEM_CONFIG__
#define __SMU_SYSTEM_CONFIG__

#include <stdint.h>

namespace smu {
enum SystemConfig_ParamID
{
	SYS_CONF_PARAM_ID_HW_BOARD_NO,
	SYS_CONF_PARAM_ID_HW_BOM_NO,
	SYS_CONF_PARAM_ID_HW_BUGFIX_NO
};

class SystemConfig
{
public:
	SystemConfig (void);

public:
	void clear (void);

public:
	int16_t get (int paramID) const { return values_[paramID]; }
	void set (int paramID, int16_t value) { values_[paramID] = value; }

public:
	int16_t hwBoardNo (void) const {
		return get (SYS_CONF_PARAM_ID_HW_BOARD_NO);
	}

	void hwBoardNo (int16_t ver) {
		set (SYS_CONF_PARAM_ID_HW_BOARD_NO, ver);
	}

public:
	int16_t hwBomNo (void) const  {
		return get (SYS_CONF_PARAM_ID_HW_BOM_NO);
	}

	void hwBomNo (int16_t ver) {
		set (SYS_CONF_PARAM_ID_HW_BOM_NO, ver);
	}

public:
	int16_t hwBugfixNo (void) const {
		return get (SYS_CONF_PARAM_ID_HW_BUGFIX_NO);
	}

	void hwBugfixNo (int16_t ver) {
		set (SYS_CONF_PARAM_ID_HW_BUGFIX_NO, ver);
	}

private:
	int16_t values_[16];
};
} //namespace smu 

#endif
