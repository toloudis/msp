//****************************************************************************
//  envNumberText.cpp
//
//      see .hpp
//
//	Extra Large Technology
//****************************************************************************
#include "Core/env/envNumberText.hpp"

#include <charconv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <type_traits>


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	template<class T>
	std::string int_to_text(T i_Value)
	{
		char buf[32];
		std::to_chars_result result = std::to_chars(buf, buf + sizeof(buf), i_Value);
		return std::string(buf, result.ptr);
	}

	//------------------------------------------------------------------------
	//	to_chars without a format gives the shortest text that reads back
	//	to the same value.
	//------------------------------------------------------------------------
	template<class T>
	std::string float_to_text(T i_Value)
	{
		// to_chars adds the payload to some NaNs ("-nan(ind)"), which the
		// text tokenizer would split at the parenthesis
		if( std::isnan(i_Value) )
			return std::signbit(i_Value) ? "-nan" : "nan";

		char buf[64];
		std::to_chars_result result = std::to_chars(buf, buf + sizeof(buf), i_Value);
		return std::string(buf, result.ptr);
	}

	//------------------------------------------------------------------------
	//	Reads through a 64-bit integer and casts, so out-of-range values
	//	wrap the way atoi and a cast did ("-1" reads as 255 for UInt8).
	//------------------------------------------------------------------------
	template<class T>
	bool int_from_text(const char* i_Text, T& o_Value)
	{
		if( i_Text == nullptr )
			return false;

		char* end = nullptr;
		T value;
		if constexpr( std::is_same_v<T, uint64_t> )
			value = static_cast<T>(std::strtoull(i_Text, &end, 10));
		else
			value = static_cast<T>(std::strtoll(i_Text, &end, 10));

		if( end == i_Text )
			return false;

		o_Value = value;
		return true;
	}

	//------------------------------------------------------------------------
	//	from_chars is locale-independent, unlike atof, but doesn't skip
	//	whitespace or a leading '+'.
	//------------------------------------------------------------------------
	template<class T>
	bool float_from_text(const char* i_Text, T& o_Value)
	{
		if( i_Text == nullptr )
			return false;

		const char* first = i_Text;
		while( std::isspace(static_cast<unsigned char>(*first)) )
			++first;
		if( (*first == '+') && (first[1] != '-') )	// "+-1" isn't a number
			++first;
		const char* last = first + std::strlen(first);

		T value;
		std::from_chars_result result = std::from_chars(first, last, value);
		if( result.ec == std::errc::invalid_argument )
			return false;

		if( result.ec == std::errc::result_out_of_range )
		{
			// from_chars leaves the value unset; take the infinity or zero
			// the C library rounds it to
			if constexpr( std::is_same_v<T, float> )
				value = std::strtof(first, nullptr);
			else
				value = std::strtod(first, nullptr);
		}

		o_Value = value;
		return true;
	}
}


//============================================================================
//============================================================================
namespace envNumberText
{
	// Int8 and UInt8 are character types; widen them so they are written
	// as numbers
	std::string ToText(int8_t i_Value)		{ return int_to_text(int(i_Value)); }
	std::string ToText(uint8_t i_Value)		{ return int_to_text(int(i_Value)); }
	std::string ToText(int16_t i_Value)		{ return int_to_text(i_Value); }
	std::string ToText(uint16_t i_Value)		{ return int_to_text(i_Value); }
	std::string ToText(int32_t i_Value)		{ return int_to_text(i_Value); }
	std::string ToText(uint32_t i_Value)		{ return int_to_text(i_Value); }
	std::string ToText(int64_t i_Value)		{ return int_to_text(i_Value); }
	std::string ToText(uint64_t i_Value)		{ return int_to_text(i_Value); }
	std::string ToText(float i_Value)	{ return float_to_text(i_Value); }
	std::string ToText(double i_Value)	{ return float_to_text(i_Value); }

	bool FromText(const char* i_Text, int8_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, uint8_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, int16_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, uint16_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, int32_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, uint32_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, int64_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, uint64_t& o_Value)		{ return int_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, float& o_Value)	{ return float_from_text(i_Text, o_Value); }
	bool FromText(const char* i_Text, double& o_Value)	{ return float_from_text(i_Text, o_Value); }
}
