/****************************************************************************\
**	mainInitGraphics.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainInitGraphics.hpp"
#include "MainApp/mnmApp.hpp"
#ifdef MSP_GUI_QT
#include "MainApp/qtGUI/qtMainForm.hpp"
#else
#include "MainApp/wxGUI/wxMainForm.hpp"
#endif
#ifdef MSP_GUI_QT
#include "MainApp/qtGUI/qtMainDropTarget.hpp"
#else
#include "MainApp/wxGUI/wxMainDropTarget.hpp"
#endif

#ifdef MSP_GUI_QT
#include "Features/RenderPanels/qtGUI/rpnRenderPanel.hpp"
#else
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"
#endif
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/ptm/ptmControlFactoryTimeline.hpp"

#include "AudioDS/AudioDSLayer.hpp"
#include "AudioDS/Sn/snExceptionX.hpp"
#include "AudioDS/Sn/snSoundManager.hpp"
#include "AudioDS/Sn/snSoundSystem.hpp"
#include "Core/Dbg/dbgSystemInfo.hpp"
#include "Graphics/G2d/g2dExceptionX.hpp"
#include "Graphics/G2d/g2dFontUtil.hpp"
#include "Graphics/G3d/g3dExceptionX.hpp"
#include "Graphics/G3d/g3dGfxOptions.hpp"
#include "Graphics/G3d/g3dProfiler.hpp"
#include "Graphics/G3d/g3dSystem.hpp"

#include <iostream>
#include "Graphics/GraphicsLayer.hpp"
#include "ImportExport/ImportExportLayer.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/In/inPackage.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmErrorCodes.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmVJoystick.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/pqt/pqtControlFactoryBase.hpp"
#else
#include "ToolUIWx/pwx/pwxControlFactoryBase.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/pqt/pqtControlFactoryCustom.hpp"
#else
#include "ToolUIWx/pwx/pwxControlFactoryCustom.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/pqt/pqtControlMgr.hpp"
#else
#include "ToolUIWx/pwx/pwxControlMgr.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtInputFeed.hpp"
#else
#include "ToolUIWx/twx/twxInputFeed.hpp"
#endif
#include "GraphicsDX11/GraphicsDX11Layer.hpp"
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#include "MainApp/mainGfxBackend.hpp"


//============================================================================
//============================================================================
namespace
{
	g3dGfxOptions l_GfxOptions;

	//--------------------------------------------------------------------
	// gfx_layer - the graphics layers have the same calls; chosen_layer
	// is the one -gfx asked for. DX11's Init can't return failure (it
	// throws the g2d exceptions caught below instead).
	//--------------------------------------------------------------------
	struct gfx_layer
	{
		bool (*Init)(void* i_hWnd, const g3dGfxOptions& i_Options);
		void (*InitGraphics)();
		void (*CleanUpGraphics)();
		void (*CleanUp)();
		g2dSystem* (*GetSystem2D)();
		g3dSystem* (*GetSystem3D)();
	};

	bool init_dx11(void* i_hWnd, const g3dGfxOptions& /*i_Options*/)
	{
		GraphicsDX11Layer::Init(i_hWnd);
		return true;
	}

	const gfx_layer l_DX11Layer = { init_dx11, GraphicsDX11Layer::InitGraphics, GraphicsDX11Layer::CleanUpGraphics,
									GraphicsDX11Layer::CleanUp, GraphicsDX11Layer::GetSystem2D, GraphicsDX11Layer::GetSystem3D };
	const gfx_layer l_VulkanLayer = { GraphicsVulkanLayer::Init, GraphicsVulkanLayer::InitGraphics, GraphicsVulkanLayer::CleanUpGraphics,
									  GraphicsVulkanLayer::CleanUp, GraphicsVulkanLayer::GetSystem2D, GraphicsVulkanLayer::GetSystem3D };

	bool l_bLayerInitialized = false;

	const gfx_layer& chosen_layer()
	{
		return l_GfxOptions.m_Backend == g3dGfxOptions::e_Vulkan ? l_VulkanLayer : l_DX11Layer;
	}

	// The chosen layer's Init returned false (GraphicsVulkanLayer::GetInitError says why)
	struct gfx_layer_init_failed {};

	//--------------------------------------------------------------------
	// report - logs a line; RenderNode also prints it to stderr, where
	// gfxcompare reads the backend and validation lines.
	//--------------------------------------------------------------------
	void report(const std::string& i_Line)
	{
		DBG_LOG(i_Line.c_str());
#ifdef BATCH_MODE
		std::cerr << i_Line << std::endl;
#endif
	}

	//--------------------------------------------------------------------
	// start_profiler - -gfxProfile <file.csv> starts the graphics
	// profiler, if this build has the profiling markers.
	//--------------------------------------------------------------------
	void start_profiler(const char* i_Backend)
	{
		if (l_GfxOptions.m_ProfileFile.empty())
		{
			return;
		}
#ifdef MSP_GFX_PROFILING
		g3dProfiler::Start(l_GfxOptions.m_ProfileFile, i_Backend);
		DBG_LOG("Graphics profiling to " << l_GfxOptions.m_ProfileFile.c_str());
#else
		DBG_LOG("-gfxProfile ignored: this build has no profiling markers (build with MspGfxProfiling=true)");
	#ifdef BATCH_MODE
		std::cerr << "-gfxProfile ignored: this build has no profiling markers (build with MspGfxProfiling=true)" << std::endl;
	#endif
#endif
	}
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mainInitGraphics::SetGfxOptions(const g3dGfxOptions& i_Options)
{
	l_GfxOptions = i_Options;
}

