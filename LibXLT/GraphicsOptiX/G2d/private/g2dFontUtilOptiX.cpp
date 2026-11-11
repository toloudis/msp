/*****************************************************************************
**  g2dFontUtilOptiX.cpp
**
**      See g2dFontUtilOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G2DFONTUTILOPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dFontUtilOptiX.hpp"
#endif

namespace gox
{
	g2dFontHandle g2dFontUtilOptiX::LoadFont(const std::string& /*i_Name*/, int /*i_Size*/)
	{
		return NULL;
	}

	void g2dFontUtilOptiX::ReleaseFont(g2dFontHandle /*i_Handle*/)
	{
	}

	void g2dFontUtilOptiX::GetLoadedFontName(g2dFontHandle /*i_Font*/, std::string& o_Name)
	{
		o_Name.clear();
	}

	void g2dFontUtilOptiX::GetTextSizeInPixels(g2dFontHandle /*i_Font*/, const std::string& /*i_Text*/, int& o_Width, int& o_Height)
	{
		o_Width = 0;
		o_Height = 0;
	}

	void g2dFontUtilOptiX::SetGlobalFontBitmap(const fsLocator& i_FontPath)
	{
		m_FontBitmap = i_FontPath;
	}

	void g2dFontUtilOptiX::GetGlobalFontBitmap(fsLocator& o_FontPath)
	{
		o_FontPath = m_FontBitmap;
	}
}
