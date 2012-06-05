/*****************************************************************************
**  dynPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/dyn/dynPackage.hpp"

#include "Support/dyn/GUI/dynDialogUtil.hpp"
#include "Support/dyn/dynDriverCreator.hpp"
#include "Support/dyn/dynSelectInterest.hpp"

#include "Tool/sel3d/sel3dMgr.hpp"

namespace dynPackage
{

	namespace
	{
		dynSelectInterest*	l_pControlsSI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		dynDialogUtil::Init();

		// register the select interest
		if ( l_pControlsSI == 0 )
		{
			l_pControlsSI = new dynSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pControlsSI );
		}

		// Timeline related creators
		tmlnCreator::AddDriverCreator(new dynDriverCreator);
		dynDriverCreator::CreateParsers();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{

		//	Unregister the Select Interest
		if ( l_pControlsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pControlsSI );
			delete l_pControlsSI;
			l_pControlsSI = 0;
		}

		// Clean up namespaces
		dynDialogUtil::CleanUp();
	}

}	// end of namespace