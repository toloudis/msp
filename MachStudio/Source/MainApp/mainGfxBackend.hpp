/****************************************************************************\
**	mainGfxBackend.hpp
**
**		The startup line "Graphics backend: <code name> on <adapter>", which
**	mainInitGraphics logs (and RenderNode also prints to stderr, where
**	gfxcompare reads it). The code name ("dx11", "vulkan", "optix") comes
**	from the graphics layer itself (GraphicsXxxLayer::GetCodeName), as does
**	its display name (GetName); mainInitGraphics chooses the layer.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef MAIN_GFXBACKEND_HPP
#error mainGfxBackend.hpp multiply included
#endif
#define MAIN_GFXBACKEND_HPP

#include <string>

namespace mainGfxBackend
{
	inline std::string BackendLine(const char* i_pCodeName, const char* i_pAdapter)
	{
		return std::string("Graphics backend: ") + (i_pCodeName && *i_pCodeName ? i_pCodeName : "unknown") + " on " +
			   (i_pAdapter && *i_pAdapter ? i_pAdapter : "unknown adapter");
	}
}
