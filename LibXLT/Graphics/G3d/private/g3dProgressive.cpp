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

	void SetCapturing(bool i_bCapturing)
	{
		l_bCapturing = i_bCapturing;
	}

	bool IsCapturing()
	{
		return l_bCapturing;
	}

	CaptureScope::CaptureScope()
		: m_bWasCapturing(l_bCapturing.exchange(true))
	{
	}

	CaptureScope::~CaptureScope()
	{
		l_bCapturing = m_bWasCapturing;
	}
}
