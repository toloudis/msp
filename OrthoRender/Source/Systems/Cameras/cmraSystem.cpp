/*****************************************************************************
**  cmraSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/cmraSystem.hpp"

#include "Systems/Cameras/GUI/cmraAnimList.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/GUI/cmraCommands.hpp"
//#include "Systems/Cameras/Cue/cmraCueDialogUtil.hpp"
#include "Systems/Cameras/GUI/cmraDialogInterest.hpp"
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"
#include "Systems/Cameras/Data/cmraDocumentInterest.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCreator.hpp"
#include "Systems/Cameras/Drivers/cmraDriverTargetParser.hpp"
#include "Systems/Cameras/Object/cmraExportInterest.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Object/cmraSelectInterest.hpp"
#include "Systems/Cameras/Object/cmraThinkInterest.hpp"

//	library
#include "Support/cams/camsCameraMgr.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Support/mexp/mexpMgr.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "ToolUIManaged/tma/tmaSystem.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Support/tmln/tmlnTimeInOutMgr.hpp"
#include "Support/vis/visMgr.hpp"


//============================================================================
//============================================================================
namespace cmraSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<cmraObjectMgr, cmraCameraObject> cmraNameInterest;
		typedef cmmPickInterestTemplate<cmraObjectMgr> cmraPickInterest;
		typedef cmmVisibleInterestTemplate<cmraObjectMgr> cmraVisibleInterest;

		cmraNameInterest*		l_pCamerasNI = 0;
		cmraPickInterest*		l_pCamerasPI = 0;
		cmraSelectInterest*		l_pCamerasSI = 0;
		cmraThinkInterest*		l_pCamerasTI = 0;
		cmraVisibleInterest*	l_pCamerasVI = 0;
		cmraDialogInterest*		l_pCamerasDI = 0;
		cmraExportInterest*		l_pCamerasEI = 0;

		// Callbacks for when to select a camera object coming from other parts
		// of the user interface
		class CameraListCallback : public camsCameraMgr::CameraListChangedCallback
		{
		public:
			virtual void CameraListChanged()
			{
			}
			virtual void SelectCamera(int i_Index)
			{
				if (i_Index >= 0 && i_Index < cmraObjectMgr::GetNumObjects())
				{
					cmraObjectMgr::SelectObject(i_Index);
				}
			}
		};
		CameraListCallback l_CameraCallback;

		// Callbacks for when to build timeline in out lists
		class TimeInOutCallback : public tmlnTimeInOutMgr::TimeInOutInterest
		{
		public:
			virtual void BuildTimeInOutLists()
			{
				cmraObjectMgr::BuildTimeInOutLists();
			}
		};
		TimeInOutCallback l_TimeInOutCallback;
	}

	//--------------------------------------------------------------------
	// Init 
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_CameraDataDir)
	{
		// Set paths
		cmraAnimList::Init( i_CameraDataDir, itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
		cmraAnimList::SetAppDirectory( i_AppDir );

		// Set up callback
		camsCameraMgr::AddCallback(&l_CameraCallback);
		tmlnTimeInOutMgr::AddInterest(&l_TimeInOutCallback);

		// Initialize namespaces
		cmraObjectMgr::Init();
		cmraDialogUtil::Init();
		//cmraCueDialogUtil::Init(i_pSystem);

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new cmraDocumentInterest());
//WXGUI
		/*
				cmraCommands::SetupMenu();
*/

		// register the pick interest
		if ( l_pCamerasPI == 0 )
		{
			l_pCamerasPI = new cmraPickInterest();
			pick3dMgr::RegisterPickInterest( l_pCamerasPI );
		}

		// register the select interest
		if ( l_pCamerasSI == 0 )
		{
			l_pCamerasSI = new cmraSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pCamerasSI );
		}

		// register the visible interest
		if ( l_pCamerasVI == 0 )
		{
			l_pCamerasVI = new cmraVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pCamerasVI );
		}

		// register the name interest
		if ( l_pCamerasNI == 0 )
		{
			l_pCamerasNI = new cmraNameInterest();
			nameMgr::RegisterNameInterest( l_pCamerasNI );
		}

		// register the think interest
		if ( l_pCamerasTI == 0 )
		{
			l_pCamerasTI = new cmraThinkInterest();
			mnmThinkMgr::RegisterThinkInterest( l_pCamerasTI );
		}

		// register the dialog interest
		if ( l_pCamerasDI == 0 )
		{
			l_pCamerasDI = new cmraDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pCamerasDI );
		}

		// register the export interest
		if ( l_pCamerasEI == 0 )
		{
			l_pCamerasEI = new cmraExportInterest();
			mexpMgr::RegisterExportInterest( l_pCamerasEI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new cmraDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		cmraDriverCreator::CreateParsers();

	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pCamerasPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pCamerasPI );
			delete l_pCamerasPI;
			l_pCamerasPI = 0;
		}

		//	Unregister the Select Interest
		if ( l_pCamerasSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pCamerasSI );
			delete l_pCamerasSI;
			l_pCamerasSI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pCamerasVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pCamerasVI );
			delete l_pCamerasVI;
			l_pCamerasVI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pCamerasNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pCamerasNI );
			delete l_pCamerasNI;
			l_pCamerasNI = 0;
		}

		//	Unregister the Think Interest
		if ( l_pCamerasTI != 0 )
		{
			mnmThinkMgr::UnRegisterThinkInterest( l_pCamerasTI );
			delete l_pCamerasTI;
			l_pCamerasTI = 0;
		}

		//	Unregister the dialog Interest
		if ( l_pCamerasDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pCamerasDI );
			delete l_pCamerasDI;
			l_pCamerasDI = 0;
		}

		//	Unregister the export Interest
		if ( l_pCamerasEI != 0 )
		{
			mexpMgr::UnRegisterExportInterest( l_pCamerasEI );
			delete l_pCamerasEI;
			l_pCamerasEI = 0;
		}

		// Remove callback
		tmlnTimeInOutMgr::RemoveInterest(&l_TimeInOutCallback);
		camsCameraMgr::RemoveCallback(&l_CameraCallback);

		// Clean up namespaces
		cmraCommands::CleanUp();
		cmraDialogUtil::CleanUp();
		cmraObjectMgr::CleanUp();
		cmraAnimList::CleanUp();
	}

}	// end of namespace