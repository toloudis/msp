/*****************************************************************************
**  mspApp.hpp
**
**      The main application
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

//#include <windows.h>

#ifndef APP_APPLICATION_HPP
#include "Core/app/appApplication.hpp"
#endif

#ifndef APP_FLOWEVENTHANDLER_HPP
#include "Core/app/appFlowEventHandler.hpp"
#endif

#include <string>

//============================================================================
//============================================================================
class g2dSystem;
class g3dSystem;
class g3dSceneRenderer;
class g3dViewer;
class g2dWindow;
class matMaterial;


//============================================================================
//	mspApp
//============================================================================
class mspApp :	public appApplication,
				public appFlowEventHandler
{
	public:
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
		//	parse the command line and configure the app accordingly
		//--------------------------------------------------------------------
		static void ParseCommandLine(std::string& i_lpCmdLine);

	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mspApp();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~mspApp();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void InitializeRender(void *i_Hwnd1, void *i_Hwnd2, int i_Width, int i_Height);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ResizeRenderWindow(int i_Width, int i_Height);

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
		virtual void Think();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		 void Update();

		//--------------------------------------------------------------------
		// Accessor to system for creating multiple view windows
		//--------------------------------------------------------------------
		inline g2dSystem* GetSystem();

		//--------------------------------------------------------------------
		//	GetWindow() - get a pointer to the main render window
		//--------------------------------------------------------------------
		inline g2dWindow* GetWindow() const;

		//--------------------------------------------------------------------
		//	Override this function to get appQuitReqeustedEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event);

private:
	g2dSystem* m_pSystem;
	g3dSystem* m_pSystem3D;
	g3dSceneRenderer* m_pRenderer;
	g3dViewer* m_pViewer;
	g2dWindow* m_pWindow;
	matMaterial* m_pSimpleMaterial;
};



//--------------------------------------------------------------------
// Accessor to system for creating multiple view windows
//--------------------------------------------------------------------
inline g2dSystem* mspApp::GetSystem()
{
	return m_pSystem;
}


//--------------------------------------------------------------------
//	GetWindow() - get a pointer to the main render window
//--------------------------------------------------------------------
g2dWindow* mspApp::GetWindow() const
{
	return m_pWindow;
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void initialize_objects();
void deinitialize_objects();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void render_3dWindow();

