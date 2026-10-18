/*****************************************************************************
**  g3dProfilerGpuVk.cpp
**
**      See g3dProfilerGpuVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef MSP_GFX_PROFILING

#ifndef GVK_G3DPROFILERGPUVK_HPP
#include "GraphicsVulkan/G3d/g3dProfilerGpuVk.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif

namespace gvk
{
	g3dProfilerGpuVk::g3dProfilerGpuVk(vkdDevice& i_Device)
		: m_Device(i_Device)
	{
		m_NsPerTick = i_Device.Properties().limits.timestampPeriod;
		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(i_Device.Physical(), &count, NULL);
		std::vector<VkQueueFamilyProperties> families(count);
		vkGetPhysicalDeviceQueueFamilyProperties(i_Device.Physical(), &count, families.data());
		const uint32_t bits = families[i_Device.QueueFamily()].timestampValidBits;
		m_ValidMask = bits >= 64 ? ~0ull : ((1ull << bits) - 1);
	}

	g3dProfilerGpuVk::~g3dProfilerGpuVk()
	{
		m_Device.WaitIdle();
		std::vector<Frame*> all(m_Free);
		all.insert(all.end(), m_InFlight.begin(), m_InFlight.end());
		if (m_pCurrent)
		{
			all.push_back(m_pCurrent);
		}
		for (Frame* frame : all)
		{
			vkDestroyQueryPool(m_Device.Device(), frame->m_Pool, NULL);
			delete frame;
		}
	}

	void g3dProfilerGpuVk::BeginFrame(uint64_t i_FrameId)
	{
		if (!m_Free.empty())
		{
			m_pCurrent = m_Free.back();
			m_Free.pop_back();
		}
		else
		{
			m_pCurrent = new Frame;
			VkQueryPoolCreateInfo info = {VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
			info.queryType = VK_QUERY_TYPE_TIMESTAMP;
			info.queryCount = 2 * c_MaxScopes;
			VK_CHECK(vkCreateQueryPool(m_Device.Device(), &info, NULL, &m_pCurrent->m_Pool));
			m_Device.SetName(VK_OBJECT_TYPE_QUERY_POOL, reinterpret_cast<uint64_t>(m_pCurrent->m_Pool), "g3dProfilerGpuVk");
		}
		m_pCurrent->m_Id = i_FrameId;
		m_pCurrent->m_Scopes = 0;
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		vkCmdResetQueryPool(m_Device.Commands(), m_pCurrent->m_Pool, 0, 2 * c_MaxScopes);
	}

	int g3dProfilerGpuVk::BeginScope()
	{
		DBG_ASSERT(m_pCurrent, "g3dProfilerGpuVk::BeginScope outside a frame");
		const uint32_t token = m_pCurrent->m_Scopes++;
		if (token < c_MaxScopes)
		{
			std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
			vkCmdWriteTimestamp2(m_Device.Commands(), VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, m_pCurrent->m_Pool, 2 * token);
		}
		return static_cast<int>(token);
	}

	void g3dProfilerGpuVk::EndScope(int i_Token)
	{
		DBG_ASSERT(m_pCurrent, "g3dProfilerGpuVk::EndScope outside a frame");
		if (static_cast<uint32_t>(i_Token) < c_MaxScopes)
		{
			std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
			vkCmdWriteTimestamp2(m_Device.Commands(), VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, m_pCurrent->m_Pool, 2 * i_Token + 1);
		}
	}

	void g3dProfilerGpuVk::EndFrame()
	{
		DBG_ASSERT(m_pCurrent, "g3dProfilerGpuVk::EndFrame outside a frame");
		m_pCurrent->m_Value = m_Device.PendingValue();
		m_InFlight.push_back(m_pCurrent);
		m_pCurrent = nullptr;
	}

	bool g3dProfilerGpuVk::Resolve(bool i_bWait, uint64_t& o_FrameId, std::vector<double>& o_ScopeMs, bool& o_bValid)
	{
		if (m_InFlight.empty())
		{
			return false;
		}
		Frame* frame = m_InFlight.front();
		if (m_Device.CompletedValue() < frame->m_Value)
		{
			if (!i_bWait)
			{
				return false;
			}
			if (frame->m_Value == m_Device.PendingValue())
			{
				m_Device.Submit();		// its commands haven't been submitted yet
			}
			m_Device.WaitFor(frame->m_Value);
		}
		m_InFlight.pop_front();

		const uint32_t scopes = std::min(frame->m_Scopes, c_MaxScopes);
		std::vector<uint64_t> ticks(2 * static_cast<size_t>(scopes) + 2);
		o_bValid = frame->m_Scopes <= c_MaxScopes;
		if (scopes > 0)
		{
			VkResult result = vkGetQueryPoolResults(m_Device.Device(), frame->m_Pool, 0, 2 * scopes,
													ticks.size() * sizeof(uint64_t), ticks.data(), sizeof(uint64_t),
													VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
			o_bValid = o_bValid && result == VK_SUCCESS;
		}
		o_FrameId = frame->m_Id;
		o_ScopeMs.assign(frame->m_Scopes, 0.0);
		for (uint32_t i = 0; i < scopes; ++i)
		{
			const uint64_t begin = ticks[2 * i] & m_ValidMask;
			const uint64_t end = ticks[2 * i + 1] & m_ValidMask;
			o_ScopeMs[i] = end >= begin ? static_cast<double>(end - begin) * m_NsPerTick / 1.0e6 : 0.0;
		}
		m_Free.push_back(frame);
		return true;
	}
}

#endif // MSP_GFX_PROFILING
