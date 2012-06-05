/*****************************************************************************
**	cptrRenderThread.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderThread.hpp"

#include "Features/Capture/cptrAccumulationBuffer.hpp"
#include "Features/Capture/cptrMBlurMotionSampler.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/cptrRenderStatsDialogUtil.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"
#include "Graphics/g3d/g3dPassBuffers.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captStereoUtil.hpp"
#include "Support/mnm/mnmTimeCodeMgr.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
//#include "Support/rlyr/data/rlyrPassesData.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"
#include "Support/rprf/rprfPrefsUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

//	library
#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
//#include "Core/Ma/maFloatRGBA.hpp"
#include "Core/name/nameString.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dImageCreate.hpp"
#include "Graphics/g2d/g2dImageSave.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "ImportExport/mray/export/mrayExportData.hpp"
#include "ImportExport/rman/export/rmanExportData.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
//#include "Tool/gpx/gpxProxyMgr.hpp"

#undef CreateDirectory

#include <windows.h>

#include <fstream>
#include <iostream>
#include <ostream>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderThread::cptrRenderThread()
:	m_bErrorState(false), m_bAppExit(false), m_bDoneFirstPass(false),
	m_nDoRenderCaptureIteration(-1),
	m_pWindow(NULL), m_pViewer(NULL), 
	m_CurrentLayerIndex(-1),
	m_CurrentPass(0),
	m_MotionSampler(NULL), m_AccBuf(NULL)
{
	// Create renderers
	m_Renderers.resize(e_NumTypes);

	m_Renderers[e_HDR] = g3dSceneRendererCreate::CreateHDRRenderer();

	m_Renderers[e_AmbientOcclusion] = g3dSceneRendererCreate::CreateAmbientOcclusionRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_AmbientOcclusion]);

	m_Renderers[e_Depth] = g3dSceneRendererCreate::CreateDepthRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_Depth]);

	m_Renderers[e_ShadowMask] = g3dSceneRendererCreate::CreateShadowMaskRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_ShadowMask]);

	m_Renderers[e_IlluminationOnly] = g3dSceneRendererCreate::CreateIlluminationRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_IlluminationOnly]);

	m_Renderers[e_Normals] = g3dSceneRendererCreate::CreateNormalRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_Normals]);

	m_Renderers[e_DirtyMatte] = g3dSceneRendererCreate::CreateRenderer(g3dSceneRendererTypes::e_DirtyMatte);
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_DirtyMatte]);

	m_Renderers[e_Wireframe] = g3dSceneRendererCreate::CreateRenderer(g3dSceneRendererTypes::e_Wireframe);
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_Wireframe]);

	m_Renderers[e_Materials] = g3dSceneRendererCreate::CreateMaterialsRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_Materials]);

	m_Renderers[e_ReflectionOnly] = g3dSceneRendererCreate::CreateReflectionOnlyRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_ReflectionOnly]);

	m_Renderers[e_VelocityMap] = g3dSceneRendererCreate::CreateVelocityMapRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_VelocityMap]);

	m_Renderers[e_GlobalIllumination] = g3dSceneRendererCreate::CreateGlobalIlluminationRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_GlobalIllumination]);

	m_Renderers[e_Glow] = g3dSceneRendererCreate::CreateGlowRenderer();
	m_Renderers[e_HDR]->ShareBuffers(m_Renderers[e_Glow]);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderThread::~cptrRenderThread()
{
	envSTLHelpers::DeleteContainer(m_Renderers);

	if (m_pViewer != NULL)
	{
		delete m_pViewer;
		m_pViewer = NULL;
	}
}

//--------------------------------------------------------------------
// Set the viewer to use for the rendering
//--------------------------------------------------------------------
//void cptrRenderThread::SetViewer(g3dViewer *i_pViewer)
//{
//	m_pViewer = i_pViewer;
//}

//--------------------------------------------------------------------
// Set the window to use for the rendering
//--------------------------------------------------------------------
void cptrRenderThread::SetWindow(g2dWindow *i_pWindow)
{
	m_pWindow = i_pWindow;
}

//--------------------------------------------------------------------
// Reset counter for number of frames in sequence
//--------------------------------------------------------------------
void cptrRenderThread::ResetRenderCaptureIteration()
{
	m_nDoRenderCaptureIteration = -1;
}

//--------------------------------------------------------------------
// SetDoneFirstPass 
//--------------------------------------------------------------------
void cptrRenderThread::SetDoneFirstPass(bool i_bDoneFirstPass)
{
	m_bDoneFirstPass = i_bDoneFirstPass;
}

//--------------------------------------------------------------------
// Free the motion sampler and accumlation buffers
//--------------------------------------------------------------------
void cptrRenderThread::FreeInternalBuffers()
{
	if (m_MotionSampler != NULL)
	{
		delete m_MotionSampler;
		m_MotionSampler = NULL;
	}
	if (m_AccBuf != NULL)
	{
		delete m_AccBuf;
		m_AccBuf = NULL;
	}
}

//--------------------------------------------------------------------
// Returns true if the last attempt at TryRenderCapture
//	returned an error.
//--------------------------------------------------------------------
bool cptrRenderThread::IsErrorState() const
{
	return m_bErrorState;
}

//--------------------------------------------------------------------
// Clears previous error state
//--------------------------------------------------------------------
void cptrRenderThread::ClearErrorState()
{
	m_ErrorMessage = "";
	m_bErrorState = false;
	m_bAppExit = false;
}

//--------------------------------------------------------------------
// Returns true if the error is irrecoverable and the 
//	application must exit.
//--------------------------------------------------------------------
bool cptrRenderThread::ShouldAppExit() const
{
	return m_bAppExit;
}

//--------------------------------------------------------------------
// Get error message from exception if in error state.
//--------------------------------------------------------------------
const std::string& cptrRenderThread::GetErrorMessage()
{
	return m_ErrorMessage;
}

//--------------------------------------------------------------------
// Starts render with a RenderThreadWrapper to mark begin and end
//	of render thread. Call this function from an envThread instance.
//--------------------------------------------------------------------
void cptrRenderThread::ThreadedRenderCapture()
{
	// Exception-safe way to track start and end of render thread
	g3dThreadControl::RenderThreadWrapper render_state;

	// do actual render
	this->TryRenderCapture();
}

//--------------------------------------------------------------------
// attempt a call to do_render_capture and catch exceptions that are
// thrown, aborting capture mode if exception is found.
//--------------------------------------------------------------------
void cptrRenderThread::TryRenderCapture()
{
	//bga - I wish the viewer was an argument to this function, but the 
	// motion capture and accumulation buffer objects need for the
	// viewer to be consistent, so it wouldn't really be possible to 
	// have different viewers in repeated calls to TryRenderCapture().
	// Maybe the viewer needs to be given in the constructor instead?
	DBG_ASSERT(m_pWindow, "Need window in order to render, call SetWindow()");
	if (!m_pWindow) return;

	this->ClearErrorState();
	try
	{
		DoRenderCaptureLayers();
	}
	catch ( const g2dCaptureX& /*i_Ex*/ )
	{
		//The surface was lost during capture
		m_ErrorMessage = "Problem accessing the render surface.";
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
		m_bAppExit  = true; //bga: is this "lost surface" exception?
	}
	catch ( const g2dImageSaveX& /*i_Ex*/ )
	{
		m_ErrorMessage = "Problem writing the image file.  Make sure no other programs have images in this sequence open.  ";
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
	}
	catch ( const envExceptionX& i_Ex )
	{
		m_ErrorMessage = "Error capturing frame. Aborting Capture, " + i_Ex.GetErrorMessage();
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
	}
	catch (const std::bad_alloc&)
	{
		m_ErrorMessage = "Out of system memory, could not render. Aborting Capture.";
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
	}
	catch (const std::exception& i_Ex)
	{
		m_ErrorMessage =  "Problem occurred during render: " + std::string(i_Ex.what()) + " Aborting Capture.";
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
		
	}
	catch(...)
	{
		m_ErrorMessage = "General exception error during render.";
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
	}
}

