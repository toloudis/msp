/*****************************************************************************
**  demG3dTestBumpMap.cpp
**
**		This mode displays a demonstration/test of texture bump mapping.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "envPlatform.hpp"

#include "demG3dTestBumpMap.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "matBumpMapUtil.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dPointLight.hpp"
#include "g3dLightManager.hpp"
#include "matMipTexture.hpp"
#include "g3dLayer.hpp"
#include "g2dWindow.hpp"
#include "matPlainTexture.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "matStaticCubeTexture.hpp"
#include "matTextureManager.hpp"
#include "matTextureManagerPAC.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"
#include "private\g2dSurfaceLoaderWin.hpp"
#include "appMouseEvent.hpp"
#include "appMouseEventHandler.hpp"

namespace
{

class demMouseHandler : public appMouseEventHandler
{
public:
	demMouseHandler()
	{
		m_X = 0;
		m_Y = 0;
	};

	~demMouseHandler()
	{
	};

	//====================================================================
	//	 ReceiveMouseUpEvent receives appMouseUpEvents.
	//====================================================================
	virtual void ReceiveMouseUpEvent( appMouseUpEvent& i_Event )
	{
		// Do not care about this event
	}

	//====================================================================
	//	ReceiveMouseDownEvent receives appMouseDownEvents.
	//====================================================================
	virtual void ReceiveMouseDownEvent( appMouseDownEvent& i_Event )
	{
		// Do not care about this event
	}

	//====================================================================
	//	ReceiveMouseMoveEvent receives appMouseMoveEvents.
	//====================================================================
	virtual void ReceiveMouseMoveEvent( appMouseMoveEvent& i_Event )
	{
		m_X = i_Event.GetX();
		m_Y = i_Event.GetY();
	}

	int GetX()
	{
		return m_X;
	}

	int GetY()
	{
		return m_Y;
	}

private:
	int	m_X;
	int	m_Y;
};

demMouseHandler* l_pDemMouse = NULL;
maMatrix4x4		l_BumpMatrix;

}

//====================================================================
//====================================================================
demG3dTestBumpMap::demG3dTestBumpMap(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL),
	m_CubeTexture(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestBumpMap::~demG3dTestBumpMap()
{
}

//====================================================================
//====================================================================
void demG3dTestBumpMap::Initialize()
{
	if ( !matTextureManager::GetEnvironmentBumpMappingSupported() )
		return;

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	l_pDemMouse = new demMouseHandler();

	demG3dTestMode::Initialize();
	SetYaw( 0.0 );
	SetPitch( maConstants::c_fAngleToRad * 1.0f );

	//	make fragments
	m_CubeFragment = g3dPrimitiveFragmentUtil::CreateCube(3.0f);
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f, 50, 50);
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(30.0f, 30.0f, 10, 10);

	//	setup fragment materials
	m_CubeMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_SphereMat.SetDiffuse(maFloatRGBA(0.8f, 0.8f, 1.0f, 1.0f));
	m_RectMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("orientvb.bmp"));

	fsLocator nmap_loc = gfPaths::GetGamePath(gfPaths::e_ExePath);
	nmap_loc.Push("data");
	nmap_loc.Push(itString("orientvn.nmp"));

	// create normal map and write it to .nmp file
	//int width, height;
	//envType::UInt8* bump_map = matBumpMapUtil::CreateNormalMapFromBMP( locator, 16.0f, width, height );
	//matBumpMapUtil::WriteNormalMap(nmap_loc, width, height, bump_map);
	//delete bump_map;

	m_BumpTexture = matTextureManager::LoadTexture( nmap_loc );

	// create bump reflection texture
	fsLocator rmap_loc = gfPaths::GetGamePath(gfPaths::e_ExePath);
	rmap_loc.Push("data");
	rmap_loc.Push(itString("orientvr.rmp"));

	// create reflection map
	//int width, height;
	//envType::UInt8* bump_map = matBumpMapUtil::CreateReflectionMapFromBMP( locator, width, height );
	//matBumpMapUtil::WriteReflectionMap(rmap_loc, width, height, bump_map);
	//delete bump_map;

	m_BumpReflTexture = matTextureManager::LoadTexture( rmap_loc );

	//locator.Push(itString("gold.bmp"));
	if( matTextureManager::GetStaticCubeMapSupported() )
	{
		fsLocator base = gfPaths::GetGamePath(gfPaths::e_ExePath);
		base.Push("data");
		fsLocator files[6];
		files[0] = files[1] = files[2] = files[3] = files[4] = files[5] = base;
		files[0].Push("View1.png");
		files[1].Push("View3.png");
		files[2].Push("View5.png");
		files[3].Push("View6.png");
		files[4].Push("View2.png");
		files[5].Push("View4.png");
		m_CubeTexture = matTextureManagerPAC::LoadStaticCubeTexture(files);
		m_CubeMat.AddTextureTop(m_CubeTexture);
	}

	m_RectTexture = matTextureManager::LoadTexture(locator);
	m_RectMat.AddTextureTop( m_BumpTexture, matMaterial::e_TexLayerBumpMapHeightMap );
	m_RectMat.AddTextureTop( m_RectTexture, matMaterial::e_TexLayerModulate );
	//m_RectMat.AddTextureTop( m_RectTexture, matMaterial::e_TexLayerNormal );

	// sphere gets reflection bump map
	m_SphereMat.AddTextureTop( m_BumpReflTexture, matMaterial::e_TexLayerBumpMapEnvironment );
	m_SphereMat.AddTextureTop( m_CubeTexture );


	m_CubeFragment->SetMaterial(&m_CubeMat);
	m_SphereFragment->SetMaterial(&m_SphereMat);
	m_RectFragment->SetMaterial(&m_RectMat);

	//	make models
	maMatrix4x4 world_mat;

	m_CubeModel = new g3dSceneNode(m_CubeFragment);
	m_Root->AddChild(m_CubeModel);
	m_SphereModel = new g3dSceneNode(m_SphereFragment);
	m_Root->AddChild(m_SphereModel);
	m_RectModel = new g3dSceneNode(m_RectFragment);
	m_Root->AddChild(m_RectModel);

	m_RectModel->GetTransform().MakeRotateX(-maConstants::c_fPI_Div_2);
	m_CubeModel->GetTransform().MakeTranslate(5, 10, 5);
	m_SphereModel->GetTransform().MakeTranslate(-5, 10, 5);

	//	set lights
	m_Light1 = g3dLightManager::CreateDirectionalLight();
	m_Light2 = g3dLightManager::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightManager::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestBumpMap::Think()
{
	if ( !matTextureManager::GetEnvironmentBumpMappingSupported() )
	{
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
		DBG_MESSAGE0( "Height map bump mapping is not supported, this demo will be skipped." );
		return;
	}

	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	// change light direction
	maVector3d dir;

	int width, height;
	m_Viewer.GetWindow()->GetDimensions( width, height );
	// the bump mapping light direction is in 3D world space.
	dir.Set( -2.0f * (l_pDemMouse->GetX() - width * 0.5f) / (float)width,
			 -1.0f,
			 -2.0f * (l_pDemMouse->GetY() - height * 0.5f) / (float)height );
	dir.Normalize();
	g3dLightManager::SetBumpMappingLightDirection( dir );

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestBumpMap::DeInitialize()
{
	if ( !matTextureManager::GetEnvironmentBumpMappingSupported() )
		return;

	delete l_pDemMouse;

	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_RectModel);
	//g3dRenderer::RemoveModel(m_CubeModel);
	//g3dRenderer::RemoveModel(m_SphereModel);

	//	cleanup fragments
	delete m_CubeFragment;
	delete m_SphereFragment;
	delete m_RectFragment;

	//	release textures
	if( m_CubeTexture )
		matTextureManagerPAC::DestroyTexture(m_CubeTexture);

	matTextureManager::ReleaseTexture(m_RectTexture);
	matTextureManager::ReleaseTexture(m_BumpTexture);
	matTextureManager::ReleaseTexture(m_BumpReflTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
