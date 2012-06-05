/*****************************************************************************
**  aoSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/aoSystem.hpp"

#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"

#include "Systems/AmbientOcclusion/GUI/aoDialogInterest.hpp"
#include "Systems/AmbientOcclusion/GUI/aoDialogUtil.hpp"
#include "Systems/AmbientOcclusion/Data/aoDocumentInterest.hpp"
#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"
#include "Systems/AmbientOcclusion/Object/aoRendermanExportInterest.hpp"
#include "Systems/AmbientOcclusion/Object/aoMRayExportInterest.hpp"
#include "Systems/AmbientOcclusion/Timeline/aoDriverCreator.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"

#include "Support/pyth/pythProperty.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"


namespace aoSystem
{
	namespace
	{
		typedef cmmPickInterestTemplate<aoObjectMgr> aoPickInterest;

//		aoPickInterest*				l_pFogPI = 0;
		aoDialogInterest*			l_pFogDI = 0;
		aoRendermanExportInterest*	l_pFogREI = 0;
		aoMRayExportInterest*		l_pFogMRayEI = 0;
		aoAOObject*					l_pAOObject = 0;

		//====================================================================
		//====================================================================
		class AONameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve the string "CapturePrefs" into our property object
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("AO"))
				{
					return l_pAOObject;
				}
				return NULL;
			}
		};

		shared_ptr<AONameResolver> l_NameResolver(new AONameResolver);
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
		if ( l_pFogREI == 0 )
		{
			l_pFogREI = new aoRendermanExportInterest();
			rmanMgr::RegisterExportInterest( l_pFogREI );
		}
		if ( l_pFogMRayEI == 0 )
		{
			l_pFogMRayEI = new aoMRayExportInterest();
			mrayMgr::RegisterExportInterest( l_pFogMRayEI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new aoDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		aoDriverCreator::CreateParsers();

		aoObjectMgr::AddObject(aoScriptData());
		l_pAOObject = aoObjectMgr::GetPickObject();
		// Add name resolver to match a string to our prtyObject
		pythProperty::AddNameResolver( l_NameResolver ); 
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		// Remove name resolver 
		pythProperty::RemoveNameResolver( l_NameResolver ); 

		aoObjectMgr::DeleteObject();

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
		if ( l_pFogREI != 0 )
		{
			rmanMgr::UnRegisterExportInterest( l_pFogREI );
			delete l_pFogREI;
			l_pFogREI = 0;
		}
		if ( l_pFogMRayEI != 0 )
		{
			mrayMgr::UnRegisterExportInterest( l_pFogMRayEI );
			delete l_pFogMRayEI;
			l_pFogMRayEI = 0;
		}

		l_pAOObject = NULL;

		// Clean up namespaces
		aoDialogUtil::CleanUp();
	}

}	// end of namespace