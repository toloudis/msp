/*****************************************************************************
**  chtrSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/chtrSystem.hpp"

#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Systems/Character/GUI/chtrCommands.hpp"
#include "Systems/Character/GUI/chtrDialogInterest.hpp"
#include "Systems/Character/GUI/chtrDialogUtil.hpp"
#include "Systems/Character/Data/chtrDocumentInterest.hpp"
#include "Systems/Character/Drivers/chtrDriverCreator.hpp"
#include "Systems/Character/Object/chtrExportInterest.hpp"
#include "Systems/Character/Expressions/chtrExpressionDriverCreator.hpp"
#include "Systems/Character/Expressions/chtrExpressionsDialogUtil.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Object/chtrSelectInterest.hpp"
#include "Systems/Character/Object/chtrReportMemInterest.hpp"

#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"

#include "Tool/api3d/api3dSubdiv.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Support/mexp/mexpMgr.hpp"
#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Drivers/Sound/tmlnDriverSoundParser.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"

//	tools
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Support/vis/visMgr.hpp"

//	lib
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"


namespace chtrSystem
{
namespace
{
	// Interests implemented through templates
	typedef cmmNameInterestTemplate<chtrObjectMgr, chtrObject> chtrNameInterest;
	typedef cmmPickInterestTemplate<chtrObjectMgr> chtrPickInterest;
	typedef cmmVisibleInterestGeomTemplate<chtrObjectMgr> chtrVisibleInterest;
	
	chtrNameInterest*		l_pCharactersNI = 0;
	chtrPickInterest*		l_pCharactersPI = 0;
	chtrSelectInterest*		l_pCharactersSI = 0;
	chtrVisibleInterest*	l_pCharactersVI = 0;
	chtrDialogInterest*		l_pCharactersDI = 0;
	chtrExportInterest*		l_pCharactersEI = 0;
	chtrReportMemInterest*	l_pCharactersMI = 0;

	const int c_MaxSubdivLevel	= 2;	//= 3;

	// Callbacks for when to change the subdiv level when called from
	// outside our system
	class SubdivCallback : public api3dSubdivInterest
	{
	public:
		virtual void SubdivLevelChanged( int i_Level )
		{
			chtrObjectMgr::SetSubdivLevel(i_Level);
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
	void Init(const fsLocator& i_AppDir, const itString& i_CharacterDataDir)
	{
		// Set up callback
		api3dSubdiv::RegisterSubdivInterest(&l_SubdivCallback);

		// Set subdiv level
		smdlSubdivCharacter::SetMaxSubdivLevel(c_MaxSubdivLevel);

		chtrAnimList::Init( i_CharacterDataDir, 
							itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
		chtrAnimList::SetAppDirectory( i_AppDir );
		chtrGeomList::Init(i_CharacterDataDir, 
						  itString(gfPaths::GetSubPath(gfPaths::e_Data)));		
		//chtrGeomList::SetAppDirectory( i_AppDir );

		// Initialize namespaces
		chtrDialogUtil::Init();
		chtrExpressionsDialogUtil::Init();
		chtrObjectMgr::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new chtrDocumentInterest());

		// register the pick interest
		if ( l_pCharactersPI == 0 )
		{
			l_pCharactersPI = new chtrPickInterest();
			pick3dMgr::RegisterPickInterest( l_pCharactersPI );
		}

		// register the select interest
		if ( l_pCharactersSI == 0 )
		{
			l_pCharactersSI = new chtrSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pCharactersSI );
		}

		// register the name interest
		if ( l_pCharactersNI == 0 )
		{
			l_pCharactersNI = new chtrNameInterest();
			nameMgr::RegisterNameInterest( l_pCharactersNI );
		}

		// register the visible interest
		if ( l_pCharactersVI == 0 )
		{
			l_pCharactersVI = new chtrVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pCharactersVI );
		}

		// register the dialog interest
		if ( l_pCharactersDI == 0 )
		{
			l_pCharactersDI = new chtrDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pCharactersDI );
		}

		// register the export interest
		if ( l_pCharactersEI == 0 )
		{
			l_pCharactersEI = new chtrExportInterest();
			mexpMgr::RegisterExportInterest( l_pCharactersEI );
		}

		// register the memory report interest
		if ( l_pCharactersMI == 0 )
		{
			l_pCharactersMI = new chtrReportMemInterest();
			rptReportMemMgr::RegisterReportMemInterest( l_pCharactersMI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new chtrDriverCreator();
		tmlnCreator::AddDriverCreator(pDriverCreator);
		chtrDriverCreator::CreateParsers();

		// Expression driver creators
		tmlnCreator::AddDriverCreator(new chtrExpressionDriverCreator);
		chtrExpressionDriverCreator::CreateParsers();

		//	commands
//WXGUI
/*
		chtrCommands::SetupMenu();
*/
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Visible Interest
		if ( l_pCharactersVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pCharactersVI );
			delete l_pCharactersVI;
			l_pCharactersVI = 0;
		}

		//	Unregister the Pick Interest
		if ( l_pCharactersPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pCharactersPI );
			delete l_pCharactersPI;
			l_pCharactersPI = 0;
		}

		//	Unregister the Pick Interest
		if ( l_pCharactersSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pCharactersSI );
			delete l_pCharactersSI;
			l_pCharactersSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pCharactersNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pCharactersNI );
			delete l_pCharactersNI;
			l_pCharactersNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pCharactersDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pCharactersDI );
			delete l_pCharactersDI;
			l_pCharactersDI = 0;
		}

		//	Unregister the Export Interest
		if ( l_pCharactersEI != 0 )
		{
			mexpMgr::UnRegisterExportInterest( l_pCharactersEI );
			delete l_pCharactersEI;
			l_pCharactersEI = 0;
		}

		//	Unregister the Memory Report Interest
		if ( l_pCharactersMI != 0 )
		{
			rptReportMemMgr::UnRegisterReportMemInterest( l_pCharactersMI );
			delete l_pCharactersMI;
			l_pCharactersMI = 0;
		}

		// Remove callback
		api3dSubdiv::UnRegisterSubdivInterest(&l_SubdivCallback);

		// Clean up namespaces
		chtrCommands::CleanUp();
		chtrObjectMgr::CleanUp();
		chtrExpressionsDialogUtil::CleanUp();
		chtrDialogUtil::CleanUp();
		chtrGeomList::CleanUp();
		chtrAnimList::CleanUp();
	}

}	// end of namespace
