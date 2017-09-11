#ifndef __SMU_CALIBRATION__
#define __SMU_CALIBRATION__

#include <stdint.h>

namespace smu {
	
template<typename T, typename U>
class CalibrationTableEntry
{
public:
	CalibrationTableEntry (void) :
		first_ (0), second_ (0)
			{}

public:
	void set (const T& first, const U& second) {
		first_ = first;
		second_ = second;
	}

public:
	const T& first  (void) const {return first_; }
	const U& second (void) const {return second_;}

private:
	T first_;
	U second_;
};

template <typename T, typename U, uint8_t N>
class CalibrationTable
{
public:
	CalibrationTableEntry<T, U>& operator[] (uint_fast8_t i) {
		return entries_[i];
	}

public:
	const CalibrationTableEntry<T, U>& operator[] (uint_fast8_t i) const {
		return entries_[i];
	}

public:
	uint8_t size (void) const {return N;}

private:
	CalibrationTableEntry<T, U> entries_[N];
};

/***********************************************************************/
/***********************************************************************/

enum CalibrationTableSize
{
	CS_CALIBRATION_TABLE_SIZE = 5,
	VS_CALIBRATION_TABLE_SIZE = 5,
	CM_CALIBRATION_TABLE_SIZE = 5,
	VM_CALIBRATION_TABLE_SIZE = 5
};

/***********************************************************************/

class CS_CalibrationTable :
	public CalibrationTable<int16_t, float, CS_CALIBRATION_TABLE_SIZE>
{};

class VS_CalibrationTable :
	public CalibrationTable<int16_t, float, VS_CALIBRATION_TABLE_SIZE>
{};

/***********************************************************************/

class CM_CalibrationTable :
	public CalibrationTable<int32_t, float, CM_CALIBRATION_TABLE_SIZE>
{};

class VM_CalibrationTable :
	public CalibrationTable<int32_t, float, VM_CALIBRATION_TABLE_SIZE>
{};

} //namespace smu 

#endif
