/*****************************************************************************
**  g3dProgressive.cpp
**
**      See g3dProgressive.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef G3D_PROGRESSIVE_HPP
#include "Graphics/G3d/g3dProgressive.hpp"
#endif

#include <atomic>
#include <mutex>

namespace g3dProgressive
{
	namespace
	{
		std::atomic<bool> l_bFrameRequested(false);
		std::atomic<bool> l_bCapturing(false);
	}

	void RequestFrame()
	{
		l_bFrameRequested = true;
	}

	bool IsFrameRequested()
	{
		return l_bFrameRequested;
	}

	void ClearFrameRequest()
	{
		l_bFrameRequested = false;
	}

	namespace
	{
		std::mutex		l_SamplingMutex;
		CaptureSampling	l_Sampling;
	}

	void SetCapturing(bool i_bCapturing, const CaptureSampling& i_Sampling)
	{
		{
			std::lock_guard<std::mutex> lock(l_SamplingMutex);
			l_Sampling = i_bCapturing ? i_Sampling : CaptureSampling();
		}
		l_bCapturing = i_bCapturing;
	}

	bool IsCapturing()
	{
		return l_bCapturing;
	}

	CaptureSampling GetCaptureSampling()
	{
		std::lock_guard<std::mutex> lock(l_SamplingMutex);
		return l_bCapturing ? l_Sampling : CaptureSampling();
	}

	CaptureScope::CaptureScope(const CaptureSampling& i_Sampling)
		: m_bWasCapturing(IsCapturing())
		, m_WasSampling(GetCaptureSampling())
	{
		SetCapturing(true, i_Sampling);
	}

	CaptureScope::~CaptureScope()
	{
		SetCapturing(m_bWasCapturing, m_WasSampling);
	}
}
