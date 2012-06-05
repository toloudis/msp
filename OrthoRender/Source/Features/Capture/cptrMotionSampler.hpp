/*****************************************************************************
**  cptrMotionSampler.hpp
**
**      Capture motion sampler
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_MOTIONSAMPLER_HPP
#error cptrMotionSampler.hpp multiply included
#endif
#define CPTR_MOTIONSAMPLER_HPP

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif


//============================================================================
//============================================================================
class cptrMotionSampler 
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrMotionSampler(int nSamplesPerFrame);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~cptrMotionSampler() {}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void CaptureMotionSample(camCamera& i_Camera, float timeline_time) {}
	
	//------------------------------------------------------------------------
	// do we need to do the final frame?
	//------------------------------------------------------------------------
	bool DidShutterClose()
	{
		if (m_nSamplesCollected % m_nSamplesPerFrame == 0)
			return true;
		return false;
	}

	//------------------------------------------------------------------------
	// intended to be called when shutter closes
	//------------------------------------------------------------------------
	virtual void CaptureFrame(camCamera& i_Camera) {}

protected:
	int m_nSamplesCollected;
	const int m_nSamplesPerFrame;
};
