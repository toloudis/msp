/*****************************************************************************
**  vkdGlobal.cpp
**
**      See vkdGlobal.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	namespace
	{
		vkdDevice*		l_pDevice = 0;
		vkdRingBuffer*	l_pRing = 0;
		vkdBindings*	l_pBindings = 0;
	}

	namespace vkdGlobal
	{
		vkdDevice& Device()
		{
			DBG_ASSERT(l_pDevice, "vkdGlobal: no Vulkan device (GraphicsVulkanLayer not initialized)");
			return *l_pDevice;
		}

		vkdRingBuffer& Ring()
		{
			DBG_ASSERT(l_pRing, "vkdGlobal: no ring buffer (GraphicsVulkanLayer not initialized)");
			return *l_pRing;
		}

		vkdBindings& Bindings()
		{
			DBG_ASSERT(l_pBindings, "vkdGlobal: no bindings (GraphicsVulkanLayer not initialized)");
			return *l_pBindings;
		}

		bool IsSet()
		{
			return l_pDevice != 0;
		}

		void Set(vkdDevice* i_pDevice, vkdRingBuffer* i_pRing, vkdBindings* i_pBindings)
		{
			l_pDevice = i_pDevice;
			l_pRing = i_pRing;
			l_pBindings = i_pBindings;
		}
	}
}
