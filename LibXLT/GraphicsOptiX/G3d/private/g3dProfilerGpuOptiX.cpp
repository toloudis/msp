/*****************************************************************************
**  g3dProfilerGpuOptiX.cpp
**
**      See g3dProfilerGpuOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DPROFILERGPUOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dProfilerGpuOptiX.hpp"
#endif

#ifdef MSP_GFX_PROFILING

#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif

namespace gox
{
	g3dProfilerGpuOptiX::g3dProfilerGpuOptiX(g3dProfilerGpu* i_pInner)
		: m_pInner(i_pInner)
		, m_bInFrame(false)
	{
	}

	g3dProfilerGpuOptiX::~g3dProfilerGpuOptiX()
	{
		if (!optxContext::Exists())
		{
			return;		// destroying the context released the events
		}
		optxContext::MakeCurrent();
		recycle(m_Current);
		for (Frame& frame : m_Frames)
		{
			recycle(frame);
		}
		for (CUevent event : m_Pool)
		{
			CU_CHECK(cu().cuEventDestroy(event));
		}
	}

	CUevent g3dProfilerGpuOptiX::take_event()
	{
		if (!m_Pool.empty())
		{
			CUevent event = m_Pool.back();
			m_Pool.pop_back();
			return event;
		}
		CUevent event = NULL;
		if (CU_CHECK(cu().cuEventCreate(&event, CU_EVENT_DEFAULT)))
		{
			return NULL;
		}
		return event;
	}

	void g3dProfilerGpuOptiX::recycle(Frame& io_Frame)
	{
		for (CUevent event : io_Frame.m_Events)
		{
			if (event)
			{
				m_Pool.push_back(event);
			}
		}
		io_Frame.m_Events.clear();
	}

	void g3dProfilerGpuOptiX::record(Frame& io_Frame, size_t i_Index)
	{
		if (io_Frame.m_Events.size() <= i_Index)
		{
			io_Frame.m_Events.resize(i_Index + 1, NULL);
		}
		CUevent& event = io_Frame.m_Events[i_Index];
		if (!event)
		{
			event = take_event();
		}
		if (event)
		{
			CU_CHECK(cu().cuEventRecord(event, optxContext::GetStream()));
		}
	}

	void g3dProfilerGpuOptiX::BeginFrame(uint64_t i_FrameId)
	{
		optxContext::MakeCurrent();
		m_pInner->BeginFrame(i_FrameId);
		recycle(m_Current);
		m_Current.m_Id = i_FrameId;
		m_bInFrame = true;
	}

	int g3dProfilerGpuOptiX::BeginScope()
	{
		const int token = m_pInner->BeginScope();
		if (m_bInFrame && token >= 0)
		{
			record(m_Current, size_t(token) * 2);
		}
		return token;
	}

	void g3dProfilerGpuOptiX::EndScope(int i_Token)
	{
		m_pInner->EndScope(i_Token);
		if (m_bInFrame && i_Token >= 0)
		{
			record(m_Current, size_t(i_Token) * 2 + 1);
		}
	}

	void g3dProfilerGpuOptiX::EndFrame()
	{
		m_pInner->EndFrame();
		m_Frames.push_back(m_Current);
		m_Current.m_Events.clear();		// now the queued frame's
		m_bInFrame = false;
	}

	bool g3dProfilerGpuOptiX::Resolve(bool i_bWait, uint64_t& o_FrameId, std::vector<double>& o_ScopeMs, bool& o_bValid)
	{
		if (!m_pInner->Resolve(i_bWait, o_FrameId, o_ScopeMs, o_bValid))
		{
			return false;
		}
		m_LastMs.clear();
		//	Frames resolve in order: drop any older ones Vulkan never reported
		while (!m_Frames.empty() && m_Frames.front().m_Id < o_FrameId)
		{
			recycle(m_Frames.front());
			m_Frames.pop_front();
		}
		if (m_Frames.empty() || m_Frames.front().m_Id != o_FrameId)
		{
			return true;
		}
		Frame& frame = m_Frames.front();
		optxContext::MakeCurrent();
		const size_t tokens = frame.m_Events.size() / 2;
		m_LastMs.assign(tokens, 0.0);
		for (size_t t = 0; t < tokens; ++t)
		{
			CUevent start = frame.m_Events[2 * t], end = frame.m_Events[2 * t + 1];
			float ms = 0.0f;
			if (start && end && !CU_CHECK(cu().cuEventSynchronize(end)) &&
				!CU_CHECK(cu().cuEventElapsedTime(&ms, start, end)))
			{
				m_LastMs[t] = ms;
			}
		}
		recycle(frame);
		m_Frames.pop_front();
		return true;
	}

	void g3dProfilerGpuOptiX::GetSecondMs(std::vector<double>& o_ScopeMs)
	{
		o_ScopeMs = m_LastMs;
	}
}

#endif
