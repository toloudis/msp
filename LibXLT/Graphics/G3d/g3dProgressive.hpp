/*****************************************************************************
**  g3dProgressive.hpp
**
**      g3dProgressive connects progressive scene renderers (GraphicsOptiX's
**	AO renderer, which refines its image a few samples per frame) with the
**	code that decides when to render:
**
**	- RequestFrame: a renderer whose image isn't finished asks for another
**	  frame. The application's render loop draws again while one is
**	  requested (gpxRenderControl::IsRenderNeeded), even when nothing in
**	  the scene changed, and clears the request when it starts a render
**	  (gpxRenderControl::RenderStarted).
**	- Capturing: the capture path (cptrRenderThread) sets it around the
**	  renders it captures. While it is set, a progressive renderer
**	  finishes each frame (all its capture samples) before Render returns,
**	  so the captured image is the converged one.
**	- The capture's sampling (CaptureSampling: its n x n supersampling,
**	  pixel filter and filter width), set with capturing. A renderer that
**	  samples captures itself (g3dSceneRenderer::SamplesCaptures: the
**	  tracer) takes n x n times its capture samples and weights them by the
**	  filter, so the capture code renders it once instead of n x n shifted
**	  frames.
**
**	All of it may be set and read from any thread.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef G3D_PROGRESSIVE_HPP
#error g3dProgressive.hpp multiply included
#endif
#define G3D_PROGRESSIVE_HPP

namespace g3dProgressive
{
	void RequestFrame();
	bool IsFrameRequested();
	void ClearFrameRequest();

	//	The capture filters, in captRenderOutputData::eFilterFunc's order
	//	(Core's maFilter kinds)
	enum FilterKind
	{
		e_FilterBox,
		e_FilterGaussian,
		e_FilterMitchell,
		e_FilterTriangle,
		e_FilterSinc,
		e_FilterLanczos,
		e_FilterBlackmanHarris,
		e_FilterCatmullRom,
		e_FilterCount
	};

	//	A capture's sampling: n x n supersampling, the filter its samples are
	//	weighted by and its width in pixels (the default: none, a box of
	//	width 1)
	struct CaptureSampling
	{
		int			m_nSupersampling = 1;
		FilterKind	m_Filter = e_FilterBox;
		float		m_FilterWidth = 1.0f;

		bool operator==(const CaptureSampling& i_Other) const
		{
			return m_nSupersampling == i_Other.m_nSupersampling && m_Filter == i_Other.m_Filter &&
				   m_FilterWidth == i_Other.m_FilterWidth;
		}
	};

	//	SetCapturing(false) also puts the sampling back to the default
	void SetCapturing(bool i_bCapturing, const CaptureSampling& i_Sampling = CaptureSampling());
	bool IsCapturing();
	//	The running capture's sampling (the default when none runs)
	CaptureSampling GetCaptureSampling();

	//	Sets capturing, with a sampling, for its lifetime (exception safe)
	class CaptureScope
	{
		public:
			explicit CaptureScope(const CaptureSampling& i_Sampling = CaptureSampling());
			~CaptureScope();
			CaptureScope(const CaptureScope&) = delete;
			CaptureScope& operator=(const CaptureScope&) = delete;

		private:
			bool			m_bWasCapturing;
			CaptureSampling	m_WasSampling;
	};
}
