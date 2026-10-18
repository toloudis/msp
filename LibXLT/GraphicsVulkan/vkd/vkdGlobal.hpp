/*****************************************************************************
**  vkdGlobal.hpp
**
**      The GraphicsVulkan objects shared by all its packages, set up by
**	GraphicsVulkanLayer (the counterpart of GraphicsDX11's g2dDX11Global).
**	Only valid between GraphicsVulkanLayer::Init and CleanUp.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDGLOBAL_HPP
#error vkdGlobal.hpp multiply included
#endif
#define GVK_VKDGLOBAL_HPP

namespace gvk
{
	class vkdDevice;
	class vkdRingBuffer;
	class vkdBindings;

	namespace vkdGlobal
	{
		vkdDevice&		Device();
		vkdRingBuffer&	Ring();
		vkdBindings&	Bindings();
		bool			IsSet();

		//	Called by GraphicsVulkanLayer; Set(NULL, NULL, NULL) clears.
		void Set(vkdDevice* i_pDevice, vkdRingBuffer* i_pRing, vkdBindings* i_pBindings);
	}
}
