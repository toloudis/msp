/****************************************************************************\
**  g2dARGBColor.hpp
**
**      g2dARGBColor.hpp is a simple color class designed for interfacing to
**	g2d functions.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_ARGBCOLOR_HPP
#error g2dARGBColor.hpp multiply included
#endif
#define G2D_ARGBCOLOR_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif


//============================================================================
//============================================================================
class g2dARGBColor
{
	public:
		typedef envType::UInt8 Value;

		//------------------------------------------------------------------------
		//	This constructor does no initialization
		//------------------------------------------------------------------------
		g2dARGBColor();

		//------------------------------------------------------------------------
		//	This constructor initializes to the given values
		//------------------------------------------------------------------------
		g2dARGBColor(Value i_Red, Value i_Green, Value i_Blue, Value i_Alpha);
		g2dARGBColor(Value i_Red, Value i_Green, Value i_Blue);

		//------------------------------------------------------------------------
		//	Set sets each color value
		//------------------------------------------------------------------------
		void Set(Value i_Red, Value i_Green, Value i_Blue, Value i_Alpha);
		void Set(Value i_Red, Value i_Green, Value i_Blue);

		//------------------------------------------------------------------------
		//	These get functions return each color value
		//------------------------------------------------------------------------
		Value GetRed() const;
		Value GetGreen() const;
		Value GetBlue() const;
		Value GetAlpha() const;

		//------------------------------------------------------------------------
		//	These set functions set each color value individually.
		//------------------------------------------------------------------------
		void SetRed(Value i_Red);
		void SetGreen(Value i_Green);
		void SetBlue(Value i_Blue);
		void SetAlpha(Value i_Alpha);

	private:

		Value m_Red, m_Green, m_Blue, m_Alpha;
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
inline g2dARGBColor::g2dARGBColor()
{
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
inline g2dARGBColor::g2dARGBColor(Value i_Red, Value i_Green, Value i_Blue, Value i_Alpha)
:	m_Red(i_Red),
	m_Green(i_Green),
	m_Blue(i_Blue),
	m_Alpha(i_Alpha)
{
}
inline g2dARGBColor::g2dARGBColor(Value i_Red, Value i_Green, Value i_Blue)
:	m_Red(i_Red),
	m_Green(i_Green),
	m_Blue(i_Blue),
	m_Alpha(255)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline void g2dARGBColor::Set(Value i_Red, Value i_Green, Value i_Blue, Value i_Alpha)
{
	m_Red = i_Red;
	m_Green = i_Green;
	m_Blue = i_Blue;
	m_Alpha = i_Alpha;
}
inline void g2dARGBColor::Set(Value i_Red, Value i_Green, Value i_Blue)
{
	m_Red = i_Red;
	m_Green = i_Green;
	m_Blue = i_Blue;
	m_Alpha = 255;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline g2dARGBColor::Value g2dARGBColor::GetRed() const
{
	return m_Red;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline g2dARGBColor::Value g2dARGBColor::GetGreen() const
{
	return m_Green;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline g2dARGBColor::Value g2dARGBColor::GetBlue() const
{
	return m_Blue;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline g2dARGBColor::Value g2dARGBColor::GetAlpha() const
{
	return m_Alpha;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline void g2dARGBColor::SetRed(Value i_Red)
{
	m_Red = i_Red;
}

inline void g2dARGBColor::SetGreen(Value i_Green)
{
	m_Green = i_Green;
}

inline void g2dARGBColor::SetBlue(Value i_Blue)
{
	m_Blue = i_Blue;
}

inline void g2dARGBColor::SetAlpha(Value i_Alpha)
{
	m_Alpha = i_Alpha;
}

