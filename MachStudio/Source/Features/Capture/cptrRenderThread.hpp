/*****************************************************************************
**	cptrRenderThread.hpp
**
**		Encapsulation of rendering thread when capturing,
**	catches exceptions and keeps track of the state of the
**	thread including if the error state.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERTHREAD_HPP
#error cptrRenderThread.hpp multiply included
#endif
#define CPTR_RENDERTHREAD_HPP

#ifndef MODE_MODETIME_HPP
#include "Support/mode/modeModeTime.hpp"
#endif
#ifndef TMLN_TIMEINOUTDATA_HPP
#include "Support/tmln/tmlnTimeInOutData.hpp"
#endif
#ifndef DM_FSM_HPP
#include "Core/dm/dmFSM.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef RLYR_PASSESOBJECT_HPP
#include "Support/rlyr/rlyrPassesObject.hpp"
#endif

//============================================================================
//============================================================================
struct mrayOptionsData;
struct rmanOptionsData;
class camCamera;
class cptrAccumulationBuffer;
class cptrMotionSampler;
class fsLocator;
class g2dSystem;
class g2dWindow;
class g3dSceneRenderer;
class g3dViewer;
class rprfPrefsObject;


//============================================================================
//============================================================================
class cptrRenderThread
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cptrRenderThread();

	//--------------------------------------------------------------------
	// pure-virtual, children must be created
	//--------------------------------------------------------------------
	virtual ~cptrRenderThread();

	//--------------------------------------------------------------------
	// Set the viewer to use for the rendering
	//--------------------------------------------------------------------
//	void SetViewer(g3dViewer *i_pViewer);

	//--------------------------------------------------------------------
	// Set the viewer to use for the rendering
	//--------------------------------------------------------------------
	void SetWindow(g2dWindow *i_pWindow);

	//--------------------------------------------------------------------
	// Reset counter for number of frames in sequence
	//--------------------------------------------------------------------
	void ResetRenderCaptureIteration();

	//--------------------------------------------------------------------
	// SetDoneFirstPass 
	//--------------------------------------------------------------------
	void SetDoneFirstPass(bool i_bDoneFirstPass);

	//--------------------------------------------------------------------
	// Free the motion sampler and accumlation buffers
	//--------------------------------------------------------------------
	void FreeInternalBuffers();
	
	//--------------------------------------------------------------------
	// Returns true if the last attempt at TryRenderCapture
	//	returned an error.
	//--------------------------------------------------------------------
	bool IsErrorState() const;

	//--------------------------------------------------------------------
	// Clears previous error state
	//--------------------------------------------------------------------
	void ClearErrorState();

	//--------------------------------------------------------------------
	// Returns true if the error is irrecoverable and the 
	//	application must exit.
	//--------------------------------------------------------------------
	bool ShouldAppExit() const;

	//--------------------------------------------------------------------
	// Get error message from exception if in error state.
	//--------------------------------------------------------------------
	const std::string& GetErrorMessage();

	//--------------------------------------------------------------------
	// Starts render with a RenderThreadWrapper to mark begin and end
	//	of render thread. Call this function from an envThread instance.
	//--------------------------------------------------------------------
	void ThreadedRenderCapture();

	//--------------------------------------------------------------------
	// attempt a call to do_render_capture and catch exceptions that are
	// thrown, aborting capture mode if exception is found.
	//--------------------------------------------------------------------
	void TryRenderCapture();

private:
	//----------------------------------------------------------------------------
	// render a whole set of layers on each frame.
	//----------------------------------------------------------------------------
	void DoRenderCaptureLayers();

	//--------------------------------------------------------------------
	// do render in secondary window and capture the frame
	//--------------------------------------------------------------------
	void DoRenderCapture();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void processAnaglyph(camCamera *pCamera);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool RenderJitteredFrame(float i_StereoOffset);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void RenderScene( const maTime& i_timeline );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetupRenderLayer(int i_CurrentLayerIndex);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetupPassBuffers();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetupPass(rlyrPassesObject::ePassType i_CurrentPass);

	//----------------------------------------------------------------------------
	// GetRmanOptions()
	//----------------------------------------------------------------------------
	rmanOptionsData GetRmanOptions(rprfPrefsObject* prefsObject);

	//----------------------------------------------------------------------------
	// GetMRayOptions()
	//----------------------------------------------------------------------------
	mrayOptionsData GetMRayOptions(rprfPrefsObject* prefsObject);

	//--------------------------------------------------------------------
	//	get the next render layer index
	//--------------------------------------------------------------------
	int get_next_layer_index(int i_CurrentLayerIndex);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CreateOutputDir(const fsLocator& i_OutputDirName);

private:

	bool m_bErrorState;
	bool m_bAppExit;
	std::string m_ErrorMessage;

	int m_nDoRenderCaptureIteration;
	bool m_bDoneFirstPass;
	cptrMotionSampler* m_MotionSampler;
	cptrAccumulationBuffer* m_AccBuf;
	g2dWindow *m_pWindow;
	int m_CurrentLayerIndex;
	int m_CurrentPass;
	g3dPrefs::g3dRenderPrefs m_CurrentLayerPrefs;

	g3dViewer *m_pViewer;
	// Enumeration for which renderer style to use
	enum RendererType
	{
		e_HDR = 0,
		e_AmbientOcclusion,
		e_Depth,
		e_ShadowMask,
		e_IlluminationOnly,
		e_Normals,
		e_DirtyMatte,
		e_Wireframe,
		e_Materials,
		e_ReflectionOnly,
		e_VelocityMap,
		e_GlobalIllumination,
		e_Glow,
		e_NumTypes
	};
	std::vector<g3dSceneRenderer*>   m_Renderers;

	void RenderAndCaptureBuckets(camCamera& i_Camera, const maTime& i_TimelineTime);
};
