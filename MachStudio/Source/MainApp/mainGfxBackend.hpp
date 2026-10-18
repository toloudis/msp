/****************************************************************************\
**	mainGfxBackend.hpp
**
**		How each graphics backend (-gfx dx11|vulkan) presents itself:
**	the name used in the profiler CSV and in the startup line
**	"Graphics backend: <name> on <adapter>" (which gfxcompare reads from
**	RenderNode's stderr), and the main window title suffix, which is empty
**	for DX11 so a DX11 run looks exactly as before. mainInitGraphics
**	chooses the layer to initialize.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef MAIN_GFXBACKEND_HPP
#error mainGfxBackend.hpp multiply included
#endif
#define MAIN_GFXBACKEND_HPP

#ifndef G3D_GFXOPTIONS_HPP
#include "Graphics/G3d/g3dGfxOptions.hpp"
#endif

#include <string>

namespace mainGfxBackend
{
	inline const char* Name(g3dGfxOptions::Backend i_Backend)
	{
		return i_Backend == g3dGfxOptions::e_Vulkan ? "vulkan" : "dx11";
	}

	inline const wchar_t* TitleSuffix(g3dGfxOptions::Backend i_Backend)
	{
		return i_Backend == g3dGfxOptions::e_Vulkan ? L" [Vulkan]" : L"";
	}

	inline std::string BackendLine(g3dGfxOptions::Backend i_Backend, const char* i_pAdapter)
	{
		return std::string("Graphics backend: ") + Name(i_Backend) + " on " +
			   (i_pAdapter && *i_pAdapter ? i_pAdapter : "unknown adapter");
	}
}
