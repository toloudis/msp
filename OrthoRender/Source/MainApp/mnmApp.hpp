/*****************************************************************************
**  mnmApp.hpp
**
**      The main application
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

//#include <windows.h>

#ifndef APP_APPLICATION_HPP
#include "Core/App/appApplication.hpp"
#endif

#ifndef APP_FLOWEVENTHANDLER_HPP
#include "Core/App/appFlowEventHandler.hpp"
#endif
#ifndef MODE_MODE_HPP
#include "Support/mode/modeMode.hpp" // for modeModeID
#endif


//============================================================================
//============================================================================
class g2dSystem;
class g3dSystem;
class g3dSceneRenderer;
class g3dViewer;
class g2dWindow;
class cptrModeCapture;
class tma3dRenderView;


//============================================================================
//	mnmApp
//============================================================================
class mnmApp :	public appApplication,
				public appFlowEventHandler
{
	public:
		//--------------------------------------------------------------------
		//	Run() should be called to start the application.  When control
		//	returns from Run(), the application is finished.
		//--------------------------------------------------------------------
		virtual void Run();

		//--------------------------------------------------------------------
		//	Thinks the app instance
		//--------------------------------------------------------------------
		static void ThinkApp();

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

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static int GetScreenSpaceIndex();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static int GetIconsLayerIndex();

		//--------------------------------------------------------------------
		//	pre parse the command line and configure the app accordingly (right after startup so only set states, no calls)
		//--------------------------------------------------------------------
		static void PreParseCommandLine(std::string& i_lpCmdLine);

		//--------------------------------------------------------------------
		//	parse the command line and configure the app accordingly
		//--------------------------------------------------------------------
		static void ParseCommandLine(std::string& i_lpCmdLine);

		static int QueryMaxVideoMemory();
		static int QueryVideoMemory();

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
		//--------------------------------------------------------------------
		void DeInitializeRender();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Render();

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
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		static bool remote_message_processing(void);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		int setup_remote_message_processing(void);

		//--------------------------------------------------------------------
		//	override the standard message loop
		//--------------------------------------------------------------------
		virtual void message_loop();

	private:
		g2dSystem*			m_pSystem;
		g3dSystem*			m_pSystem3D;
		//std::vector<g3dSceneRenderer*> m_pRenderer;
		//g3dViewer*			m_pViewer;
		//g2dWindow*			m_pWindow;

		// Other views besides main window
		std::vector<tma3dRenderView*> m_RenderViews;
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
void initialize_objects();
void deinitialize_objects();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void render_3dWindow();
