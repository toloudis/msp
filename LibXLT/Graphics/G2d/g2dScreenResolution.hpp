/****************************************************************************\
**  g2dScreenResolution.hpp
**
**      g2dScreenResolution is an internal definition for a structure
**	representing a screen resolution.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_SCREENRESOLUTION_HPP
#error g2dScreenResolution.hpp multiply included
#endif
#define G2D_SCREENRESOLUTION_HPP


//============================================================================
//============================================================================
struct g2dScreenResolution
{
	bool operator == (const g2dScreenResolution& i_Resolution) const;

	int m_Width;
	int m_Height;
	int m_BitDepth;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g2dScreenResolution::operator == (const g2dScreenResolution& i_Resolution) const
{
	return	(m_Width == i_Resolution.m_Width) &&
			(m_Height == i_Resolution.m_Height) &&
			(m_BitDepth == i_Resolution.m_BitDepth);
}