//----------------------------------------------------------------------------
// render a whole set of layers on each frame.
//----------------------------------------------------------------------------
void cptrRenderThread::DoRenderCaptureLayers()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	int lastLayerIndex = rlyrRenderLayerMgr::GetNumRenderLayers();

	// if render was aborted, currentpassindex will be initialized to the pass 
	// where we aborted.  if we are starting from scratch, get the first pass index.
	if (m_CurrentLayerIndex == -1)
		m_CurrentLayerIndex = get_next_layer_index(m_CurrentLayerIndex);

	fsLocator full_output;
	int frame_count;

	rmanMgr::ClearWrittenTextures();
	mrayMgr::ClearWrittenTextures();

	for (; 
		(m_CurrentLayerIndex < lastLayerIndex) && (m_CurrentLayerIndex != -1);
		m_CurrentLayerIndex = get_next_layer_index(m_CurrentLayerIndex))
	{
		SetupRenderLayer(m_CurrentLayerIndex);

		rlyrPassesObject* passesObject = rlyrRenderLayerMgr::GetLayerRenderPasses(
			rlyrRenderLayerMgr::GetLayerName(m_CurrentLayerIndex));
		std::vector<rlyrPassesObject::ePassType> thePasses;
		passesObject->CollectPasses(thePasses);

		pfxPostEffectObject* pfxObject = rlyrRenderLayerMgr::GetLayerRenderPfx(
			rlyrRenderLayerMgr::GetLayerName(m_CurrentLayerIndex));
		if (pfxObject)
			pfxObject->ApplyPostEffect();

		for (int i = 0; i < thePasses.size(); i++)
		{
			SetupPass(thePasses[i]);

			//gpxProxyMgr::Update();
			DoRenderCapture();

			if( !data.m_bCaptureMovie.GetValue() )
			{
				full_output = cptrRenderUtil::GetFullOutputFile();
				frame_count = data.m_nCurrentFrame.GetValue();

				rlyrRenderLayerMgr::AddToLayerRenderDataList( rlyrRenderLayerMgr::GetLayerName(m_CurrentLayerIndex),
															  full_output, frame_count );
			}

			// poll for abort.
			if (g3dThreadControl::ShouldRenderThreadAbort())
				break;
		}
	}
	m_CurrentLayerIndex = -1;

	// Make sure that no renderer is considered current after this function
	// Any attempt to use a capture renderer is an error.
	cptrRenderUtil::SetCaptureRenderer(NULL);

}

void cptrRenderThread::RenderAndCaptureBuckets(camCamera& i_Camera, const maTime& i_TimelineTime)
{
	m_pViewer->SetCamera(&i_Camera);

	cptrRenderUtil::BeginQuadrants();

	// store state of timecode in order to restore later
	bool show_timecode = mnmTimeCodeMgr::IsShowTimeCode();

	const int num_quadrants = cptrRenderUtil::GetNumQuadrants();
	for (int i=0; i<num_quadrants; i++)
	{
		// Do Quadrant
		cptrRenderUtil::ConfigureViewport(i, i_Camera);
		RenderScene( i_TimelineTime );
		cptrRenderUtil::CaptureQuadrant(i);
		m_pViewer->Present();

		// only show time code on one quadrant 
		// (i.e. turn it off after first quadrant)
		if (i==0)
		{
			mnmTimeCodeMgr::SetShowTimeCode( false );
		}
	}
	cptrRenderUtil::EndQuadrants();

	// restore camera
	i_Camera.SetSubViewport(-1,1, -1,1);

	if ( show_timecode )
		mnmTimeCodeMgr::SetShowTimeCode( true );
}

