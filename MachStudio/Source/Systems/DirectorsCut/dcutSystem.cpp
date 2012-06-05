/*****************************************************************************
**  dcutSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/DirectorsCut/dcutSystem.hpp"

#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"
#include "Systems/DirectorsCut/GUI/dcutCommands.hpp"
#include "Systems/DirectorsCut/Cue/dcutCueDialogUtil.hpp"
#include "Systems/DirectorsCut/GUI/dcutDialogInterest.hpp"
#include "Systems/DirectorsCut/GUI/dcutDialogUtil.hpp"
#include "Systems/DirectorsCut/Data/dcutDocumentInterest.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCreator.hpp"
#include "Systems/DirectorsCut/Object/dcutExportInterest.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"
#include "Systems/DirectorsCut/Object/dcutScriptObject.hpp"
#include "Systems/DirectorsCut/Object/dcutSelectInterest.hpp"

//	library
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Support/mexp/mexpMgr.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Support/tmln/tmlnTimeInOutMgr.hpp"
#include "Support/vis/visMgr.hpp"


//============================================================================
//============================================================================
namespace dcutSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<dcutObjectMgr, dcutDirectorsCutObject> dcutNameInterest;

		dcutNameInterest*		l_pDirectorsCutsNI = 0;
		dcutSelectInterest*		l_pDirectorsCutsSI = 0;
		dcutDialogInterest*		l_pDirectorsCutsDI = 0;
		dcutExportInterest*		l_pDirectorsCutsEI = 0;
		
		// Callbacks for when to select a camera object coming from other parts
		// of the user interface
		class DCutListCallback : public camsDirectorsCutMgr::DirectorsCutListChangedCallback
		{
		public:
			virtual void DirectorsCutListChanged()
			{
			}
			virtual void SelectDirectorsCut(int i_Index)
			{
				if (i_Index >= 0 && i_Index < dcutObjectMgr::GetNumObjects())
				{
					sel3dMgr::CreateUndoOperation();
					dcutObjectMgr::SelectObject(i_Index);
				}
			}
		};
		DCutListCallback l_DCutCallback;

		// Callbacks for when to build timeline in out lists
		class TimeInOutCallback : public tmlnTimeInOutMgr::TimeInOutInterest
		{
		public:
			virtual void BuildTimeInOutLists()
			{
				dcutObjectMgr::BuildTimeInOutLists();
			}
		};
		TimeInOutCallback l_TimeInOutCallback;
	}

	//--------------------------------------------------------------------
	// Init  - needs the system for creating rendering views
	//--------------------------------------------------------------------
	void Init(g2dSystem *i_pSystem)
	{
		// Initialize namespaces
		//dcutDialogUtil::Init();
		//dcutCueDialogUtil::Init(i_pSystem);

		// Set up callback
		camsDirectorsCutMgr::AddCallback(&l_DCutCallback);
		tmlnTimeInOutMgr::AddInterest(&l_TimeInOutCallback);

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new dcutDocumentInterest());
		//dcutCommands::SetupMenu();

		// register the select interest
		if ( l_pDirectorsCutsSI == 0 )
		{
			l_pDirectorsCutsSI = new dcutSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pDirectorsCutsSI );
		}

		// register the name interest
		if ( l_pDirectorsCutsNI == 0 )
		{
			l_pDirectorsCutsNI = new dcutNameInterest();
			nameMgr::RegisterNameInterest( l_pDirectorsCutsNI );
		}

		// register the dialog interest
		/*if ( l_pDirectorsCutsDI == 0 )
		{
			l_pDirectorsCutsDI = new dcutDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pDirectorsCutsDI );
		}*/

		// register the export interest
		if ( l_pDirectorsCutsEI == 0 )
		{
			l_pDirectorsCutsEI = new dcutExportInterest();
			mexpMgr::RegisterExportInterest( l_pDirectorsCutsEI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new dcutDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		dcutDriverCreator::CreateParsers();


	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pDirectorsCutsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pDirectorsCutsSI );
			delete l_pDirectorsCutsSI;
			l_pDirectorsCutsSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pDirectorsCutsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pDirectorsCutsNI );
			delete l_pDirectorsCutsNI;
			l_pDirectorsCutsNI = 0;
		}

		//	Unregister the dialog Interest
		/*if ( l_pDirectorsCutsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pDirectorsCutsDI );
			delete l_pDirectorsCutsDI;
			l_pDirectorsCutsDI = 0;
		}*/

		//	Unregister the export Interest
		if ( l_pDirectorsCutsEI != 0 )
		{
			mexpMgr::UnRegisterExportInterest( l_pDirectorsCutsEI );
			delete l_pDirectorsCutsEI;
			l_pDirectorsCutsEI = 0;
		}
	
		// Remove callback
		tmlnTimeInOutMgr::RemoveInterest(&l_TimeInOutCallback);
		camsDirectorsCutMgr::RemoveCallback(&l_DCutCallback);

		// Clean up namespaces
		//dcutCueDialogUtil::CleanUp();
		//dcutDialogUtil::CleanUp();
	}

}	// end of namespace