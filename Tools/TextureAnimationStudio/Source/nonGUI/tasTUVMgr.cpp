/********************************************************************************************\
**  tasTUVMgr.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "tasTUVMgr.hpp"

#include "tasApp.hpp"		// circular dependency
#include "tasTUVData.hpp"
#include "tasTUVDataUtil.hpp"

#include "api3dLightMgr.hpp"
#include "api3dObject.hpp"
#include "api3dObjectSimple.hpp"
#include "api3dScene.hpp"
#include "api3dShape.hpp"
#include "dbgLog.hpp"
#include "effPhongData.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g3dConstants.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dFragment.hpp"
#include "g3dSceneGlobal.hpp"
#include "gfPaths.hpp"
#include "itStringUtil.hpp"
#include "matMaterial.hpp"
#include "matTextureMgr.hpp"
#include "matUVATexture.hpp"

#undef CreateFile
#undef DeleteFile


//------------------------------------------------------------------------
// Local variables and functions
//------------------------------------------------------------------------
namespace
{
	tasAnimatedTextureChangedCallback *	l_ChangedCallback = NULL;

	std::vector<fsLocator> l_TUVTextureNames;
	matMaterial*	l_pMaterial;
	matUVATexture*	l_pUVATexture;

//	matTexture*							l_pCurrentTexture;
//	fsLocator							l_CurrentTextureLocator;
//	fsLocator							l_CurrentGeometryLocator;
//	maPoint3d l_Center(0,0,0);
	g3dDirectionalLight* l_pDirLight = NULL; 
//	g3dPointLight *l_pPointLight1 = NULL; 
//	api3dObject *l_pPointObj1 = NULL;
//	g3dProjectedLight *l_pProjectedLight = NULL; 
//	api3dProjectedLightWrapper *l_pPrjLightWrapper = NULL;
	api3dObjectSimple* l_pProjObj = NULL;

}	// namespace


//------------------------------------------------------------------------
//	Initialize()
//------------------------------------------------------------------------
void tasTUVMgr::Initialize()
{
	SetToDefault();

	l_pUVATexture = matTextureMgr::CreateUVATexture();
	SetStartTime();

	l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	l_pDirLight->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	////l_pDirLight->SetIntensity(maFloatRGBA(0.65f, 0.65f, 0.6f, 1.0f));
	l_pDirLight->SetIntensity(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
	l_pDirLight->Enable();
	l_pDirLight->SetCastsShadow(false);

	//l_pPointLight1 = api3dLightMgr::CreatePointLight();
	//l_pPointLight1->SetPosition(maVector3d(1.2f, -1.0f, -1.0f));
	////l_pPointLight1->SetIntensity(maFloatRGBA(0.65f, 0.6f, 0.6f, 1.0f));
	//l_pPointLight1->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	//l_pPointLight1->Enable();
	//l_pPointLight1->SetCastsShadow(true);

	//l_pPointObj1 = api3dShape::CreateSphere(l_pPointLight1->GetIntensity(), 0.25f, 6, 6);
	//api3dScene::AddObject(l_pPointObj1);

	//l_pProjectedLight = api3dLightMgr::CreateProjectedLight();
	//l_pProjectedLight->SetPosition(maVector3d(1.2f, -1.0f, -1.0f));
	//l_pProjectedLight->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	//l_pProjectedLight->Disable();				// off by default
	//l_pProjectedLight->SetCastsShadow(true);

	//l_pPrjLightWrapper = new api3dProjectedLightWrapper(*l_pProjectedLight);
	//fsLocator tex_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	//tex_loc.Push("projection.bmp");
	//if (fsFileUtil::FileExists(tex_loc))
	//{
	//	matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc);
	//	l_pPrjLightWrapper->SetTexture(pTexture); // ownership of texture passes to wrapper
	//}
	//else 
	//{
	//	tex_loc.Pop();
	//	tex_loc.Push("Data");
	//	tex_loc.Push("Stock");
	//	tex_loc.Push("Effects");
	//	tex_loc.Push("General");
	//	tex_loc.Push("Textures");
	//	tex_loc.Push("projection.dds");
	//	if (fsFileUtil::FileExists(tex_loc))
	//	{
	//		matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc);
	//		l_pPrjLightWrapper->SetTexture(pTexture); // ownership of texture passes to wrapper
	//	}
	//}
	//l_pPrjLightWrapper->OrientCamera();

	fsLocator texture_locator = gfPaths::GetPath( gfPaths::e_ExePath );
	texture_locator.Push("Data");
	texture_locator.Push("ground.png");

	//std::string szFilename;			// debug only
	//fsFileUtil::LocatorToANSIFilename( texture_locator, szFilename );
	//DBG_LOG1("make_environment: %s", szFilename.c_str() );

	//l_pProjObj = api3dShape::CreateTexturedRectangle(l_pProjectedLight->GetIntensity(), 0.25f, 0.25f, 6);
	//api3dScene::AddObject(l_pProjObj);
	//l_pProjObj->SetRenderable(false);

	l_pProjObj = api3dShape::CreateTexturedRectangle( texture_locator, 
							maFloatRGBA(0.5f,0.5f,0.5f,1.0f),
							2.0f, 
							2.0f, 
							2,
 							2 );

	////g3dFragmentManager::SetMaterial(l_Fragment, &l_Material, 0);
	////g3dHFragment hfrag(l_Fragment);
	////hfrag.GetMatrix().MakeTranslate(-0.4f, 0.10f, 0.5);
	////l_Model = g3dRenderer::AddDynamicModel(hfrag);
	////l_Model->SetRenderSpace(g3dModel::e_ScreenSpace);
	////l_Model->SetRenderable(true);

	//maRotation rotation;
	//rotation.SetValue( maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2 );
	//l_pGround->SetPosition( maPoint3d( 0.0f, 0.0f, 0.0f ) );
	//l_pGround->SetOrientation( rotation );
	//l_pGround->SetScale( maVector3d(1,1,1) );
	api3dScene::AddObject( l_pProjObj, tasApp::GetScreenSpaceIndex() );

	//	the ground
	//
	//fsLocator texture_locator = gfPaths::GetPath( gfPaths::e_ExePath );
	//texture_locator.Push("Data");
	//texture_locator.Push("ground.png");

	//std::string szFilename;			// debug only
	//fsFileUtil::LocatorToANSIFilename( texture_locator, szFilename );
	//DBG_LOG1("make_environment: %s", szFilename.c_str() );

	//l_pGround = api3dShape::CreateTexturedRectangle( texture_locator, 
	//						maFloatRGBA(0.5f,0.5f,0.5f,0.5f),
	//						50.0f, 
	//						50.0f, 
	//						2,
	//						2 );
	//maRotation rotation;
	//rotation.SetValue( maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2 );

	//l_pGround->SetPosition( maPoint3d( 0.0f, 0.0f, 0.0f ) );
	//l_pGround->SetOrientation( rotation );
	//l_pGround->SetScale( maVector3d(1,1,1) );

	//api3dScene::AddObject( l_pGround );

	////	set lights
	////
	//l_pLight1 = api3dLightMgr::CreateDirectionalLight();
	//l_pLight2 = api3dLightMgr::CreateDirectionalLight();

	//l_pLight1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	//l_pLight2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	//l_pLight1->SetIntensity(maFloatRGBA(0.4f, 0.7f, 0.2f, 1.0f));
	//l_pLight2->SetIntensity(maFloatRGBA(0.3f, 0.6f, 0.8f, 1.0f));

	//l_pLight1->Enable();
	//l_pLight2->Enable();
}

//------------------------------------------------------------------------
//	DeInitialize()
//------------------------------------------------------------------------
void tasTUVMgr::DeInitialize()
{
	delete l_pUVATexture;
	l_pUVATexture = 0;

	api3dLightMgr::DestroyLight(l_pDirLight);
	l_pDirLight = NULL;
	//api3dLightMgr::DestroyLight(l_pPointLight1);
	//l_pPointLight1 = NULL;
	//api3dLightMgr::DestroyLight(l_pProjectedLight);
	//l_pProjectedLight = NULL;
	//delete l_pPrjLightWrapper;
	//l_pPrjLightWrapper = NULL;

	//api3dScene::RemoveObject(l_pPointObj1);
	//delete l_pPointObj1;
	//l_pPointObj1 = NULL;

	api3dScene::RemoveObject(l_pProjObj);
	delete l_pProjObj;
	l_pProjObj = NULL;

	Clear();
}

//----------------------------------------------------------------------------
//	SetAnimatedTextureChangedCallback
//----------------------------------------------------------------------------
void tasTUVMgr::SetAnimatedTextureChangedCallback( tasAnimatedTextureChangedCallback *i_Callback )
{
	l_ChangedCallback = i_Callback;
}

//----------------------------------------------------------------------------
//	Think - Handle material animation timing
//----------------------------------------------------------------------------
void tasTUVMgr::Think()
{
}

//------------------------------------------------------------------------
//	Clear
//------------------------------------------------------------------------
void
tasTUVMgr::Clear()
{
	// Call user callback to update interface
	//if (l_ChangedCallback)
	//	l_ChangedCallback->ModelChange();
}

//------------------------------------------------------------------------
//	Save a TUV
//------------------------------------------------------------------------
void
tasTUVMgr::SaveTUV(const fsLocator& i_Locator)
{
	//	Saving a TUV
	//
	try
	{
		//	save
		std::vector<itString> texture_names;
		int i;
		int num = l_TUVTextureNames.size();

		if( num == 0 )
			throw itString("You must add a texture first.");

		for( i = 0 ; i < num ; i++ )
		{
			texture_names.push_back(l_TUVTextureNames[i].GetLastName());

			std::string fname;
			fsFileUtil::LocatorToANSIFilename( l_TUVTextureNames[i],fname );
			DBG_LOG2("%d (%s)", i, fname.c_str() );
		}

		if( fsFileUtil::FileExists(i_Locator) )
			fsFileUtil::DeleteFile(i_Locator);

		fsFileUtil::CreateFile(i_Locator);

		num = texture_names.size();
		for( i = 0 ; i < num ; i++ )
		{
			std::string fname;
			fname = itStringUtil::GetStdString(texture_names[i]);
			DBG_LOG2("%d (%s)", i, fname.c_str() );
		}

		matTextureMgr::WriteUVATexture(*l_pUVATexture, i_Locator, &(texture_names[0]));

		//..

//		l_CurFilename = i_Locator;
//		l_UVAFilename->SetText(i_Locator.GetLastName());

//		refresh_data();

//		l_UVADirty = false;
//		l_StatusLine->SetTextColor(l_IdleText, guiWindow::e_Idle);
//		l_StatusLine->SetText(itString("File saved."));
	}
	catch( const itString& /*i_String*/ )
	{
//		l_StatusLine->SetTextColor(l_ErrorText, guiWindow::e_Idle);
//		l_StatusLine->SetText(i_String);
	}
	catch( const fsReadOnlyX& )
	{
//		l_StatusLine->SetTextColor(l_ErrorText, guiWindow::e_Idle);
//		l_StatusLine->SetText(itString("That file is read-only."));
	}
}