//----------------------------------------------------------------------------
// do render in secondary window and capture the frame
//----------------------------------------------------------------------------
void cptrRenderThread::DoRenderCapture()
{
	camCamera *pCamera = camsFollowUtil::GetFollowCamera();

	if ( pCamera->GetStereoType() == STEREO_ANAGLYPH )
	{
		processAnaglyph(pCamera);
		return;
	}

	bool translatingStereoCams = false;
	if ( pCamera->GetStereoType() == STEREO_LEFTCAM ||
		pCamera->GetStereoType() == STEREO_RIGHTCAM || 
		pCamera->GetStereoType() == STEREO_DUALCAM)
	{
		translatingStereoCams = true;
	}

	maPoint3d origPos;
	maPoint3d origTarget;
	maPoint3d origUp;
	maPoint3d origLeft;

	bool choseMspEngine = g3dPrefs::CurrentPrefs().m_RendererEngine == g3dSceneRenderEngineCreate::e_Default ? true : false;

	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	data.m_CurrentStereoMode.SetValue( STEREO_OFF );
	float viewportShift = 0;

	if ( translatingStereoCams )
	{
		data.m_CurrentStereoMode.SetValue( pCamera->GetStereoType() );

		if ( pCamera->GetStereoType() == STEREO_DUALCAM ) 
		{
			data.m_CurrentStereoMode.SetValue( m_bDoneFirstPass ? STEREO_LEFTCAM : STEREO_RIGHTCAM );
		}

		origPos		= pCamera->GetPosition();
		origTarget	= pCamera->GetTarget();
		origUp		= pCamera->GetUp();
		origLeft	= pCamera->GetLeft();
		viewportShift = captStereoUtil::SetStereoCamPos( origLeft, origPos, origTarget, origUp, pCamera, data.m_CurrentStereoMode.GetValue() );
	}

	m_nDoRenderCaptureIteration++;
	g3dPrefs::CurrentPrefs().m_nDoRenderCaptureIteration = m_nDoRenderCaptureIteration;

	//	Motion Samples
	//
// disabling motion blur implementation:
//	if (data.m_nMotionSamplesPerFrame.GetValue() > 1)
//	{
//		if (m_MotionSampler == NULL)
//			m_MotionSampler = new cptrSupersampleMotionSampler(data.m_nMotionSamplesPerFrame.GetValue(), m_pViewer);
//
//		// give the scene a chance to store what it needs
//		// in order to record a sub-frame motion blur sample
//		m_MotionSampler->CaptureMotionSample(*pCamera, tmlnTimeLine::GetValue());
//
//		// if we are ready, then capture a frame!
//		if (m_MotionSampler->DidShutterClose())
//			m_MotionSampler->CaptureFrame(*pCamera);
//
//		return;
//	}

	api3dTargetRendererMgr::RenderTargets( appSimTime::GetTime() );

	//	render out either quadrants or the whole image
	//
	if (cptrRenderUtil::GetCaptureQuadrants() && choseMspEngine)
	{
		// Render large resolution in 4 quadrant renders
		maTime timeline_time = tmlnTimeLine::GetValue();

		if (m_pViewer && cptrRenderUtil::GetCapture())
		{
			RenderAndCaptureBuckets(*pCamera, timeline_time);
		}
	}
	else
	{
		// Render capture window normally
		if (m_pViewer)
		{
			if (data.m_bJitteredSampling.GetValue() && choseMspEngine )
			{
				RenderJitteredFrame(viewportShift);
			}
			else
			{
				maTime timeline_time = tmlnTimeLine::GetValue();
				m_pViewer->SetCamera(pCamera);
				RenderScene(timeline_time);
			}
		}

		//	capture the current frame
		//
		if ( cptrRenderUtil::GetCapture() && choseMspEngine )
		{

			if (data.m_bJitteredSampling.GetValue())
			{
				DBG_ASSERT(m_AccBuf, "Accumulation buffer not initialized");
				cptrRenderUtil::CaptureImage( m_AccBuf->GetAsImage() );
			}
			else
			{
				cptrRenderUtil::CaptureFrame();
			}
			
		}

		//	
		if (m_pViewer && !data.m_bJitteredSampling.GetValue())
		{
			m_pViewer->Present();
		}
	}

	if ( translatingStereoCams )
	{
		pCamera->SetHorizontalFilmOffset( 0 );
		pCamera->LookAt(origPos,origTarget,origUp);
	}
	
}

