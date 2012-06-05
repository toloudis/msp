/****************************************************************************\
**  g2dRGBColor.hpp
**
**      g2dRGBColor.hpp is a simple color class designed for interfacing to
**	g2d functions.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_RGBCOLOR_HPP
#error g2dRGBColor.hpp multiply included
#endif
#define G2D_RGBCOLOR_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif


//============================================================================
//============================================================================
class g2dRGBColor
{
	public:
		typedef envType::UInt8 Value;

		//------------------------------------------------------------------------
		//	This constructor does no initialization
		//------------------------------------------------------------------------
		inline g2dRGBColor();

		//------------------------------------------------------------------------
		//	This constructor initializes to the given values
		//------------------------------------------------------------------------
		inline g2dRGBColor(Value i_Red, Value i_Green, Value i_Blue);

		//------------------------------------------------------------------------
		//	Copy constructor
		//------------------------------------------------------------------------
		inline g2dRGBColor(const g2dRGBColor &i_CopyFrom);

		//------------------------------------------------------------------------
		//	Set sets each color value
		//------------------------------------------------------------------------
		inline void Set(Value i_Red, Value i_Green, Value i_Blue);

		//------------------------------------------------------------------------
		//	These get functions return each color value
		//------------------------------------------------------------------------
		Value GetRed() const;
		Value GetGreen() const;
		Value GetBlue() const;

		//------------------------------------------------------------------------
		//	These set functions set each color value individually.
		//------------------------------------------------------------------------
		void SetRed(Value i_Red);
		void SetGreen(Value i_Green);
		void SetBlue(Value i_Blue);

		//------------------------------------------------------------------------
		//	Assignment
		//------------------------------------------------------------------------
		inline const g2dRGBColor& operator = (const g2dRGBColor& i_CopyFrom);

		//------------------------------------------------------------------------
		//	Equality
		//------------------------------------------------------------------------
		inline bool operator == (const g2dRGBColor& i_Color) const;

		//------------------------------------------------------------------------
		//	Inequality
		//------------------------------------------------------------------------
		inline bool operator != (const g2dRGBColor& i_Color) const;

		//------------------------------------------------------------------------
		//	Addition
		//------------------------------------------------------------------------
		inline g2dRGBColor operator + (const g2dRGBColor& i_Color) const;

		//------------------------------------------------------------------------
		//	Subtraction
		//------------------------------------------------------------------------
		inline g2dRGBColor operator - (const g2dRGBColor& i_Color) const;

	private:

		Value m_Red, m_Green, m_Blue;
};


//--------------------------------------------------------------------------------
//	This constructor does no initialization
//--------------------------------------------------------------------------------
inline g2dRGBColor::g2dRGBColor()
{
}

//--------------------------------------------------------------------------------
//	This constructor initializes to the given values
//--------------------------------------------------------------------------------
inline g2dRGBColor::g2dRGBColor(Value i_Red, Value i_Green, Value i_Blue)
:	m_Red(i_Red),
	m_Blue(i_Blue),
	m_Green(i_Green)
{
}

//--------------------------------------------------------------------------------
//	Copy constructor
//--------------------------------------------------------------------------------
inline g2dRGBColor::g2dRGBColor(const g2dRGBColor &i_CopyFrom)
	: m_Red(i_CopyFrom.m_Red),
	  m_Blue(i_CopyFrom.m_Blue),
	  m_Green(i_CopyFrom.m_Green)
{
}

//------------------------------------------------------------------------
//	Set sets each color value
//------------------------------------------------------------------------
inline void g2dRGBColor::Set(Value i_Red, Value i_Green, Value i_Blue)
{
	m_Red = i_Red;
	m_Blue = i_Blue;
	m_Green = i_Green;
}

//------------------------------------------------------------------------
//	These get functions return each color value
//------------------------------------------------------------------------
inline g2dRGBColor::Value g2dRGBColor::GetRed() const
{
	return m_Red;
}

inline g2dRGBColor::Value g2dRGBColor::GetGreen() const
{
	return m_Green;
}

inline g2dRGBColor::Value g2dRGBColor::GetBlue() const
{
	return m_Blue;
}

//------------------------------------------------------------------------
//	These set functions set each color value individually.
//------------------------------------------------------------------------
inline void g2dRGBColor::SetRed(Value i_Red)
{
	m_Red = i_Red;
}

inline void g2dRGBColor::SetGreen(Value i_Green)
{
	m_Green = i_Green;
}

inline void g2dRGBColor::SetBlue(Value i_Blue)
{
	m_Blue = i_Blue;
}

//------------------------------------------------------------------------
//	Assignment
//------------------------------------------------------------------------
inline const g2dRGBColor& g2dRGBColor::operator = (const g2dRGBColor& i_CopyFrom)
{
	// Check for self-assignment
	if (&i_CopyFrom != this)
	{
		m_Red = i_CopyFrom.m_Red;
		m_Green = i_CopyFrom.m_Green;
		m_Blue = i_CopyFrom.m_Blue;
	}

	return *this;
}

//------------------------------------------------------------------------
//	Equality
//------------------------------------------------------------------------
inline bool g2dRGBColor::operator==(const g2dRGBColor& i_Color) const
{
	return (m_Red == i_Color.GetRed() && m_Green == i_Color.GetGreen() && m_Blue == i_Color.GetBlue());
}

//------------------------------------------------------------------------
//	Inequality
//------------------------------------------------------------------------
inline bool g2dRGBColor::operator != (const g2dRGBColor& i_Color) const
{
	return !(*this == i_Color);
}


//------------------------------------------------------------------------
//	Addition
//------------------------------------------------------------------------
inline g2dRGBColor g2dRGBColor::operator + (const g2dRGBColor& i_Color) const
{
	return g2dRGBColor(m_Red + i_Color.m_Red,
					   m_Green + i_Color.m_Green,
					   m_Blue + i_Color.m_Blue);
}

//------------------------------------------------------------------------
//	Subtraction
//------------------------------------------------------------------------
inline g2dRGBColor g2dRGBColor::operator - (const g2dRGBColor& i_Color) const
{
	return g2dRGBColor(m_Red - i_Color.m_Red,
					   m_Green - i_Color.m_Green,
					   m_Blue - i_Color.m_Blue);
}

