/****************************************************************************\
**  envNumberText.hpp
**
**	envNumberText converts numbers to and from text, for the text and XML
**	file readers and writers. Every type reads back exactly what was
**	written:
**	- Int8 and UInt8 are written as numbers, not as characters (they are
**	  character types, so streaming them writes the raw byte).
**	- Floats are written with the fewest digits that read back to the
**	  same value ("0.1", not "0.100000001"; not the 6 significant digits
**	  an ostream uses by default).
**	- Integers use the full range of their type (atoi stops at 2^31).
**
**	The text is locale-independent.
**
**	Extra Large Technology
\****************************************************************************/

#ifdef ENV_NUMBERTEXT_HPP
#error envNumberText.hpp multiply included
#endif
#define ENV_NUMBERTEXT_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#include <string>


//============================================================================
//	envNumberText
//============================================================================
namespace envNumberText
{
	//------------------------------------------------------------------------
	//	ToText returns the decimal text for i_Value. Floats use plain or
	//	scientific notation, whichever is shorter ("1e-05"); infinities
	//	are "inf" and "-inf", and NaNs are "nan" or "-nan".
	//------------------------------------------------------------------------
	std::string ToText(int8_t i_Value);
	std::string ToText(uint8_t i_Value);
	std::string ToText(int16_t i_Value);
	std::string ToText(uint16_t i_Value);
	std::string ToText(int32_t i_Value);
	std::string ToText(uint32_t i_Value);
	std::string ToText(int64_t i_Value);
	std::string ToText(uint64_t i_Value);
	std::string ToText(float i_Value);
	std::string ToText(double i_Value);

	//------------------------------------------------------------------------
	//	FromText reads the number at the start of i_Text, after any
	//	whitespace and an optional sign, and ignores what follows it
	//	(as atoi and atof do). Integers out of the type's range wrap,
	//	as they did when read through atoi and a cast.
	//
	// \return false, leaving o_Value unchanged, if i_Text does not start
	//	with a number
	//------------------------------------------------------------------------
	bool FromText(const char* i_Text, int8_t& o_Value);
	bool FromText(const char* i_Text, uint8_t& o_Value);
	bool FromText(const char* i_Text, int16_t& o_Value);
	bool FromText(const char* i_Text, uint16_t& o_Value);
	bool FromText(const char* i_Text, int32_t& o_Value);
	bool FromText(const char* i_Text, uint32_t& o_Value);
	bool FromText(const char* i_Text, int64_t& o_Value);
	bool FromText(const char* i_Text, uint64_t& o_Value);
	bool FromText(const char* i_Text, float& o_Value);
	bool FromText(const char* i_Text, double& o_Value);

	template<class T>
	inline bool FromText(const std::string& i_Text, T& o_Value)
	{
		return FromText(i_Text.c_str(), o_Value);
	}
}
