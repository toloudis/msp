/*****************************************************************************
**  vkdOcclusionQuery.cpp
**
**      See vkdOcclusionQuery.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDOCCLUSIONQUERY_HPP
#include "GraphicsVulkan/vkd/vkdOcclusionQuery.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <mutex>

namespace gvk
{
	vkdOcclusionQuery::vkdOcclusionQuery(vkdDevice& i_Device)
		: m_Device(i_Device)
	{
		VkQueryPoolCreateInfo info = {VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
		info.queryType = VK_QUERY_TYPE_OCCLUSION;
		info.queryCount = 1;
		VK_CHECK(vkCreateQueryPool(m_Device.Device(), &info, NULL, &m_Pool));
		m_Device.SetName(VK_OBJECT_TYPE_QUERY_POOL, reinterpret_cast<uint64_t>(m_Pool), "vkdOcclusionQuery");
	}

	vkdOcclusionQuery::~vkdOcclusionQuery()
	{
		DBG_ASSERT(!m_bActive, "vkdOcclusionQuery destroyed between Begin and End");
		if (m_bPending)
		{
			//	the GPU may still write it
			m_Device.WaitFor(m_Value);
		}
		vkDestroyQueryPool(m_Device.Device(), m_Pool, NULL);
	}

	void vkdOcclusionQuery::Begin(vkdContext& i_Context)
	{
		DBG_ASSERT(!m_bActive, "vkdOcclusionQuery::Begin twice");
		if (m_bPending)
		{
			//	a result never read: its submission must be done before the reset
			if (m_Value == m_Device.PendingValue())
			{
				i_Context.Flush();
			}
			m_Device.WaitFor(m_Value);
			m_bPending = false;
		}
		vkResetQueryPool(m_Device.Device(), m_Pool, 0, 1);
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		vkCmdBeginQuery(i_Context.Commands(), m_Pool, 0, VK_QUERY_CONTROL_PRECISE_BIT);
		m_bActive = true;
	}

	void vkdOcclusionQuery::End(vkdContext& i_Context)
	{
		DBG_ASSERT(m_bActive, "vkdOcclusionQuery::End without Begin");
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		vkCmdEndQuery(i_Context.Commands(), m_Pool, 0);
		m_Value = m_Device.PendingValue();
		m_bActive = false;
		m_bPending = true;
	}

	uint64_t vkdOcclusionQuery::GetResult(vkdContext& i_Context)
	{
		DBG_ASSERT(m_bPending, "vkdOcclusionQuery::GetResult without Begin and End");
		if (!m_bPending)
		{
			return 0;
		}
		if (m_Value == m_Device.PendingValue())
		{
			i_Context.Flush();		// End hasn't been submitted yet
		}
		m_Device.WaitFor(m_Value);
		m_bPending = false;
		uint64_t samples = 0;
		VK_CHECK(vkGetQueryPoolResults(m_Device.Device(), m_Pool, 0, 1, sizeof(samples), &samples, sizeof(samples),
									   VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT));
		return samples;
	}
}
