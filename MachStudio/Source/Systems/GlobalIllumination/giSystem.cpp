/*****************************************************************************
**  giSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/giSystem.hpp"

#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"

#include "Systems/GlobalIllumination/GUI/giDialogInterest.hpp"
#include "Systems/GlobalIllumination/GUI/giDialogUtil.hpp"
#include "Systems/GlobalIllumination/Data/giDocumentInterest.hpp"
#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"
#include "Systems/GlobalIllumination/Object/giRendermanExportInterest.hpp"
#include "Systems/GlobalIllumination/Object/giMRayExportInterest.hpp"
#include "Systems/GlobalIllumination/Timeline/giDriverCreator.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"

#include "Support/pyth/pythProperty.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"


namespace giSystem
{
	namespace
	{
		typedef cmmPickInterestTemplate<giObjectMgr> giPickInterest;

//		aoPickInterest*					l_pFogPI = 0;
		giDialogInterest*				l_pFogDI = 0;
		giRendermanExportInterest*		l_pFogREI = 0;
		giMRayExportInterest*			l_pFogMRayEI = 0;
		giGIObject*						l_pGIObject = 0;
		
		//====================================================================
		//====================================================================
		class GINameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve the string "CapturePrefs" into our property object
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("GI"))
				{
					return l_pGIObject;
				}
				return NULL;
			}
		};

		shared_ptr<GINameResolver> l_NameResolver(new GINameResolver);
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		giDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new giDocumentInterest());

		// register the pick interest
//		if ( l_pFogPI == 0 )
//		{
//			l_pFogPI = new aoPickInterest();
//			pick3dMgr::RegisterPickInterest( l_pFogPI );
//		}

		// register the dialog interest
		if ( l_pFogDI == 0 )
		{
			l_pFogDI = new giDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pFogDI );
		}

		// register the renderman interest
		if ( l_pFogREI == 0 )
		{
			l_pFogREI = new giRendermanExportInterest();
			rmanMgr::RegisterExportInterest( l_pFogREI );
		}

		// register the mray interest
		if ( l_pFogMRayEI == 0 )
		{
			l_pFogMRayEI = new giMRayExportInterest();
			mrayMgr::RegisterExportInterest( l_pFogMRayEI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new giDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		giDriverCreator::CreateParsers();

		giObjectMgr::AddObject(giScriptData());
		l_pGIObject = giObjectMgr::GetPickObject();

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

		giObjectMgr::DeleteObject();

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

		//	Unregister the renderman Interest
		if ( l_pFogREI != 0 )
		{
			rmanMgr::UnRegisterExportInterest( l_pFogREI );
			delete l_pFogREI;
			l_pFogREI = 0;
		}


		//	Unregister the mray Interest
		if ( l_pFogMRayEI != 0 )
		{
			mrayMgr::UnRegisterExportInterest( l_pFogMRayEI );
			delete l_pFogMRayEI;
			l_pFogMRayEI = 0;
		}

		// Clean up namespaces
		giDialogUtil::CleanUp();
	}

}	// end of namespace