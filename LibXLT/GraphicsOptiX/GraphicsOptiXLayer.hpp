/*****************************************************************************
**  GraphicsOptiXLayer.hpp
**
**      GraphicsOptiXLayer initializes the GraphicsOptiX library: NVIDIA
**	OptiX (ray tracing) on top of GraphicsVulkan, with the same calls as
**	GraphicsDX11Layer and GraphicsVulkanLayer, so MachStudio can start it
**	the same way. Init can fail (no NVIDIA driver with OptiX, no Vulkan
**	device that is the CUDA device, ...); GetInitError then says why.
**
**	Init loads CUDA and OptiX and creates optxContext (the GPU is CUDA's
**	choice), then starts the whole GraphicsVulkan layer on that same GPU
**	(by UUID) with external memory, and installs its own scene renderer
**	creator over Vulkan's. Everything else is Vulkan's: windows, text,
**	textures, shaders, lights, fragments, capture, picking, baking.
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

		//------------------------------------------------------------------------
		//	GetOptixMessageCounts - OptiX messages at error (and fatal) and at
		//	warning level so far; kept after CleanUp, like Vulkan's counts
		//	(GraphicsVulkanLayer::GetValidationStats)
		//------------------------------------------------------------------------
		static void GetOptixMessageCounts(int& o_nErrors, int& o_nWarnings);
};
