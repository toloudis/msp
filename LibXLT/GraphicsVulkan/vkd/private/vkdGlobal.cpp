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
		vkdPipelineCache* l_pPipelines = 0;
		vkdContext*		l_pContext = 0;
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

		vkdPipelineCache& Pipelines()
		{
			DBG_ASSERT(l_pPipelines, "vkdGlobal: no pipeline cache (GraphicsVulkanLayer not initialized)");
			return *l_pPipelines;
		}

		vkdContext& Context()
		{
			DBG_ASSERT(l_pContext, "vkdGlobal: no context (GraphicsVulkanLayer not initialized)");
			return *l_pContext;
		}

		bool IsSet()
		{
			return l_pDevice != 0;
		}

		void Set(vkdDevice* i_pDevice, vkdRingBuffer* i_pRing, vkdBindings* i_pBindings,
				 vkdPipelineCache* i_pPipelines, vkdContext* i_pContext)
		{
			l_pPipelines = i_pPipelines;
			l_pContext = i_pContext;
			l_pDevice = i_pDevice;
			l_pRing = i_pRing;
			l_pBindings = i_pBindings;
		}
	}
}
