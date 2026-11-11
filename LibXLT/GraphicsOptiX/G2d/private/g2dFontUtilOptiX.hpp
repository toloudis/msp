/*****************************************************************************
**  g2dFontUtilOptiX.hpp
**
**      g2dFontUtilOptiX is the OptiX backend's font support (g2dFontUtil).
**
**	Stub: no fonts load (handles are NULL, text measures 0x0); the global
**	font bitmap's path is kept.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DFONTUTILOPTIX_HPP
#error g2dFontUtilOptiX.hpp multiply included
#endif
#define GOX_G2DFONTUTILOPTIX_HPP

#ifndef G2D_FONTUTIL_HPP
#include "Graphics/G2d/g2dFontUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <string>

namespace gox
{
	class g2dFontUtilOptiX : public g2dFontUtilImpl
	{
		public:
			virtual g2dFontHandle LoadFont(const std::string& i_Name, int i_Size);
			virtual void ReleaseFont(g2dFontHandle i_Handle);
			virtual void GetLoadedFontName(g2dFontHandle i_Font, std::string& o_Name);
			virtual void GetTextSizeInPixels(g2dFontHandle i_Font, const std::string& i_Text, int& o_Width, int& o_Height);
			virtual void SetGlobalFontBitmap(const fsLocator& i_FontPath);
			virtual void GetGlobalFontBitmap(fsLocator& o_FontPath);

		private:
			fsLocator	m_FontBitmap;
	};
}
