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
#include "Features/Debug/wxGUI/dbgConsoleDialog.hpp"
#include "Support/mnm/mnmThinkInterest.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Env/envThread.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

//============================================================================
//============================================================================
namespace dbgConsoleDialogUtil
{
#ifdef USE_WXWIDGETS
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
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show(dbgConsoleDialog::Instance);
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#ifdef USE_WXWIDGETS
		mnmThinkMgr::UnRegisterThinkInterest(&l_ThinkInterest);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateConsoleDialog(std::string& i_String)
	{
#ifdef USE_WXWIDGETS
		// Put messages into a buffer and let the think interest
		// pick them off and give them to wxWidgets later from the gui thread.
		//
		envScopedLock message_lock(l_DebugMessagesMutex);
		l_Messages.push_back( i_String );
		l_bHasNewMessages = true;
#endif
	}
}

