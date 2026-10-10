/*****************************************************************************
**	cptrRenderStatsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Debug/dbgConsoleDialogUtil.hpp"

#include "Features/Debug/dbgConsoleDataUtil.hpp"
#ifdef MSP_GUI_QT
#include "Features/Debug/qtGUI/dbgConsoleDialog.hpp"
#else
#include "Features/Debug/wxGUI/dbgConsoleDialog.hpp"
#endif
#include "Support/mnm/mnmThinkInterest.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Env/envThread.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#else
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

//============================================================================
//============================================================================
namespace dbgConsoleDialogUtil
{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
	// Buffer debug messages into list and wait for 
	// gui thread to pick off the messages from the buffer
	bool l_bHasNewMessages = false;
	envMutex l_DebugMessagesMutex;
	std::list<std::string> l_Messages;

	//============================================================================
	//============================================================================
	class DebugMonitorThinkInterest : public mnmThinkInterest
	{
		public:
			//--------------------------------------------------------------------
			//	Think
			//--------------------------------------------------------------------
			virtual void Think( )
			{
				// Quicker and safe to check the boolean flag without the mutex lock
				if (l_bHasNewMessages)
				{
					// Use the lock when we want to access the thread shared 
					// message buffer.
					envScopedLock message_lock(l_DebugMessagesMutex);
					if (dbgConsoleDialog::Instance != NULL)
					{
						std::list<std::string>::iterator it;
						for (it = l_Messages.begin(); it != l_Messages.end(); ++it)
							dbgConsoleDialog::Instance->UpdateConsoleString( *it );
					}
					l_Messages.clear();
					l_bHasNewMessages = false;
				}
			}
	};
	DebugMonitorThinkInterest l_ThinkInterest;
#endif

	//------------------------------------------------------------------------
	//  Init
	//------------------------------------------------------------------------
	void  Init()
	{
#ifdef USE_WXWIDGETS
		if (dbgConsoleDialog::Instance == NULL)
		{
			dbgConsoleDialog::Instance = new dbgConsoleDialog( twxSystem::g_pMainForm );
		}
		mnmThinkMgr::RegisterThinkInterest(&l_ThinkInterest);
#elif defined(USE_QT)
		if (dbgConsoleDialog::Instance == NULL)
		{
			dbgConsoleDialog::Instance = new dbgConsoleDialog( tqtSystem::g_pMainForm );
		}
		mnmThinkMgr::RegisterThinkInterest(&l_ThinkInterest);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show(dbgConsoleDialog::Instance);
#elif defined(USE_QT)
		tqtPaneMgr::Show(dbgConsoleDialog::Instance);
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		mnmThinkMgr::UnRegisterThinkInterest(&l_ThinkInterest);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateConsoleDialog(std::string& i_String)
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		// Put messages into a buffer and let the think interest
		// pick them off and give them to wxWidgets later from the gui thread.
		//
		envScopedLock message_lock(l_DebugMessagesMutex);
		l_Messages.push_back( i_String );
		l_bHasNewMessages = true;
#endif
	}
}

