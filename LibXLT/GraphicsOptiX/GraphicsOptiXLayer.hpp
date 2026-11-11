/*****************************************************************************
**  GraphicsOptiXLayer.hpp
**
**      GraphicsOptiXLayer initializes the GraphicsOptiX library: an NVIDIA
**	OptiX (ray tracing) implementation of the Graphics abstractions, with
**	the same calls as GraphicsDX11Layer and GraphicsVulkanLayer, so
**	MachStudio can start it the same way. Init can fail (no NVIDIA driver
**	with OptiX, ...); GetInitError then says why.
**
**	Stub: every Graphics hook is installed, but renders, loads and creates
**	nothing yet. The library is built up from here.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_GRAPHICSOPTIXLAYER_HPP
#error GraphicsOptiXLayer.hpp multiply included
#endif
#define GOX_GRAPHICSOPTIXLAYER_HPP

#include <string>

class g2dSystem;
class g3dSystem;
struct g3dGfxOptions;

class GraphicsOptiXLayer
{
	public:
		//------------------------------------------------------------------------
		//	Init - loads OptiX and creates the 2D/3D systems. Returns false
		//	(and GetInitError says why) if OptiX can't be used.
		//------------------------------------------------------------------------
		static bool Init(void* i_hWnd, const g3dGfxOptions& i_Options);

		//------------------------------------------------------------------------
		//	CleanUp
		//------------------------------------------------------------------------
		static void CleanUp() throw();

		//------------------------------------------------------------------------
		//	InitGraphics - should be called after device is created
		//------------------------------------------------------------------------
		static void InitGraphics();

		//------------------------------------------------------------------------
		//	CleanUpGraphics - should be called before device is destroyed
		//------------------------------------------------------------------------
		static void CleanUpGraphics();

		//------------------------------------------------------------------------
		//	Accessors for the major graphics system objects
		//------------------------------------------------------------------------
		static g2dSystem* GetSystem2D();
		static g3dSystem* GetSystem3D();

		//------------------------------------------------------------------------
		//	GetName - the backend's name for people ("OptiX");
		//	GetCodeName - its name in command lines, logs and files ("optix")
		//------------------------------------------------------------------------
		static const char* GetName();
		static const char* GetCodeName();

		//------------------------------------------------------------------------
		//	GetInitError - why Init failed (empty if it didn't)
		//------------------------------------------------------------------------
		static const std::string& GetInitError();
};
