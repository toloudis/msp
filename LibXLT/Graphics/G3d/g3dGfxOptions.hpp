/****************************************************************************\
**  g3dGfxOptions.hpp
**
**      g3dGfxOptions holds the graphics start-up options an application
**	reads from its command line before graphics are initialized.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef G3D_GFXOPTIONS_HPP
#error g3dGfxOptions.hpp multiply included
#endif
#define G3D_GFXOPTIONS_HPP

#include <string>

//============================================================================
//============================================================================
struct g3dGfxOptions
{
	enum Backend { e_DX11, e_Vulkan };

	Backend		m_Backend = e_Vulkan;	// -gfx dx11|vulkan
	bool		m_bValidation = false;	// -gfxValidation (Debug builds always validate)
	std::string m_ProfileFile;			// -gfxProfile <file.csv>; empty: profiling off
	//	Windows render into an image of their size, with no swap chain
	//	(RenderNode, which never shows them). Only GraphicsVulkan reads it:
	//	DX11's windows keep their swap chains on the hidden native windows.
	bool		m_bHeadless = false;
};
