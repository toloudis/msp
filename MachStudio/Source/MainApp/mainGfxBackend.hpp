/****************************************************************************\
**	mainGfxBackend.hpp
**
**		How each graphics backend (-gfx dx11|vulkan) is named: in the
**	profiler CSV and in the startup line "Graphics backend: <name> on
**	<adapter>", which mainInitGraphics logs (and RenderNode also prints
**	to stderr, where gfxcompare reads it). mainInitGraphics chooses the
**	layer to initialize.
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

	inline std::string BackendLine(g3dGfxOptions::Backend i_Backend, const char* i_pAdapter)
	{
		return std::string("Graphics backend: ") + Name(i_Backend) + " on " +
			   (i_pAdapter && *i_pAdapter ? i_pAdapter : "unknown adapter");
	}
}
