/*****************************************************************************
**	mnmApp.hpp
**
**		The main application
**
**	StudioGPU
**	Copyright(C) 2003-10 - All Rights Reserved
\****************************************************************************/
#ifndef APP_APPLICATION_HPP
#include "Core/app/appApplication.hpp"
#endif

#ifndef APP_FLOWEVENTHANDLER_HPP
#include "Core/app/appFlowEventHandler.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef GPX_PROXYDEFS_HPP
#include "Tool/gpx/gpxProxyDefs.hpp"
#endif 
#ifndef MA_RUNNINGAVERAGE_HPP
#include "Core/ma/maRunningAverage.hpp"
#endif
#ifndef MODE_MODE_HPP
#include "Support/mode/modeMode.hpp" // for modeModeID
#endif


//============================================================================
//	Forward References
//============================================================================
class g2dSystem;
class g3dSystem;
class g3dSceneRenderer;
class g3dViewer;
class g2dWindow;
class cptrModeCapture;
class tma3dRenderView;
class rpnPanelViewer;


//============================================================================
//	mnmApp
//============================================================================
class mnmApp :	public appApplication,
				public appFlowEventHandler
{
	public:
		//--------------------------------------------------------------------
		//	RunApp() should be called to start the application.  When control
		//	returns from RunApp(), the application is finished.
		//--------------------------------------------------------------------
		static void RunApp();

		//--------------------------------------------------------------------
		//	Thinks the app instance
		//--------------------------------------------------------------------
		static void ThinkApp();

		//--------------------------------------------------------------------
		//	Run() should be called to start the application.  When control
		//	returns from Run(), the application is finished.
		//--------------------------------------------------------------------
		virtual void Run();

		//--------------------------------------------------------------------
		//	Thinks the app instance
		//--------------------------------------------------------------------
		static bool IsActive();

		//--------------------------------------------------------------------
		//	set this app as "active"
		//--------------------------------------------------------------------
		static void SetActive( bool i_bActive );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static void ResizeRender(int i_Width, int i_Height);

		//--------------------------------------------------------------------
		// Enable/Disable rendering in main window
		//--------------------------------------------------------------------
		static void EnableRender(bool i_bVal);
		static bool IsRenderEnabled();

		//--------------------------------------------------------------------
		//	SetRenderer() - give renderers a chance to free up resources
		//  before switching
		//--------------------------------------------------------------------
		static void SetRenderer(int i_RendererType);
		static void SetRenderPass(int i_RenderPassType, std::string& i_RenderPassName);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static int GetScreenSpaceIndex();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//static int GetIconsLayerIndex();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//static int GetManipulatorsLayerIndex();

		//--------------------------------------------------------------------
		//	parse the command line and configure the app accordingly
		//--------------------------------------------------------------------
		static void ParseCommandLine(std::string& i_lpCmdLine);

		//--------------------------------------------------------------------
		// return value in KB
		//--------------------------------------------------------------------
		static float QueryMaxVideoMemory();
		static float QueryVideoMemory();

		//------------------------------------------------------------------------
		// Enable viewport multithreading from preferences
		//------------------------------------------------------------------------
		static void SetThreadingEnabled(bool i_bThreading);

		//------------------------------------------------------------------------
		// Set the size of the Qt widget that shows MSP window
		//------------------------------------------------------------------------
		static void SetWindowSize(int width, int height);

	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mnmApp();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~mnmApp();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void InitializeRender(void *i_Hwnd1, void *i_Hwnd2, 
							 int i_Width, int i_Height);

		//--------------------------------------------------------------------
		// Create an additional render view using the given HWND
		//--------------------------------------------------------------------
		void CreateRenderView(void *i_Hwnd);

		//--------------------------------------------------------------------
		// GetRenderView
		//--------------------------------------------------------------------
		tma3dRenderView* GetRenderView(int i_Index);

		//--------------------------------------------------------------------
		// GetNumberOfRenderViews - return the number of render views 
		//--------------------------------------------------------------------
		int GetNumberOfRenderViews();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DeInitializeRender();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Render();

		//--------------------------------------------------------------------
		// Part of render that runs in separate thread
		//--------------------------------------------------------------------
		static void DoRenderThread();

 		//--------------------------------------------------------------------
		// Actual rendering function, may or may not be in a thread.
		//--------------------------------------------------------------------
		static void DoRender();

		//--------------------------------------------------------------------
		//	Override this function to get appStartEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveStartEvent(appStartEvent& i_Event);

		//--------------------------------------------------------------------
		//	Override this function to get appStopEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveStopEvent(appStopEvent& i_Event);

		//--------------------------------------------------------------------
		//	Override this function to get appSuspendEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveSuspendEvent(appSuspendEvent& i_Event);

		//--------------------------------------------------------------------
		//	Override this function to get appResumeEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveResumeEvent(appResumeEvent& i_Event);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event);

		//--------------------------------------------------------------------
		// Wrapper around Think() that handles exceptions
		//--------------------------------------------------------------------
		void TryThink();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Think();

		//--------------------------------------------------------------------
		// Accessor to system for creating multiple view windows
		//--------------------------------------------------------------------
		inline g2dSystem* GetSystem();

		//--------------------------------------------------------------------
		// Accessor to 3D system
		//--------------------------------------------------------------------
		inline g3dSystem* GetSystem3D();

		//--------------------------------------------------------------------
		//	Exit() should be called by the client when it decides it is
		//	ready to end the program.
		//--------------------------------------------------------------------
		void Exit();

private:
	static bool				sm_bThreadingEnabled;
	static maRunningAverage	sm_FPSAverage;

	g2dSystem*			m_pSystem;
	g3dSystem*			m_pSystem3D;
	//std::vector<g3dSceneRenderer*> m_pRenderer;
	//g3dViewer*			m_pViewer;
	//g2dWindow*			m_pWindow;

	// Other views besides main window
	std::vector<tma3dRenderView*> m_RenderViews;
	std::vector<rpnPanelViewer*> m_PanelViewers;
};



//--------------------------------------------------------------------
// Accessor to system for creating multiple view windows
//--------------------------------------------------------------------
inline g2dSystem* mnmApp::GetSystem()
{
	return m_pSystem;
}

//--------------------------------------------------------------------
// Accessor to 3D system
//--------------------------------------------------------------------
inline g3dSystem* mnmApp::GetSystem3D()
{
	return m_pSystem3D;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void render_3dWindow();
