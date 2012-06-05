/*****************************************************************************
**  xtraPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/xtraPackage.hpp"

#include "Support/xtra/xtraDriverCreator.hpp"
#include "Support/xtra/xtraPython.hpp"
#include "Support/xtra/xtraSelectInterest.hpp"
#include "Support/xtra/GUI/xtraCommands.hpp"

#include "Tool/sel3d/sel3dMgr.hpp"

namespace xtraPackage
{

	namespace
	{
		xtraSelectInterest*	l_pPropertiesSI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Submit python commands
		xtraPython::AddCommands("mach");

		// register the select interest
		if ( l_pPropertiesSI == 0 )
		{
			l_pPropertiesSI = new xtraSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pPropertiesSI );
		}

		// Create commands to enable hot keys
		xtraCommands::SetupMenu();

		// Timeline related creators
		tmlnCreator::AddDriverCreator(new xtraDriverCreator);
		xtraDriverCreator::CreateParsers();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pPropertiesSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pPropertiesSI );
			delete l_pPropertiesSI;
			l_pPropertiesSI = 0;
		}

	}

}	// end of namespace