//------------------------------------------------------------------------
//	LoadTUV()	
//------------------------------------------------------------------------
void
tasTUVMgr::LoadTUV(const fsLocator& i_Locator)
{
	try
	{
		//	get rid of old uva
		//if (l_pUVATexture != 0)
		//{
		//	api3dScene::RemoveObject(l_pProjObj);
		//	delete l_pUVATexture;
		//}

		//	read data
		//	we assume all the texture files are in the same directory as the .tuv file
		std::vector<itString> names;
		l_pUVATexture = matTextureMgr::ReadUVATexture(i_Locator, names);

		tasTUVData& data = tasTUVDataUtil::Data();

		data.m_TUVFilename	= i_Locator;
		data.m_WidthFrames	= l_pUVATexture->GetNumWidthFrames();
		data.m_HeightFrames = l_pUVATexture->GetNumHeightFrames();
		data.m_NumFrames	= l_pUVATexture->GetNumFrames();
		data.m_Rate			= l_pUVATexture->GetFrameRate();
		data.m_bLooping		= l_pUVATexture->GetLooping();
		data.m_bReversing	= l_pUVATexture->GetReversing();

		fsLocator texture_dir = i_Locator;
		texture_dir.Pop();

		int i;
		int num = names.size();
		data.m_Filenames.resize(num);
		for( i = 0 ; i < num ; i++ )
		{
			data.m_Filenames[i].m_Filename = itStringUtil::GetStdString(names[i]);
		}

		//l_CurFilename = i_Locator;
		//l_UVAFilename->SetText(i_Locator.GetLastName());
		//l_Material.RemoveTextures();
		//l_Material.AddTextureTop(l_pUVATexture);
	}
	catch( const fsFileDoesntExistX& )
	{
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasTUVMgr::AddTUVTextureFilename(fsLocator& i_TFilename)
{
	fsLocator dir(i_TFilename);
	itString fname(i_TFilename.GetLastName());
	dir.Pop();

	//	debug only
	std::string texpath;
	fsFileUtil::LocatorToANSIFilename(i_TFilename, texpath);
	DBG_LOG1("texture loading (%s)", texpath.c_str());

	//	load the texture
	//
	const bool c_MIPMAP_IF_2D = true;
	const bool c_IGNORE_IF_TUV_EXISTS = true;
	matTexture* pTexture = matTextureMgr::LoadTexture(dir, fname, c_MIPMAP_IF_2D, c_IGNORE_IF_TUV_EXISTS);
	if (pTexture == 0)
		return;

	//	remove all the texure pages (only supporting one)
	//
	if (l_pUVATexture != 0)
	{
		l_pUVATexture->RemoveAllTexturePages();
		l_pUVATexture->AddTexturePage(pTexture);
	}

	l_TUVTextureNames.clear();
	l_TUVTextureNames.push_back(i_TFilename);

	l_pMaterial = l_pProjObj->Fragment()->GetMaterial();
	l_pMaterial->RemoveTextures();
	l_pMaterial->AddTextureTop(l_pUVATexture);
	l_pMaterial->TypedData<effPhongData>()->m_TextureDiffuse = l_pUVATexture;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasTUVMgr::ClearTUVTextureFilenames()
{
	l_TUVTextureNames.clear();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasTUVMgr::SetStartTime()
{
	if (l_pUVATexture != 0)
	{
		// HACK - I don't like the frametime variable here
		l_pUVATexture->SetStartTime(g3dSceneGlobal::g_FrameTime);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasTUVMgr::DataChanged()
{
	tasTUVData& data = tasTUVDataUtil::Data();

	l_pUVATexture->SetNumWidthFrames(data.m_WidthFrames);
	l_pUVATexture->SetNumHeightFrames(data.m_HeightFrames);
	l_pUVATexture->SetFrameRate(data.m_Rate);
	l_pUVATexture->SetLooping(data.m_bLooping);
	l_pUVATexture->SetReversing(data.m_bReversing);
	l_pUVATexture->SetNumFrames(data.m_NumFrames);
//	data.m_Filenames[i].m_Filename = itStringUtil::GetStdString(names[i]);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tasTUVMgr::SetToDefault()
{
	tasTUVData& data = tasTUVDataUtil::Data();

	data.m_TUVFilename.Clear();
	data.m_WidthFrames	= 1;
	data.m_HeightFrames = 1;
	data.m_NumFrames	= 0;
	data.m_Rate			= g3dConstants::c_fDefaultFrameRate;
	data.m_bLooping		= true;
	data.m_bReversing	= false;
	data.m_Filenames.resize(0);
}
