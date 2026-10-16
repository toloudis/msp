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

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
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
	std::string ToText(envType::Int8 i_Value);
	std::string ToText(envType::UInt8 i_Value);
	std::string ToText(envType::Int16 i_Value);
	std::string ToText(envType::UInt16 i_Value);
	std::string ToText(envType::Int32 i_Value);
	std::string ToText(envType::UInt32 i_Value);
	std::string ToText(envType::Int64 i_Value);
	std::string ToText(envType::UInt64 i_Value);
	std::string ToText(envType::Float32 i_Value);
	std::string ToText(envType::Float64 i_Value);

	//------------------------------------------------------------------------
	//	FromText reads the number at the start of i_Text, after any
	//	whitespace and an optional sign, and ignores what follows it
	//	(as atoi and atof do). Integers out of the type's range wrap,
	//	as they did when read through atoi and a cast.
	//
	// \return false, leaving o_Value unchanged, if i_Text does not start
	//	with a number
	//------------------------------------------------------------------------
	bool FromText(const char* i_Text, envType::Int8& o_Value);
	bool FromText(const char* i_Text, envType::UInt8& o_Value);
	bool FromText(const char* i_Text, envType::Int16& o_Value);
	bool FromText(const char* i_Text, envType::UInt16& o_Value);
	bool FromText(const char* i_Text, envType::Int32& o_Value);
	bool FromText(const char* i_Text, envType::UInt32& o_Value);
	bool FromText(const char* i_Text, envType::Int64& o_Value);
	bool FromText(const char* i_Text, envType::UInt64& o_Value);
	bool FromText(const char* i_Text, envType::Float32& o_Value);
	bool FromText(const char* i_Text, envType::Float64& o_Value);

	template<class T>
	inline bool FromText(const std::string& i_Text, T& o_Value)
	{
		return FromText(i_Text.c_str(), o_Value);
	}
}
