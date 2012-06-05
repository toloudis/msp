/*****************************************************************************
**  aoSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/aoSystem.hpp"

#include "Systems/AmbientOcclusion/GUI/aoDialogInterest.hpp"
#include "Systems/AmbientOcclusion/GUI/aoDialogUtil.hpp"
#include "Systems/AmbientOcclusion/Data/aoDocumentInterest.hpp"
#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"


namespace aoSystem
{
	namespace
	{
		typedef cmmPickInterestTemplate<aoObjectMgr> aoPickInterest;

//		aoPickInterest*		l_pFogPI = 0;
		aoDialogInterest*		l_pFogDI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		aoDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new aoDocumentInterest());

		// register the pick interest
//		if ( l_pFogPI == 0 )
//		{
//			l_pFogPI = new aoPickInterest();
//			pick3dMgr::RegisterPickInterest( l_pFogPI );
//		}

		// register the dialog interest
		if ( l_pFogDI == 0 )
		{
			l_pFogDI = new aoDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pFogDI );
		}
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
		if ( l_pFogDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pFogDI );
			delete l_pFogDI;
			l_pFogDI = 0;
		}

		// Clean up namespaces
		aoDialogUtil::CleanUp();
	}

}	// end of namespace