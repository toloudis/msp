/*****************************************************************************
**  propSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/propSystem.hpp"

#include "Systems/Props/GUI/propAnimList.hpp"
#include "Systems/Props/GUI/propCommands.hpp"
#include "Systems/Props/GUI/propDialogInterest.hpp"
#include "Systems/Props/GUI/propDialogUtil.hpp"
#include "Systems/Props/Data/propDocumentInterest.hpp"
#include "Systems/Props/Drivers/propDriverCreator.hpp"
#include "Systems/Props/Object/propExportInterest.hpp"
#include "Systems/Props/GUI/propGeomList.hpp"
#include "Systems/Props/Object/propObjectMgr.hpp"
#include "Systems/Props/Object/propReportMemInterest.hpp"
#include "Systems/Props/Object/propSelectInterest.hpp"

#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Drivers/Sound/tmlnDriverSoundParser.hpp"

//	tools
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Support/mexp/mexpMgr.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Support/vis/visMgr.hpp"

//	lib
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"

//============================================================================
//============================================================================
namespace propSystem
{

namespace
{
	// Interests implemented through templates
	typedef cmmNameInterestTemplate<propObjectMgr, propPropObject> propNameInterest;
	typedef cmmPickInterestTemplate<propObjectMgr> propPickInterest;
	typedef cmmVisibleInterestGeomTemplate<propObjectMgr> propVisibleInterest;

	propNameInterest*		l_pPropsNI = 0;
	propPickInterest*		l_pPropsPI = 0;
	propSelectInterest*		l_pPropsSI = 0;
	propVisibleInterest*	l_pPropsVI = 0;
	propDialogInterest*		l_pPropsDI = 0;
	propExportInterest*		l_pPropsEI = 0;
	propReportMemInterest*	l_pPropsMI = 0;

	
	// Callbacks for when to change the subdiv level when called from
	// outside our system
	class SubdivCallback : public api3dSubdivInterest
	{
	public:
		virtual void SubdivLevelChanged( int i_Level )
		{
			propObjectMgr::SetSubdivLevel(i_Level);
		}
	};
	SubdivCallback l_SubdivCallback;
}

	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//
	//	SoundDir should be the post-fix dir path under the app path.
	//--------------------------------------------------------------------
	void Init( const itString& i_PropDataDirName )
	{
		//DBG_LOG1( "prop data dir (%s)", dbgLog::UnicodetoANSI(i_PropDataDirName.GetString(),i_PropDataDirName.GetLength()) );
		propAnimList::Init( i_PropDataDirName, 
							itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
		propGeomList::Init(i_PropDataDirName, 
							itString(gfPaths::GetSubPath(gfPaths::e_Data)));	
		propDialogUtil::Init();
		propObjectMgr::Init();

		// Set up callback
		api3dSubdiv::RegisterSubdivInterest(&l_SubdivCallback);

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new propDocumentInterest());

		// register the pick interest
		if ( l_pPropsPI == 0 )
		{
			l_pPropsPI = new propPickInterest();
			pick3dMgr::RegisterPickInterest( l_pPropsPI );
		}

		// register the select interest
		if ( l_pPropsSI == 0 )
		{
			l_pPropsSI = new propSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pPropsSI );
		}

		// register the name interest
		if ( l_pPropsNI == 0 )
		{
			l_pPropsNI = new propNameInterest();
			nameMgr::RegisterNameInterest( l_pPropsNI );
		}

		// register the visible interest
		if ( l_pPropsVI == 0 )
		{
			l_pPropsVI = new propVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pPropsVI );
		}

		// register the dialog interest
		if ( l_pPropsDI == 0 )
		{
			l_pPropsDI = new propDialogInterest();
			cmmDialogInterestMgr::RegisterInterest(l_pPropsDI);
		}

		// register the export interest
		if ( l_pPropsEI == 0 )
		{
			l_pPropsEI = new propExportInterest();
			mexpMgr::RegisterExportInterest(l_pPropsEI);
		}

		// register the memory report interest
		if ( l_pPropsMI == 0 )
		{
			l_pPropsMI = new propReportMemInterest();
			rptReportMemMgr::RegisterReportMemInterest( l_pPropsMI );
		}

		// Timeline related creators
		fsLocator i_SoundDir;	// FIX: - this is here temp so this can compile.
		tmlnDriverCreator* pDriverCreator = new propDriverCreator(i_SoundDir);
		tmlnCreator::AddDriverCreator(pDriverCreator);
		propDriverCreator::CreateParsers();

		//	commands
		propCommands::SetupMenu();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pPropsPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pPropsPI );
			delete l_pPropsPI;
			l_pPropsPI = 0;
		}

		//	Unregister the Pick Interest
		if ( l_pPropsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pPropsSI );
			delete l_pPropsSI;
			l_pPropsSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pPropsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pPropsNI );
			delete l_pPropsNI;
			l_pPropsNI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pPropsVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pPropsVI );
			delete l_pPropsVI;
			l_pPropsVI = 0;
		}

		// Unregister the dialog interest
		if ( l_pPropsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest(l_pPropsDI);
			delete l_pPropsDI;
			l_pPropsDI = 0;
		}

		// Unregister the export interest
		if ( l_pPropsEI != 0 )
		{
			mexpMgr::UnRegisterExportInterest(l_pPropsEI);
			delete l_pPropsEI;
			l_pPropsEI = 0;
		}

		//	Unregister the Memory Report Interest
		if ( l_pPropsMI != 0 )
		{
			rptReportMemMgr::UnRegisterReportMemInterest( l_pPropsMI );
			delete l_pPropsMI;
			l_pPropsMI = 0;
		}

		// Remove callback
		api3dSubdiv::UnRegisterSubdivInterest(&l_SubdivCallback);

		// Clean up namespaces
		propCommands::CleanUp();
		propObjectMgr::CleanUp();
		propDialogUtil::CleanUp();
		propGeomList::CleanUp();
		propAnimList::CleanUp();
	}

}	// end of namespace
