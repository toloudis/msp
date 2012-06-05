/*****************************************************************************
**  fogSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/fogSystem.hpp"

#include "Systems/Fog/GUI/fogDialogInterest.hpp"
#include "Systems/Fog/GUI/fogDialogUtil.hpp"
#include "Systems/Fog/Data/fogDocumentInterest.hpp"
#include "Systems/Fog/Object/fogObjectMgr.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"


namespace fogSystem
{
	namespace
	{
		typedef cmmPickInterestTemplate<fogObjectMgr> fogPickInterest;

//		fogPickInterest*		l_pFogPI = 0;
		fogDialogInterest*		l_pFogDI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		//fogDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new fogDocumentInterest());

		// register the pick interest
//		if ( l_pFogPI == 0 )
//		{
//			l_pFogPI = new fogPickInterest();
//			pick3dMgr::RegisterPickInterest( l_pFogPI );
//		}

		// register the dialog interest
	/*	if ( l_pFogDI == 0 )
		{
			l_pFogDI = new fogDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pFogDI );
		}*/
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
//		if ( l_pFogPI != 0 )
//		{
//			pick3dMgr::UnRegisterPickInterest( l_pFogPI );
//			delete l_pFogPI;
//			l_pFogPI = 0;
//		}

		//	Unregister the dialog Interest
		/*if ( l_pFogDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pFogDI );
			delete l_pFogDI;
			l_pFogDI = 0;
		}*/

		// Clean up namespaces
		//fogDialogUtil::CleanUp();
	}

}	// end of namespace