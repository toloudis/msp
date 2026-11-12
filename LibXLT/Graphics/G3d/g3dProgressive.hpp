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
**
**	Both flags may be set and read from any thread.
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

	void SetCapturing(bool i_bCapturing);
	bool IsCapturing();

	//	Sets capturing for its lifetime (exception safe)
	class CaptureScope
	{
		public:
			CaptureScope();
			~CaptureScope();
			CaptureScope(const CaptureScope&) = delete;
			CaptureScope& operator=(const CaptureScope&) = delete;

		private:
			bool m_bWasCapturing;
	};
}