//----------------------------------------------------------------------------
// RenderScene()
//----------------------------------------------------------------------------
void cptrRenderThread::RenderScene( const maTime& i_timeline )
{
	// Render using MSP rasterizer
	if ( g3dPrefs::CurrentPrefs().m_RendererEngine == g3dSceneRenderEngineCreate::e_Default )
	{
		m_pViewer->Render( i_timeline.AsSeconds() );
	}

	// Render using RenderMan
	else if ( g3dPrefs::CurrentPrefs().m_RendererEngine == g3dSceneRenderEngineCreate::e_RmanPrman )
	{
		captRenderOutputData& data = captRenderOutputDataUtil::Data();

		rprfPrefsObject* prefsObject = rlyrRenderLayerMgr::GetLayerRenderPrefs(
			rlyrRenderLayerMgr::GetLayerName(m_CurrentLayerIndex));
		rmanOptionsData options = GetRmanOptions(prefsObject);

		fsLocator fileName = cptrRenderUtil::GetRibFilename();

		bool err = false;
		std::string err_str;
		rmanMgr::RendermanEntry( m_pViewer->GetAspectRatio() , m_pViewer->GetCamera() , m_CurrentLayerPrefs,
								  fileName, data.m_CurrentScene.GetValue(), data.m_nWidth.GetValue(), data.m_nHeight.GetValue(),
								  data.m_FilterFunc.GetValue(), data.m_FilterWidth.GetValue(), data.m_nCaptureSampling.GetValue(), 
								  options,
								  err, err_str);
		if ( err )
		{
			m_ErrorMessage = err_str;
			DBG_ERROR(m_ErrorMessage);
			m_bErrorState = true;
			m_bAppExit  = false;
		}
	}

	// Render using Mental Ray
	else if ( g3dPrefs::CurrentPrefs().m_RendererEngine == g3dSceneRenderEngineCreate::e_MentalRay )
	{
		captRenderOutputData& data = captRenderOutputDataUtil::Data();

		rprfPrefsObject* prefsObject = rlyrRenderLayerMgr::GetLayerRenderPrefs(
			rlyrRenderLayerMgr::GetLayerName(m_CurrentLayerIndex));

		fsLocator fileName = cptrRenderUtil::GetMiFilename();
		mrayOptionsData options = GetMRayOptions(prefsObject);

		bool err = false;
		std::string err_str;
		mrayMgr::MentalRayEntry( m_pViewer->GetAspectRatio() , m_pViewer->GetCamera() , m_CurrentLayerPrefs,
								  fileName, data.m_CurrentScene.GetValue(), data.m_nWidth.GetValue(), data.m_nHeight.GetValue(),
								  data.m_FilterFunc.GetValue(), data.m_FilterWidth.GetValue(), data.m_nCaptureSampling.GetValue(),
								  options,
								  err, err_str);
		if ( err )
		{
			m_ErrorMessage = err_str;
			DBG_ERROR(m_ErrorMessage);
			m_bErrorState = true;
			m_bAppExit  = false;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrRenderThread::processAnaglyph(camCamera *pCamera) 
{
	if ( g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_Depth )
	{
		m_ErrorMessage = "Anaglyph rendering of depth buffer is not supported.";
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
		return;
	}

	m_nDoRenderCaptureIteration++;
	g3dPrefs::CurrentPrefs().m_nDoRenderCaptureIteration = m_nDoRenderCaptureIteration;

	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// Dont support FP output yet
	if ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"HDR",3) == 0 ||
		 strncmp(data.m_CaptureFormat.GetValue().c_str(),"PFM",3) == 0 ||
		 strncmp(data.m_CaptureFormat.GetValue().c_str(),"EXR",3) == 0 )
	{
		m_ErrorMessage = "Anaglyph rendering into floating point file formats is not currently supported.";
		DBG_ERROR(m_ErrorMessage);
		m_bErrorState = true;
	}

	bool choseMspEngine = g3dPrefs::CurrentPrefs().m_RendererEngine == g3dSceneRenderEngineCreate::e_Default ? true : false;

	api3dTargetRendererMgr::RenderTargets( appSimTime::GetTime() );

	// Init masks to white
	maFloatRGBA left_eye_mask = maFloatRGBA(1,1,1,1);
	maFloatRGBA right_eye_mask = maFloatRGBA(1,1,1,1);

	// Set color filters
	captStereoUtil::SetLeftColorFilter(pCamera->GetStereoFilterColor(), &left_eye_mask);
	captStereoUtil::SetRightColorFilter(pCamera->GetStereoFilterColor(), &right_eye_mask);

	// Save original camera info
	maPoint3d origPos	 = pCamera->GetPosition();
	maPoint3d origTarget = pCamera->GetTarget();
	maPoint3d origUp	 = pCamera->GetUp();
	maPoint3d origLeft	 = pCamera->GetLeft();

	g2dPFD::PixelFormat rawBufferPFD = m_pViewer->GetRenderer()->GetRawBufferPFD();
	rawBufferPFD = g2dPFD::e_Color;
	int numBytesPerPixel = captStereoUtil::GetNumBytesPerPixel( rawBufferPFD );
	g2dPFD pfd(rawBufferPFD, numBytesPerPixel*8);
	if ( rawBufferPFD == g2dPFD::e_Color )
	{
		pfd.Set(0,8, 8,8, 16,8, 24,8, 32);
	}

	g2dImage* pAccum = NULL;
	g2dImage* pAnaglyph = g2dImageCreate::Make(data.m_nWidth.GetValue(),data.m_nHeight.GetValue(),
									 pfd,g2dImage::e_SystemMemory);
	g2dImage* pLeft = g2dImageCreate::Make(data.m_nWidth.GetValue(),data.m_nHeight.GetValue(),
									 pfd,g2dImage::e_SystemMemory);
	g2dImage* pRight = g2dImageCreate::Make(data.m_nWidth.GetValue(),data.m_nHeight.GetValue(),
									 pfd,g2dImage::e_SystemMemory);

	// Loop once for left eye and once for right eye
	int currentEye;
	for ( currentEye = STEREO_LEFTCAM ; currentEye <= STEREO_RIGHTCAM ; currentEye++ )
	{		
		data.m_CurrentStereoMode.SetValue( currentEye );

		// Translate camera
		float viewportOffset = captStereoUtil::SetStereoCamPos( origLeft,origPos,origTarget,origUp,pCamera,currentEye );

		// Render from translated camera
		if (cptrRenderUtil::GetCaptureQuadrants())
		{
			// Render large resolution in 4 quadrant renders
			maTime timeline_time = tmlnTimeLine::GetValue();

			if (m_pViewer && cptrRenderUtil::GetCapture())
			{
				m_pViewer->SetCamera(pCamera);

				cptrRenderUtil::BeginQuadrants();

				// store state of timecode in order to restore later
				bool show_timecode = mnmTimeCodeMgr::IsShowTimeCode();

				const int num_quadrants = cptrRenderUtil::GetNumQuadrants();
				for (int i=0; i<num_quadrants; i++)
				{
					// Do Quadrant
					cptrRenderUtil::ConfigureViewport(i, *pCamera);
					RenderScene(timeline_time);
					cptrRenderUtil::CaptureQuadrant(i);
					m_pViewer->Present();

					// only show time code on one quadrant 
					// (i.e. turn it off after first quadrant)
					if (i==0)
					{
						mnmTimeCodeMgr::SetShowTimeCode( false );
					}
				}
				//cptrRenderUtil::EndQuadrants();
				if ( currentEye == STEREO_LEFTCAM )
				{
					cptrRenderUtil::EndAnaglyphQuadrants(pLeft,left_eye_mask,rawBufferPFD);
				}
				else
				{
					cptrRenderUtil::EndAnaglyphQuadrants(pRight,right_eye_mask,rawBufferPFD);
				}				

				// restore camera
				pCamera->SetSubViewport(-1,1, -1,1);

				if ( show_timecode )
					mnmTimeCodeMgr::SetShowTimeCode( true );
			}
		}
		else
		{
			// Render from translated camera
			if (m_pViewer)
			{
				if (data.m_bJitteredSampling.GetValue() && choseMspEngine)
				{
					RenderJitteredFrame(viewportOffset);
				}
				else
				{
					maTime timeline_time = tmlnTimeLine::GetValue();
					m_pViewer->SetCamera(pCamera);
					RenderScene(timeline_time);
				}
			}

			// Send render to pImg
			if ( cptrRenderUtil::GetCapture() && choseMspEngine )
			{
				if (data.m_bJitteredSampling.GetValue())
				{
					DBG_ASSERT(m_AccBuf, "Accumulation buffer not initialized");
					pAccum = m_AccBuf->GetAsImage();
					if ( currentEye == STEREO_LEFTCAM )
					{
						pLeft->CopyImage(*pAccum);
					}
					else
					{
						pRight->CopyImage(*pAccum);
					}
				}
				else
				{
					g2dScreenCaptureUtil::CaptureWindowToImage(*(m_pViewer->GetWindow()), (currentEye == STEREO_LEFTCAM ? pLeft : pRight));
				}
			}

			if ( currentEye == STEREO_LEFTCAM )
			{
				captStereoUtil::ApplyAnaglyphColorFilter(pLeft, left_eye_mask, rawBufferPFD);
			}
			else
			{
				captStereoUtil::ApplyAnaglyphColorFilter(pRight, right_eye_mask, rawBufferPFD);
			}
		}

	} // End of for() loop

	captStereoUtil::MergeLeftRight(pAnaglyph,pLeft,pRight,rawBufferPFD);
	
	// Save anaglyph to disk
	data.m_CurrentStereoMode.SetValue( STEREO_OFF );
	if ( choseMspEngine ) cptrRenderUtil::CaptureImage(pAnaglyph);
	
	// put image to backbuffer
	m_pViewer->GetWindow()->MakeCurrent();
	m_pViewer->GetWindow()->DrawImage(0,0, *pAnaglyph);
	if (m_pViewer)
	{
		m_pViewer->Present();
	}

	// Clean up
	delete pAnaglyph;
	pAnaglyph = NULL;

	// Restore original camera position
	pCamera->SetHorizontalFilmOffset( 0 );
	pCamera->LookAt(origPos,origTarget,origUp);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool cptrRenderThread::RenderJitteredFrame(float i_StereoOffset)
{
	g2dPFD pfd = cptrRenderUtil::GetCapturePFD();

	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	// first time (lazy) init the buffer
	if (m_AccBuf == NULL)
		m_AccBuf = new cptrAccumulationBuffer(data.m_nWidth.GetValue(), data.m_nHeight.GetValue(), data.m_nCaptureSampling.GetValue()*data.m_nCaptureSampling.GetValue(), pfd);
	else
		m_AccBuf->Clear();

	maTime timeline_time = tmlnTimeLine::GetValue();
	camCamera *pCamera = camsFollowUtil::GetFollowCamera();

	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	// draw each jittered sample,
	// and then put the accum buffer back in the backbuffer,
	// to be captured?
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	pCamera->SetHorizontalFilmOffset( i_StereoOffset );
	bool choseMspEngine = g3dPrefs::CurrentPrefs().m_RendererEngine == g3dSceneRenderEngineCreate::e_Default ? true : false;
	int i;
	for (i = 0; i < data.m_nCaptureSampling.GetValue()*data.m_nCaptureSampling.GetValue(); i++)
	{
		// See if we should abort the render thread before doing the next target
		if (g3dThreadControl::ShouldRenderThreadAbort())
			break;

		float dx = (cptrRenderUtil::GetSamplePos(i).GetX() - 0.5f)*2.0f/(float)data.m_nWidth.GetValue();
		float dy = (cptrRenderUtil::GetSamplePos(i).GetY() - 0.5f)*2.0f/(float)data.m_nHeight.GetValue();

		pCamera->SetSubViewport(-1+dx, 1+dx, -1+dy, 1+dy);

		m_pViewer->SetCamera(pCamera);
		RenderScene( timeline_time );
		pCamera->SetSubViewport(-1, 1, -1, 1);

		g2dImage* pImg = NULL;
		cptrRenderUtil::CaptureFrameToImage(pImg);
		DBG_ASSERT(pImg != NULL, "failed to capture frame to image");
		data.m_nWidth.SetValue(pImg->GetWidth());
		data.m_nHeight.SetValue(pImg->GetHeight());
		m_AccBuf->Add(pImg, cptrRenderUtil::GetSamplePos(i).GetZ());
		delete pImg;

		// we can now draw the frame sample into the window.
		if ( pCamera->GetStereoType() == STEREO_OFF )
			m_pViewer->Present();
	}
	if ( choseMspEngine ) m_AccBuf->Capture();

	return choseMspEngine;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrRenderThread::SetupRenderLayer(int i_CurrentLayerIndex)
{
	//set layer visibility prefs for each object in the scene
	rlyrRenderLayerMgr::SetupLayerObjectsVisible(i_CurrentLayerIndex);

	bool capture_tonemapped = rlyrRenderLayerMgr::GetLayerRenderPrefs(
		rlyrRenderLayerMgr::GetLayerName(i_CurrentLayerIndex))->m_Data.m_bCaptureToneMapped.GetValue();

	cptrRenderUtil::SetCaptureToneMapped(capture_tonemapped);

	//update the global capture data with the values from the current render layer
	rlyrRenderLayerMgr::ApplyLayerCaptureOptions( i_CurrentLayerIndex );

	captRenderOutputDataUtil::EnableShadows( rlyrRenderLayerMgr::GetLayerActualData(i_CurrentLayerIndex).m_bMultipassOn );
	captRenderOutputDataUtil::EnableWireframe( rlyrRenderLayerMgr::GetLayerActualData(i_CurrentLayerIndex).m_bRenderWireframe );
}

//--------------------------------------------------------------------
//	get the next render layer index
//--------------------------------------------------------------------
int cptrRenderThread::get_next_layer_index(int i_CurrentLayerIndex)
{
	i_CurrentLayerIndex = rlyrRenderLayerMgr::GetNextLayerIndex( i_CurrentLayerIndex );
	return i_CurrentLayerIndex;
}


//----------------------------------------------------------------------------
// GetRmanOptions()
//----------------------------------------------------------------------------
rmanOptionsData cptrRenderThread::GetRmanOptions(rprfPrefsObject* prefsObject)
{
	rmanOptionsData options;

	bool allowReflections = false;
	if ( m_CurrentPass == rlyrPassesObject::e_Beauty || m_CurrentPass == rlyrPassesObject::e_ReflectionsOnly )
	{
		allowReflections = true;
	}

	options.m_bRmanRewriteAssets = prefsObject->m_Data.m_bRmanCacheTextures.GetValue();
	options.m_RmanOutType = prefsObject->m_Data.m_RmanOutType.GetValue();
	options.m_RmanShadingRate = prefsObject->m_Data.m_RmanShadingRate.GetValue();
	options.m_RmanBucketOrder = prefsObject->m_Data.m_RmanBucketOrder.GetValue();
	options.m_RmanBucketSize = prefsObject->m_Data.m_RmanBucketSize.GetValue();
	options.m_RmanRayDepth = prefsObject->m_Data.m_RmanRayDepth.GetValue();	
	options.m_RmanNumCores = prefsObject->m_Data.m_RmanNumCores.GetValue();
	options.m_RmanTexMemory = prefsObject->m_Data.m_RmanTexMemory.GetValue();
	options.m_bRmanDisableWarnings = prefsObject->m_Data.m_bRmanDisableWarnings.GetValue();
	options.m_bRmanReflEnable = prefsObject->m_Data.m_bRmanReflEnable.GetValue() & allowReflections;
	options.m_RmanReflType = prefsObject->m_Data.m_RmanReflType.GetValue();
	options.m_bRmanShadowEnable = prefsObject->m_Data.m_bRmanShadowEnable.GetValue();
	options.m_RmanShadowType = prefsObject->m_Data.m_RmanShadowType.GetValue();
	options.m_bRmanAOEnable = prefsObject->m_Data.m_bRmanAOEnable.GetValue();
	options.m_RmanAOsamples = prefsObject->m_Data.m_RmanAOsamples.GetValue();
	options.m_RmanAOMaxVariation = prefsObject->m_Data.m_RmanAOMaxVariation.GetValue();
	options.m_bRmanGIEnable = prefsObject->m_Data.m_bRmanGIEnable.GetValue();
	options.m_RmanGIsamples = prefsObject->m_Data.m_RmanGIsamples.GetValue();
	options.m_RmanGIMaxVariation = prefsObject->m_Data.m_RmanGIMaxVariation.GetValue();
	options.m_bTonemapEnable = prefsObject->m_Data.m_bRmanTonemapEnable.GetValue();
	return options;
}

//----------------------------------------------------------------------------
// GetMRayOptions()
//----------------------------------------------------------------------------
mrayOptionsData cptrRenderThread::GetMRayOptions(rprfPrefsObject* prefsObject)
{
	mrayOptionsData options;

	bool allowReflections = false;
	if ( m_CurrentPass == rlyrPassesObject::e_Beauty || m_CurrentPass == rlyrPassesObject::e_ReflectionsOnly )
	{
		allowReflections = true;
	}

	options.m_Verbosity = prefsObject->m_Data.m_MrayVerbosity.GetValue();
	options.m_NumReflBounces = prefsObject->m_Data.m_MrayNumReflBounces.GetValue();
	options.m_NumRefrBounces = prefsObject->m_Data.m_MrayNumRefrBounces.GetValue();
	options.m_MrayMaxTraceDepth = prefsObject->m_Data.m_MrayMaxTraceDepth.GetValue();
	options.m_bAO = prefsObject->m_Data.m_bMrayAO.GetValue();
	options.m_AOSamples = prefsObject->m_Data.m_MrayAOSamples.GetValue();
	options.m_bFinalGather = prefsObject->m_Data.m_bMrayFinalGather.GetValue();
	//options.m_bFGBlur = prefsObject->m_Data.m_bMrayFGBlur.GetValue();
	options.m_FGNDiffuse = prefsObject->m_Data.m_MrayFGNDiffuse.GetValue();
	options.m_FGNRefl = prefsObject->m_Data.m_MrayFGNRefl.GetValue();
	options.m_FGNRefr = prefsObject->m_Data.m_MrayFGNRefr.GetValue();
	options.m_FGNRays = prefsObject->m_Data.m_MrayFGNRays.GetValue();
	options.m_FGColor = prefsObject->m_Data.m_MrayFGColor.GetValue();
	options.m_OutputFormat = prefsObject->m_Data.m_MrayOutputFormat.GetValue();
	options.m_NumThreads = prefsObject->m_Data.m_MrayNumThreads.GetValue();
	options.m_MemoryLimit = prefsObject->m_Data.m_MrayMemoryLimit.GetValue();
	options.m_bEnableReflections = prefsObject->m_Data.m_bMrayEnableReflections.GetValue() & allowReflections;
	options.m_bEnableShadows = prefsObject->m_Data.m_bMrayEnableShadows.GetValue();
	options.m_MrayShadowType = prefsObject->m_Data.m_MrayShadowType.GetValue();
	options.m_bRewriteAssets = prefsObject->m_Data.m_bMrayRewriteAssets.GetValue();
	options.m_VerbosityLevel = prefsObject->m_Data.m_MrayVerbosityLevel.GetValue();
	options.m_bFGMapEnable = prefsObject->m_Data.m_bMrayFGMapEnable.GetValue();
	options.m_FGMapRebuild = prefsObject->m_Data.m_MrayFGMapRebuild.GetValue();
	options.m_FGMapPath = prefsObject->m_Data.m_MrayFGMapPath.GetValue();
	options.m_ReflSamples = prefsObject->m_Data.m_MrayReflSamples.GetValue();
	options.m_bOverrideMSPSampling = prefsObject->m_Data.m_bMrayOverrideMSPSampling.GetValue();
	options.m_MinCaptureSamples = prefsObject->m_Data.m_MrayMinCaptureSamples.GetValue();
	options.m_MaxCaptureSamples = prefsObject->m_Data.m_MrayMaxCaptureSamples.GetValue();
	options.m_AAContrast = prefsObject->m_Data.m_MRayAAContrast.GetValue();
	options.m_bProgressive = prefsObject->m_Data.m_bMrayProgressive.GetValue();
	options.m_bTonemapEnable = prefsObject->m_Data.m_bMRayTonemapEnable.GetValue();
	options.m_bEnableIBL = prefsObject->m_Data.m_bEnableIBL.GetValue();
	options.m_IBLQuality = prefsObject->m_Data.m_IBLQuality.GetValue();
	options.m_IBLMapRes = prefsObject->m_Data.m_IBLMapRes.GetValue();
	options.m_IBLScale = prefsObject->m_Data.m_IBLScale.GetValue();
	options.m_IBLSampleNum = prefsObject->m_Data.m_IBLSampleNum.GetValue();
	options.m_bIgnoreBadTex = prefsObject->m_Data.m_bMrayIgnoreBadTex.GetValue();
	options.m_bDisplayPreviewer = prefsObject->m_Data.m_bMrayDisplayPreview.GetValue();
	options.m_ProgSubsamplingSize = prefsObject->m_Data.m_MrayProgSubsamplingSize.GetValue();
	options.m_ProgSubsamplingMode = prefsObject->m_Data.m_MrayProgSubsamplingMode.GetValue();
	options.m_ProgSubsamplingPattern = prefsObject->m_Data.m_MrayProgSubsamplingPattern.GetValue();
	options.m_ProgMinSamples = prefsObject->m_Data.m_MrayProgMinSamples.GetValue();
	options.m_ProgMaxSamples = prefsObject->m_Data.m_MrayProgMaxSamples.GetValue();
	options.m_ProgMaxTime = prefsObject->m_Data.m_MrayProgMaxTime.GetValue();
	options.m_ProgErrorThreshold = prefsObject->m_Data.m_MrayProgErrorThreshold.GetValue();

	return options;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrRenderThread::SetupPassBuffers()
{
	rprfPrefsObject* prefsObject = rlyrRenderLayerMgr::GetLayerRenderPrefs(
			rlyrRenderLayerMgr::GetLayerName(m_CurrentLayerIndex));
	
	//captPassBufferFlags flags;
	//flags.bPassBufferCapturing = true;
	//flags.bPassBufferAO = prefsObject->m_Data.m_bRPF_AO.GetValue();
	//flags.bPassBufferGI = prefsObject->m_Data.m_bRPF_GI.GetValue();
	//flags.bPassBufferRefl = prefsObject->m_Data.m_bRPF_Refl.GetValue();
	//flags.bPassBufferShadowMask = prefsObject->m_Data.m_bRPF_ShadowMask.GetValue();
	//flags.bPassBufferBeauty = prefsObject->m_Data.m_bRPF_Beauty.GetValue();

	//g3dPassBuffers::SetCaptPassBufferFlags( flags );

	camCamera &render_camera = cam3dMgr::GetCamera();

	// Update camera values here
	camPassBuffersData passBuffersData;
	render_camera.GetPassBuffersParams(passBuffersData);

	if ( !prefsObject->m_Data.m_bRPF_AO.GetValue() )
	{
		passBuffersData.m_AOBuffer = NULL;
	}
	if ( !prefsObject->m_Data.m_bRPF_GI.GetValue() )
	{
		passBuffersData.m_GIBuffer = NULL;
	}
	if ( !prefsObject->m_Data.m_bRPF_Refl.GetValue() )
	{
		passBuffersData.m_ReflBuffer = NULL;
	}
	if ( !prefsObject->m_Data.m_bRPF_ShadowMask.GetValue() )
	{
		passBuffersData.m_ShadowMaskBuffer = NULL;
	}
	if ( !prefsObject->m_Data.m_bRPF_Beauty.GetValue() )
	{
		passBuffersData.m_BeautyBuffer = NULL;
	}

	g3dPassBuffers::SetDoingFileRefl(prefsObject->m_Data.m_bRPF_Refl.GetValue() && g3dPassBuffers::GetDoingFileRefl());

	// pass settings back into cam
	render_camera.SetPassBuffersParams(passBuffersData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrRenderThread::SetupPass(rlyrPassesObject::ePassType i_CurrentPass)
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	if (m_pViewer != NULL)
	{
		delete m_pViewer;
		m_pViewer = NULL;
	}

	camCamera &render_camera = cam3dMgr::GetCamera();

	m_CurrentPass = i_CurrentPass;

	// This isn't necessary because the viewer will
	// do it before rendering when we set SetMatchAspectToWindow(true)
	//render_camera.SetAspect(data.m_nWidth, data.m_nHeight);

	// Inherit prefs from the render layer.
	// save a copy of the current layer prefs here.
	m_CurrentLayerPrefs = rlyrRenderLayerMgr::GetLayerActualData(m_CurrentLayerIndex);

	m_CurrentLayerPrefs.m_bHDRAA = data.m_bHardwareAA.GetValue();
	
	// Setup the renderer and prefs for rendering this pass.
	rlyrPassesObject::AdjustPrefs(i_CurrentPass, m_CurrentLayerPrefs);

	g3dSceneRenderer* pRenderer = NULL;
	switch (i_CurrentPass)
	{
	case rlyrPassesObject::e_Beauty:
	case rlyrPassesObject::e_Diffuse:
	case rlyrPassesObject::e_Specular:
	case rlyrPassesObject::e_Bloom:
	case rlyrPassesObject::e_Star:
	case rlyrPassesObject::e_CameraDOF:
	case rlyrPassesObject::e_Preview:
	case rlyrPassesObject::e_DiffEnv:
	case rlyrPassesObject::e_DiffLit:
	case rlyrPassesObject::e_SpecEnv:
	case rlyrPassesObject::e_SpecLit:
	case rlyrPassesObject::e_Emissive:
		pRenderer = m_Renderers[e_HDR];
		break;
	case rlyrPassesObject::e_AOOnly:
		pRenderer = m_Renderers[e_AmbientOcclusion];
		break;
	case rlyrPassesObject::e_Depth:
		pRenderer = m_Renderers[e_Depth];
		break;
	case rlyrPassesObject::e_ShadowMask:
		pRenderer = m_Renderers[e_ShadowMask];
		break;
	case rlyrPassesObject::e_IlluminationOnly:
		pRenderer = m_Renderers[e_IlluminationOnly];
		break;
	case rlyrPassesObject::e_Normals:
		pRenderer = m_Renderers[e_Normals];
		break;
	case rlyrPassesObject::e_DirtyMatte:
		pRenderer = m_Renderers[e_DirtyMatte];
		break;
	case rlyrPassesObject::e_Wireframe:
		pRenderer = m_Renderers[e_Wireframe];
		break;
	case rlyrPassesObject::e_Materials:
		pRenderer = m_Renderers[e_Materials];
		break;
	case rlyrPassesObject::e_ReflectionsOnly:
		pRenderer = m_Renderers[e_ReflectionOnly];
		break;
	case rlyrPassesObject::e_Velocity:
		pRenderer = m_Renderers[e_VelocityMap];
		break;
	case rlyrPassesObject::e_GlobalIllumination:
		pRenderer = m_Renderers[e_GlobalIllumination];
		break;
	case rlyrPassesObject::e_Glow:
		pRenderer = m_Renderers[e_Glow];
		break;
	default:
		pRenderer = NULL;//m_Renderers[e_HDR];
		break;
	}

	m_pViewer = new g3dViewer(m_pWindow, pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );
	m_pViewer->SetCamera(&render_camera);
	m_pViewer->SetAspectRatio(data.m_PixelAspect.GetValue());
	m_pViewer->SetScene(api3dScene::GetScene());
	//m_pViewer->SetMatchAspectToWindow(true); // camera has this flag now, and it is set to true for scripted cameras

	mnmApp::EnableRender(false);

	//this->SetViewer(m_pViewer);
	cptrRenderUtil::SetCaptureRenderer(pRenderer);

	itString layer_name;
	nameString layer_desc = rlyrRenderLayerMgr::GetLayerName( m_CurrentLayerIndex );
	layer_name = layer_desc.GetString().c_str();

	itString pass_name = captRenderOutputDataUtil::GetRenderPassName(i_CurrentPass);

	data.m_RenderPosLayer.SetValue( layer_name );			// for saving render position

	if ( data.m_bUseLayerNameInFilename.GetValue() || data.m_bUseLayerNameAsDirectory.GetValue() )
	{
		captRenderOutputDataUtil::SetCurrentLayer( layer_name );
	}
	if ( data.m_bUseRenderPassInFilename.GetValue() || data.m_bUseRenderPassAsDirectory.GetValue() )
	{
		captRenderOutputDataUtil::SetCurrentRenderPass( pass_name );
	}
	// set up folder name in data util
	captRenderOutputDataUtil::GenerateDirectoryName();
	CreateOutputDir(data.m_OutputDirectory.GetValue());

	//rlyrRenderLayerMgr::ApplyLayerPrefs(i_CurrentLayerIndex);
	// make these prefs current (should be sent to the viewport as local viewer data)
	g3dPrefs::SetPrefs(&m_CurrentLayerPrefs);

	// consequence of having applied new render prefs:
	rndrPrefsUtil::SetMultipassRendering(g3dPrefs::CurrentPrefs().m_bMultipassOn);
	mnmApp::SetRenderer(g3dPrefs::CurrentPrefs().m_RendererType);
	// just in case we can free up reflection maps due to render prefs change:
	mtrlScriptObject::RecreateRenderTargets();
	// the mtrlScriptObject rendertargets 
	// may have been rebuilt for renderer change.  
	// So now it is safe to add them to this viewer.
	mtrlScriptObject::AddTargetsToViewer(m_pViewer);

	SetupPassBuffers();

}

void cptrRenderThread::CreateOutputDir(const fsLocator& i_OutputDirName)
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	//	check if the directory exists, if not, create it.
	//
	if ( !fsFileUtil::DirectoryExists(i_OutputDirName) )
	{
		try
		{
			fsFileUtil::CreateDirectory(i_OutputDirName);
		}
		catch ( const fsDirectoryDoesntExistX& /*i_Ex*/ )
		{
			std::string dir;
			fsFileUtil::LocatorToANSIFilename(i_OutputDirName, dir);
			std::string msg = "Error trying to create Output directory: " + dir;
			DBG_ERROR(msg);
			if(data.m_bBatchMode.GetValue()
				&& data.m_bBatchSkipDialog.GetValue())
			{
				cptrRenderStatsDialogUtil::AddStatsMessage(itString(msg.c_str()));
				cptrRenderStatsDialogUtil::Newline();
			}
			else
			{
				guiMessageBox::Show(msg.c_str(), "Create Directory Error", guiMessageBox::e_OKOnly);
			}		
			throw;
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Error trying to create Output directory, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			if(data.m_bBatchMode.GetValue()
				&& data.m_bBatchSkipDialog.GetValue())
			{
				cptrRenderStatsDialogUtil::AddStatsMessage(itString(msg.c_str()));
				cptrRenderStatsDialogUtil::Newline();
			}
			else
			{
				guiMessageBox::Show(msg.c_str(), "Create Directory Error", guiMessageBox::e_OKOnly);
			}		
			throw;
		}	
	}
}
