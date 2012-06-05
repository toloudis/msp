/*****************************************************************************
**  cptrMBlurMotionSampler.cpp
**
**      see .h
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrMBlurMotionSampler.hpp"

#include "Features/Capture/cptrAccumulationBuffer.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/cptrWriteUtil.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/mnm/mnmTimeCodeMgr.hpp"

// library
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Core/app/appSimTime.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "GraphicsDX9/g3d/g3dRenderScreenSpace.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dViewer.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrSupersampleMotionSampler::cptrSupersampleMotionSampler(int nSamplesPerFrame, g3dViewer* pViewer)
:	cptrMotionSampler(nSamplesPerFrame),
	m_pViewer(pViewer),m_w(0),m_h(0),m_pMotionBuffer(NULL), m_pAABuffer(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrSupersampleMotionSampler::~cptrSupersampleMotionSampler()
{
	delete m_pMotionBuffer;
	delete m_pAABuffer;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrSupersampleMotionSampler::CaptureMotionSample(camCamera& i_Camera, float timeline_time)
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	m_nSamplesCollected++;

	// init accumulation buffer on first motion sample of the frame
	if ((m_nSamplesCollected % m_nSamplesPerFrame) == 1)
	{
		if (m_pMotionBuffer)
		{
			DBG_ASSERT0((m_w*m_h) > 0, "Bad image dimensions for clearing motion buffer");
			m_pMotionBuffer->Clear();
		}
		else
		{
			m_w = data.m_nWidth.GetValue();
			m_h = data.m_nHeight.GetValue();
			m_pMotionBuffer = new cptrAccumulationBuffer(m_w,m_h,m_nSamplesPerFrame);
		}
	}

	// in this algo, we will capture a "frame" and store it in an accumulation buffer,
	// to be added and averaged with previous samples

	// TODO : apply the time code to the final frame

	// render away!
	g2dPixelR8G8B8* currentFrameBuffer = NULL;
	int width=0,ht=0;

	api3dTargetRendererMgr::RenderTargets( appSimTime::GetTime() );

	if (cptrRenderUtil::GetCaptureQuadrants())
	{
		// Render large resolution in 4 quadrant renders
		float timeline_time = tmlnTimeLine::GetValue();

		if (m_pViewer && cptrRenderUtil::GetCapture())
		{
			m_pViewer->SetCamera(&i_Camera);
			cptrRenderUtil::BeginQuadrants();

			// no time code on a motion sampled image
			bool show_timecode = mnmTimeCodeMgr::IsShowTimeCode();
			mnmTimeCodeMgr::SetShowTimeCode( false );

			const int num_quadrants = cptrRenderUtil::GetNumQuadrants();
			for (int i=0; i<num_quadrants; i++)
			{
				// Do Quadrant
				cptrRenderUtil::ConfigureViewport(i, i_Camera);
				m_pViewer->Render(timeline_time);
				cptrRenderUtil::CaptureQuadrant(i);
				m_pViewer->Present();
			}

			cptrRenderUtil::EndQuadrants(&currentFrameBuffer,width,ht);

			// restore camera
			i_Camera.SetSubViewport(-1,1, -1,1);

			if ( show_timecode )
				mnmTimeCodeMgr::SetShowTimeCode( true );
		}
	}
	else
	{
		// Render capture window normally
		if (m_pViewer)
		{
			// no time code on a motion sampled image
			bool show_timecode = mnmTimeCodeMgr::IsShowTimeCode();
			mnmTimeCodeMgr::SetShowTimeCode( false );

			if (data.m_bJitteredSampling.GetValue())
			{
				RenderJitteredFrame(i_Camera);
			}
			else
			{
				float timeline_time = tmlnTimeLine::GetValue();
				m_pViewer->SetCamera(&i_Camera);
				m_pViewer->Render(timeline_time);
			}

			if ( show_timecode )
				mnmTimeCodeMgr::SetShowTimeCode( true );
		}
		//	capture the current frame
		//
		if ( cptrRenderUtil::GetCapture() )
		{
			if (!data.m_bJitteredSampling.GetValue())
			{
				cptrRenderUtil::CaptureFrame(&currentFrameBuffer,width,ht);
			}
		}

		if (m_pViewer)
		{
			m_pViewer->Present();
		}
	}

	if (currentFrameBuffer)
	{
		DBG_ASSERT0(width == m_w, "Motion sampling width mismatch");
		DBG_ASSERT0(ht == m_h, "Motion sampling height mismatch");

		// add in to accumulation buffer.
		// am I damn sure the width and height match up? you bet i am!
		DBG_ASSERT0((m_w*m_h) > 0, "Bad image dimensions for motion buffer");
		m_pMotionBuffer->Add(currentFrameBuffer);
		delete [] currentFrameBuffer;
	}
	else if (data.m_bJitteredSampling.GetValue())
	{
		DBG_ASSERT0(m_pMotionBuffer, "Motion accumulation buffer not initialized");
		m_pMotionBuffer->Add(m_pAABuffer);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrSupersampleMotionSampler::CaptureFrame(camCamera& i_Camera)
{
	// average the accumulated samples into a d3d surface
	m_pMotionBuffer->Capture();
	// then pass the surface to the frame capture writer...

	// put image to backbuffer, then draw time stamp on top.
	m_pViewer->GetWindow()->MakeCurrent();
	m_pViewer->GetWindow()->DrawImage(0,0, *m_pMotionBuffer->GetAsImage());
	// draw time stamp...
	// render time code (layer 2 = screen space layer)
	g3dRenderScreenSpaceObjects rScreenSpace;
	rScreenSpace.Set(m_pViewer->GetScene()->GetLayer(2),&i_Camera,NULL);
	float timeline_time = tmlnTimeLine::GetValue();
	rScreenSpace.Render(timeline_time);

	// assume cptrRenderUtil has the same window as m_pViewer->GetWindow
	cptrRenderUtil::CaptureFrame();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrSupersampleMotionSampler::RenderJitteredFrame(camCamera& i_Camera)
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	// first time (lazy) init the buffer
	if (m_pAABuffer == NULL)
		m_pAABuffer = new cptrAccumulationBuffer(data.m_nWidth.GetValue(), data.m_nHeight.GetValue(), data.m_nCaptureSampling.GetValue()*data.m_nCaptureSampling.GetValue());
	else
		m_pAABuffer->Clear();

	float timeline_time = tmlnTimeLine::GetValue();

	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	// draw each jittered sample,
	// and then put the accum buffer back in the backbuffer,
	// to be captured?
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	int i;
	for (i = 0; i < data.m_nCaptureSampling.GetValue()*data.m_nCaptureSampling.GetValue(); i++)
	{
		float dx = (cptrRenderUtil::GetSamplePos(i).GetX() - 0.5f)*2.0f/(float)data.m_nWidth.GetValue();
		float dy = (cptrRenderUtil::GetSamplePos(i).GetY() - 0.5f)*2.0f/(float)data.m_nHeight.GetValue();

		i_Camera.SetSubViewport(-1+dx, 1+dx, -1+dy, 1+dy);
		m_pViewer->SetCamera(&i_Camera);
		m_pViewer->Render(timeline_time);
		i_Camera.SetSubViewport(-1, 1, -1, 1);

		g2dPixelR8G8B8* currentFrameBuffer = NULL;
		int w,h;
		cptrRenderUtil::CaptureFrame(&currentFrameBuffer, w, h);
		data.m_nWidth.SetValue(w);
		data.m_nHeight.SetValue(h);
		m_pAABuffer->Add(currentFrameBuffer);
		delete [] currentFrameBuffer;

		// we can now draw the frame sample into the window.
		m_pViewer->Present();

	}
	m_pAABuffer->Capture();
}
