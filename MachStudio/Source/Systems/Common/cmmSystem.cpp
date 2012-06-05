/*****************************************************************************
**  cmmSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/cmmSystem.hpp"

#include "Systems/Common/GUI/cmmCommands.hpp"
//#include "Systems/Common/GUI/cmmDialogUtil.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmPython.hpp"
#include "Systems/Common/cmmSelectInterest.hpp"
#include "Systems/Common/Spline/cmmSplineCommands.hpp"
#include "Systems/Common/Spline/cmmSplineDialogUtil.hpp"
#include "Systems/Common/Spline/cmmSplineSelectInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Common/TimeData/cmmTimeDocumentInterest.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace cmmSystem
{
	namespace
	{
		cmmSelectInterest*			l_pCommonSI = 0;
		cmmSplineSelectInterest*	l_pSplineSI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		cmmSystemDialogUtil::Init();
		cmmObjectDialogUtil::Init();
		//cmmDialogUtil::Init();
		cmmSplineDialogUtil::Init();
		cmmDialogInterestMgr::Init();

		// Add common chunk
		docSingleTypeMgr::AddDocumentInterest(new cmmTimeDocumentInterest());

		// register the select interest
		//
		if ( l_pCommonSI == 0 )
		{
			l_pCommonSI = new cmmSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pCommonSI );
		}

		if ( l_pSplineSI == 0 )
		{
			l_pSplineSI = new cmmSplineSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pSplineSI );
		}

		//
		//	commands
		//
		cmmCommands::SetupMenu();
		cmmSplineCommands::SetupMenu();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//
		//	commands
		//
		cmmCommands::CleanUp();

		//	Unregister the Select Interest
		if ( l_pCommonSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pCommonSI );
			delete l_pCommonSI;
			l_pCommonSI = 0;
		}
		if ( l_pSplineSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pSplineSI );
			delete l_pSplineSI;
			l_pSplineSI = 0;
		}

		// Clean up namespaces
		cmmSplineDialogUtil::CleanUp();
		//cmmDialogUtil::CleanUp();
		cmmObjectDialogUtil::CleanUp();
		cmmSystemDialogUtil::CleanUp();
	}

}	// end of namespace