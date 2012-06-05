/*****************************************************************************
**  chnlPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-9 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlPackage.hpp"

#include "Features/Channels/chnlCommands.hpp"
#include "Features/Channels/Data/chnlTimeDocumentInterest.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/chnlPython.hpp"
#include "Features/Channels/Markers/chnlPythonMarkers.hpp"
#include "Features/Channels/Notes/chnlPythonNotes.hpp"
#include "Features/Channels/chnlSelectInterest.hpp"
#include "Features/Channels/chnlPrefsInterest.hpp"
#include "Features/Prefs/PrefsMgr.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace chnlPackage
{
	namespace
	{
		chnlSelectInterest*	l_pChannelsSI = 0;
		chnlPrefsInterest*	l_pChannelsPI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		chnlDialogUtil::Init();

		//	register the document interest
		docSingleTypeMgr::AddDocumentInterest(new chnlTimeDocumentInterest());

		// register the select interest
		if ( l_pChannelsSI == 0 )
		{
			l_pChannelsSI = new chnlSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pChannelsSI );
		}

		// register the prefs interest
		if ( l_pChannelsPI == 0 )
		{
			l_pChannelsPI = new chnlPrefsInterest();
			PrefsMgr::RegisterInterest( l_pChannelsPI );
		}

		//	add commands
		chnlCommands::SetupMenu();

		// Add in python commands
		chnlPython::AddCommands("mach");
		chnlPythonMarkers::AddCommands("mach");
		chnlPythonNotes::AddCommands("mach");
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pChannelsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pChannelsSI );
			delete l_pChannelsSI;
			l_pChannelsSI = 0;
		}

		//	Unregister the Prefs Interest
		if ( l_pChannelsPI != 0 )
		{
			PrefsMgr::UnRegisterInterest( l_pChannelsPI );
			delete l_pChannelsPI;
			l_pChannelsPI = 0;
		}

		// Clean up namespaces
		chnlDialogUtil::CleanUp();
	}

}	// end of namespace