const g3dGfxOptions& mainInitGraphics::GetGfxOptions()
{
	return l_GfxOptions;
}


//--------------------------------------------------------------------
// Initializes library packages
//--------------------------------------------------------------------
#ifdef USE_WXWIDGETS
mainInitGraphics::mainInitGraphics(wxMainForm *i_pMainFrame)
#elif defined(USE_QT)
mainInitGraphics::mainInitGraphics(qtMainForm *i_pMainFrame)
#else
mainInitGraphics::mainInitGraphics(int i_AppWidth,
								   int i_AppHeight,
								   int i_ViewOffsetX,
								   int i_ViewOffsetY )
#endif
:	m_pApp(NULL),
	m_bSuccessInit(false)
{	
	// needed before initialize_render()
	m_pApp = new mnmApp();
	mnmApp::SetActive( false );

	// initialize everything else
	//
	try 
	{
		GraphicsLayer::Init();

		const gfx_layer& layer = chosen_layer();
#ifdef USE_WXWIDGETS
		void* hWnd = i_pMainFrame->GetRenderWindow()->GetHandle();
#elif defined(USE_QT)
		void* hWnd = i_pMainFrame->GetRenderWindow()->GetHandle();
#else
		void* hWnd = appApplication::GetMainWindowHandle();
#endif
		if (!layer.Init(hWnd, l_GfxOptions))
		{
			throw gfx_layer_init_failed();
		}
		l_bLayerInitialized = true;
		report(mainGfxBackend::BackendLine(l_GfxOptions.m_Backend, layer.GetSystem3D()->GetVideoAdapterName()));

		GraphicsLayer::InitGraphics(layer.GetSystem2D(), layer.GetSystem3D());
		layer.InitGraphics();
		start_profiler(mainGfxBackend::Name(l_GfxOptions.m_Backend));

		fsLocator font_loc;
		font_loc.Push( gfPaths::GetPath(mnmPaths::e_ExeArt) ); 
		font_loc.Push( "font-lucd00.png" );
		g2dFontUtil::SetGlobalFontBitmap(font_loc);

		AudioDSLayer::Init();
		ImportExportLayer::Init();

		cptrRenderUtil::Initialize();	// no deinitialize
		tmlnTimeLine::Initialize();

		dbgSystemInfo::logSystemInfo();

		//	set-up rendering window
		//
#ifdef USE_WXWIDGETS
		//wxSize size = i_pMainFrame->GetSize();
		//wxSize client_size = i_pMainFrame->GetClientSize();
		wxRect rect = i_pMainFrame->GetRenderWindow()->GetRect();
		m_pApp->InitializeRender( (void*)i_pMainFrame->GetRenderWindow()->GetHandle(),
			(void*)i_pMainFrame->GetRenderWindow()->GetHandle(),
			rect.GetWidth(), rect.GetHeight());

		tma3dScreenUtil::SetWindowSize( maPoint2d( rect.GetLeft(), rect.GetTop() ),
			maPoint2d( rect.GetWidth(), rect.GetHeight() ) );

		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			i_pMainFrame->GetRenderWindow()->SetRenderView(m_pApp->GetRenderView(0));
			i_pMainFrame->GetRenderWindow()->SetDropTarget(new wxMainDropTarget(m_pApp->GetRenderView(0))); // for drag & drop
			inDeviceMgr::SetWindowHandle( i_pMainFrame->GetRenderPane(0)->GetHandle() );
		}
#elif defined(USE_QT)
		// The render panels have their own native windows, sized in
		// physical pixels
		rpnRenderPanel* pRenderWindow = i_pMainFrame->GetRenderWindow();
		const int render_width = pRenderWindow->PhysicalWidth();
		const int render_height = pRenderWindow->PhysicalHeight();
		m_pApp->InitializeRender( (void*)pRenderWindow->GetHandle(),
			(void*)pRenderWindow->GetHandle(),
			render_width, render_height);

		tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
			maPoint2d( render_width, render_height ) );

		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			pRenderWindow->SetRenderView(m_pApp->GetRenderView(0));
			new qtMainDropTarget(pRenderWindow, m_pApp->GetRenderView(0)); // for drag & drop
			inDeviceMgr::SetWindowHandle( i_pMainFrame->GetRenderPane(0)->GetHandle() );
		}
