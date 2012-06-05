/********************************************************************************************\
**  lodLevel.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "lodLevel.hpp"

#include "lodErrorHandler.hpp"

#include "api3dImport.hpp"
#include "api3dLightMgr.hpp"
#include "api3dLODUtil.hpp"
#include "api3dObjectEntity.hpp"
#include "api3dObjectGeom.hpp"
#include "api3dObjectSimple.hpp"
#include "api3dShape.hpp"
#include "api3dScene.hpp"
#include "cam3dMgr.hpp"
#include "lod3dFileUtil.hpp"

#include "appSimTime.hpp"
#include "dbgLog.hpp"
#include "entEntity.hpp"
#include "entLODModelTemplate.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dFragment.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "gfPaths.hpp"
#include "maConstants.hpp"



//------------------------------------------------------------------------
// Local variables and functions
//------------------------------------------------------------------------
namespace
{
	lod3dData	l_Data;

	lodLODChangedCallback *	l_ChangedCallback = NULL;

	api3dObject*		l_pObject	= NULL;
	api3dObjectSimple*	l_pGround	= NULL;

	g3dDirectionalLight* l_pLight1	= NULL;
	g3dDirectionalLight* l_pLight2	= NULL;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void make_environment()
{
	//	the ground
	//
	fsLocator texture_locator = gfPaths::GetPath( gfPaths::e_ExePath );
	texture_locator.Push("Data");
	texture_locator.Push("ground.png");

	l_pGround = api3dShape::CreateTexturedRectangle( texture_locator,
							maFloatRGBA(1.0f,1.0f,1.0f,1.0f),
							50.0f,
							50.0f,
							2,
							2,
							false );
	maRotation rotation;
	rotation.SetValue( maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2 );

	l_pGround->SetPosition( maPoint3d( 0.0f, 0.0f, 0.0f ) );
	l_pGround->SetOrientation( rotation );
	l_pGround->SetScale( maVector3d(1,1,1) );

	api3dScene::AddObject( l_pGround );

	//	set lights
	//
	l_pLight1 = api3dLightMgr::CreateDirectionalLight();
	l_pLight1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	//l_pLight1->SetIntensity(maFloatRGBA(0.4f, 0.7f, 0.2f, 1.0f));
	l_pLight1->SetIntensity(maFloatRGBA(1.0f,1.0f,1.0f,1.0f));
	l_pLight1->Enable();

	l_pLight2 = api3dLightMgr::CreateDirectionalLight();
	l_pLight2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));
	l_pLight2->SetIntensity(maFloatRGBA(1.0f,1.0f,1.0f,1.0f));
	//l_pLight2->SetIntensity(maFloatRGBA(0.3f, 0.6f, 0.8f, 1.0f));
	l_pLight2->Enable();

	//api3dLightMgr::SetAmbient(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
}

//
void deinitialize_objects()
{
	if ( l_pGround != 0 )
	{
		api3dScene::RemoveObject(l_pGround);
		delete l_pGround;
		l_pGround = 0;
	}

	if ( l_pObject != 0 )
	{
		api3dScene::RemoveObject(l_pObject);
		delete l_pObject;
		l_pObject = NULL;
	}

	if ( l_pLight1 )
		api3dLightMgr::DestroyLight( l_pLight1 );
	if ( l_pLight2 )
		api3dLightMgr::DestroyLight( l_pLight2 );
}

}	// namespace


//------------------------------------------------------------------------
//	Initialize()
//------------------------------------------------------------------------
void
lodLevel::Initialize()
{
//	lodLODTemplate::Init();

	make_environment();

	l_pObject = NULL;
//	l_pObject = new api3dObjectEntity( new entLODModelTemplate(), NULL );

//entLODModelTemplate* lodmodel_template = new entLODModelTemplate;
//
//std::string tempstring;
//fsFileUtil::LocatorToANSIFilename( i_Locator, tempstring );
//DBG_LOG1( "api3d lod path (%s)", tempstring.c_str() );
//
////	read in the LOD file
////
//lod3dData lodData;
//lod3dFileUtil::Read( i_Locator, lodData );
//
////	for each model file create a model template
////
//int j;
//for ( j = 0 ; j < lodData.m_NumberOfLevels ; j++ )
//{
//	entModelTemplate* model_template;
//
//	model_template = load_model_template( i_Locator, lodData.m_LODLevels[j].m_ModelFile );
//
//	if (model_template != NULL)
//	{
//		lodmodel_template->AddTemplate( model_template, 
//										lodData.m_LODLevels[j].m_ModelFile, 
//										lodData.m_LODLevels[j].m_fStartDistance, 
//										lodData.m_LODLevels[j].m_bEditorDefault );
//	}
//}
//
////	now create an entEntity.
////
//if ( lodmodel_template )
//{
//	entEntity* pEnt = new entEntity( *lodmodel_template );
//	entEntityTemplate* pEntTemp;
//	pEntTemp = dynamic_cast<entEntityTemplate*>(lodmodel_template);
//	DBG_ASSERT0( pEntTemp != 0, "Invalid entity template" );
//
//	return new api3dObjectEntity( pEntTemp, pEnt );
//}

	// Initialize the data
	//
	l_Data.m_LODLevels[0].m_bEditorDefault = true;
	l_Data.m_LODLevels[0].m_fStartDistance =  0.0f;
	l_Data.m_LODLevels[1].m_bEditorDefault = false;
	l_Data.m_LODLevels[1].m_fStartDistance = 20.0f;
	l_Data.m_LODLevels[2].m_bEditorDefault = false;
	l_Data.m_LODLevels[2].m_fStartDistance = 60.0f;
}

//------------------------------------------------------------------------
//	DeInitialize()
//------------------------------------------------------------------------
void
lodLevel::DeInitialize()
{
//	lodLODTemplate::CleanUp();

	deinitialize_objects();

	Clear();
}

//----------------------------------------------------------------------------
//	SetLODChangedCallback
//----------------------------------------------------------------------------
void lodLevel::SetLODChangedCallback( lodLODChangedCallback *i_Callback )
{
	l_ChangedCallback = i_Callback;
}

//----------------------------------------------------------------------------
//	Think - Handle material animation timing
//----------------------------------------------------------------------------
void lodLevel::Think()
{
	if ( l_Data.m_bReloadObject )
	{
		lodLevel::LoadModel( l_Data.m_LODFilename );
		l_Data.m_bReloadObject = false;
	}
	if ( l_Data.m_bReloadDataOnly )
	{
		UpdateObjectData();
		l_Data.m_bReloadDataOnly = false;
	}
}

//------------------------------------------------------------------------
//	Clear
//------------------------------------------------------------------------
void
lodLevel::Clear()
{
	if (l_pObject)
		api3dScene::RemoveObject(l_pObject);
	delete l_pObject;
	l_pObject = NULL;
}


//------------------------------------------------------------------------
//	Save saves a level to a given locator
//------------------------------------------------------------------------
void
lodLevel::Save(const fsLocator& i_Locator)
{
	lod3dFileUtil::Write( i_Locator, l_Data );
}

//------------------------------------------------------------------------
//	Load()
//------------------------------------------------------------------------
void
lodLevel::Load(const fsLocator& i_Locator)
{
	LoadModel( i_Locator );
}

//------------------------------------------------------------------------
//	show the ground or not.
//------------------------------------------------------------------------
void lodLevel::ShowGround( bool i_bRenderGround )
{
	l_pGround->SetRenderable( i_bRenderGround );
}

//------------------------------------------------------------------------
// Focus camera on bounding box of object.
//------------------------------------------------------------------------
void lodLevel::FocusCamera()
{
	if (l_pObject)
	{
		cam3dMgr::FocusCamera(l_pObject->GetWorldBox());
	}
}

//------------------------------------------------------------------------
//	LoadModel loads geometry from the given locator
//------------------------------------------------------------------------
void lodLevel::LoadModel(const fsLocator& i_Locator)
{
	Clear();

	try
	{
		if ( l_pObject != 0 )
		{
			delete l_pObject;
		}

		l_pObject = api3dLODUtil::LoadObject( i_Locator );
		
		//UpdateObjectData();

		api3dScene::AddObject(l_pObject);

		FocusCamera();
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		lodErrorHandler *handler = lodErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
	}
	catch( ... )
	{
		lodErrorHandler *handler = lodErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

	//	now grab the data out of the object
	//
	api3dObjectEntity* pOE = dynamic_cast<api3dObjectEntity*>( l_pObject );
	if ( pOE != 0 )
	{
		api3dLODUtil::ExtractLODDataFromObject( pOE, l_Data );
	}
}

//------------------------------------------------------------------------
//	UpdateObjectData() - update the data for the current object
//------------------------------------------------------------------------
void lodLevel::UpdateObjectData()
{
	api3dObjectEntity* pOE = dynamic_cast<api3dObjectEntity*>( l_pObject );
	if ( pOE != 0 )
	{
		api3dLODUtil::SetLODDataOfObject( pOE, l_Data );
	}
}

//------------------------------------------------------------------------
//	Get the data structure for the lod.
//------------------------------------------------------------------------
lod3dData& lodLevel::GetData()
{
	return l_Data;
}
