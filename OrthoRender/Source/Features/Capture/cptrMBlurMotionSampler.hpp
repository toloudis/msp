/*****************************************************************************
**  cptrMBlurMotionSampler.hpp
**
**      The Capture mode
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_MBLURMOTIONSAMPLER_HPP
#error cptrMBlurMotionSampler.hpp multiply included
#endif
#define CPTR_MBLURMOTIONSAMPLER_HPP

#ifndef CPTR_MOTIONSAMPLER_HPP
#include "Features/Capture/cptrMotionSampler.hpp"
#endif

//============================================================================
//============================================================================
class cptrAccumulationBuffer;
class g3dViewer;

//============================================================================
//============================================================================
class cptrSupersampleMotionSampler : public cptrMotionSampler
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrSupersampleMotionSampler(int nSamplesPerFrame, g3dViewer* pViewer);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~cptrSupersampleMotionSampler();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void CaptureMotionSample(camCamera& i_Camera, float timeline_time);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void CaptureFrame(camCamera& i_Camera);

protected:
	// the 3d scene renderer
	g3dViewer *m_pViewer;

	// combine several images together in this buffer
	cptrAccumulationBuffer* m_pMotionBuffer;

	// jittered sampling into this buffer
	cptrAccumulationBuffer* m_pAABuffer;

	// dimensions of accumulation buffer
	int m_w, m_h;

	// draw frame with jittered camera
	void RenderJitteredFrame(camCamera& i_Camera);
};