#else // USE_WXWIDGETS
		m_pApp->InitializeRender( 0, 0, i_AppWidth, i_AppHeight );
		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			tma3dScreenUtil::SetWindowSize( maPoint2d( i_ViewOffsetX, i_ViewOffsetY ),
											maPoint2d( i_AppWidth, i_AppHeight ) );
		}
#endif // USE_WXWIDGETS

#ifdef USE_WXWIDGETS
		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			// Create other render views and hook up the render panes to the render views
			for (int i=1; i<4; ++i)
			{
				m_pApp->CreateRenderView( i_pMainFrame->GetRenderPane(i)->GetHandle() );
				i_pMainFrame->GetRenderPane(i)->SetRenderView(m_pApp->GetRenderView(i));
				i_pMainFrame->GetRenderPane(i)->SetDropTarget(new wxMainDropTarget(m_pApp->GetRenderView(i))); // for drag & drop
			}
		}
#elif defined(USE_QT)
		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			// Create other render views and hook up the render panes to the render views
			for (int i=1; i<4; ++i)
			{
				m_pApp->CreateRenderView( i_pMainFrame->GetRenderPane(i)->GetHandle() );
				i_pMainFrame->GetRenderPane(i)->SetRenderView(m_pApp->GetRenderView(i));
				new qtMainDropTarget(i_pMainFrame->GetRenderPane(i), m_pApp->GetRenderView(i)); // for drag & drop
			}
		}
#endif // USE_WXWIDGETS
		DBG_TRACE("Render Window Init complete.");
		m_bSuccessInit = (m_pApp->GetNumberOfRenderViews() > 0);
	}
	catch (const g3dShaderLoadX& ex)
	{
		mnmAppUtil::SetErrorCode(mnmErrorCodes::c_ErrorLoadShaders);
		std::string message("Failed to load shader: ");
		message += ex.GetShaderName();
		guiMessageBox::Show(message.c_str(), "Critical Error", guiMessageBox::e_OKOnly);
	}
	catch( const g2dScreenInitX& )
	{
		mnmAppUtil::SetErrorCode(mnmErrorCodes::c_ErrorCreatingGraphics);
		guiMessageBox::Show("There is a problem initializing the graphics system. Please close all programs and try again. If the problem persists, your graphics hardware may be incompatible with DirectX 11. Try running Windows Update and updating your graphics drivers. If your OS is Windows Vista, try applying update KB971512.", "Error Initializing Graphics");
	}
	catch( const g2dHardwareCapabilityX& )
	{
		mnmAppUtil::SetErrorCode(mnmErrorCodes::c_ErrorCreatingGraphics);
		guiMessageBox::Show("Your graphics hardware may be incompatible with DirectX 11. Try running Windows Update and updating your graphics drivers. If your OS is Windows Vista, try applying update KB971512.", "Error Initializing Graphics");
	}
	catch( const gfx_layer_init_failed& )
	{
		mnmAppUtil::SetErrorCode(mnmErrorCodes::c_ErrorCreatingGraphics);
		const std::string message = "Vulkan graphics could not start: " + GraphicsVulkanLayer::GetInitError();
		report(message);
#ifndef BATCH_MODE
		guiMessageBox::Show(message.c_str(), "Error Initializing Graphics");
#endif
	}

	if (m_bSuccessInit)
	{
#ifdef USE_WXWIDGETS
		pwxControlMgr::Initialize();
		pwxControlMgr::AddControlFactory( new pwxControlFactoryBase() );
		pwxControlMgr::AddControlFactory( new pwxControlFactoryCustom() );
		pwxControlMgr::AddControlFactory( new ptmControlFactoryTimeline() );
#elif defined(USE_QT)
		pqtControlMgr::Initialize();
		pqtControlMgr::AddControlFactory( new pqtControlFactoryBase() );
		pqtControlMgr::AddControlFactory( new pqtControlFactoryCustom() );
		pqtControlMgr::AddControlFactory( new ptmControlFactoryTimeline() );
#endif // USE_WXWIDGETS
		DBG_TRACE("Property controls Init complete.");

		//SplashScreen::SetStatus("Initializing Sound, Input"); System::Threading::Thread::Sleep(1);
		//guiMessageBox::Show("Test Test", "Critical Test");

		inPackage::Init();
		// The keyboard and mouse get their input from the GUI toolkit. The
		// render panels feed the wheel, so only the view under the cursor zooms.
#if defined(USE_QT)
		tqtInputFeed::Init();
#elif defined(USE_WXWIDGETS)
		twxInputFeed::Init();
#endif
		mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick
		DBG_TRACE("Input Init complete.");

		try
		{
			snSoundSystem::Initialize();
		}
		catch (snSoundSystemCreateFailedX)
		{
			//	Don't show a dialog, just record the error
			//
			//guiMessageBox::Show("No sound card installed!", "Critical Error", guiMessageBox::e_OKOnly);
			DBG_ERROR("No sound card installed!");
		}
		snSoundManager::Initialize();
		DBG_TRACE("Sound Init complete.");

		//SplashScreen::SetStatus("Initializing Effects, Model"); System::Threading::Thread::Sleep(1);

		//	read the render prefs data
		//
		rndrPrefsMgr::ReadPrefs(rndrPrefsMgr::e_ViewportPrefs);
		//	commit the render prefs to g3d
		rndrPrefsMgr::ApplyPrefs(rndrPrefsMgr::e_ViewportPrefs);

		DBG_TRACE("Graphics Init complete.");
	}
}

//--------------------------------------------------------------------
// DeInitializes library packages
//--------------------------------------------------------------------
mainInitGraphics::~mainInitGraphics()
{
	//	close it
	//
	snSoundManager::DeInitialize();
	snSoundSystem::DeInitialize();
#if defined(USE_QT)
	tqtInputFeed::CleanUp();
#elif defined(USE_WXWIDGETS)
	twxInputFeed::CleanUp();
#endif
	inPackage::CleanUp();

#ifdef USE_WXWIDGETS
//	pwxCallbackMgr::DeInitialize();
	pwxControlMgr::DeInitialize();
#elif defined(USE_QT)
	pqtControlMgr::DeInitialize();
#endif // USE_WXWIDGETS

	m_pApp->DeInitializeRender();
	delete m_pApp;

	tmlnTimeLine::DeInitialize();

	ImportExportLayer::CleanUp();
	AudioDSLayer::CleanUp();
	g3dProfiler::Stop();
	const gfx_layer& layer = chosen_layer();
	layer.CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	layer.CleanUp();
	GraphicsLayer::CleanUp();

#ifdef BATCH_MODE
	//	GraphicsVulkanLayer::CleanUp logs the counts; RenderNode also
	//	prints them for gfxcompare ("off" when validation didn't run)
	if (l_bLayerInitialized && l_GfxOptions.m_Backend == g3dGfxOptions::e_Vulkan)
	{
		if (GraphicsVulkanLayer::IsValidating())
		{
			const gvk::vkdValidationStats stats = GraphicsVulkanLayer::GetValidationStats();
			std::cerr << "Vulkan validation: " << stats.m_Errors << " errors, " << stats.m_Warnings << " warnings" << std::endl;
		}
		else
		{
			std::cerr << "Vulkan validation: off" << std::endl;
		}
	}
#endif
	l_bLayerInitialized = false;
}

//--------------------------------------------------------------------
// Returns the Graphics system that was created
//--------------------------------------------------------------------
g2dSystem* mainInitGraphics::GetSystem()
{
	return m_pApp->GetSystem();
}

