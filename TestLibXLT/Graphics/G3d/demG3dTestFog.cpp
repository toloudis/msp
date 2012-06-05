/*****************************************************************************
**  demG3dTestFog.cpp
**
**		This mode displays a demonstration/test of the fogging function of
**	the Terawatt engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestFog.hpp"

#include "Core/app/appTime.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <algorithm>

namespace
{

const maFloatRGBA c_FogColor(1.0f, 0.5f, 0.5f, 0.5f);
const float c_FogDensity = 0.05f;

}

//====================================================================
//====================================================================
demG3dTestFog::demG3dTestFog(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestFog::~demG3dTestFog()
{
}

//====================================================================
//====================================================================
void demG3dTestFog::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Fog test"));
	m_Viewer.SetTextMessage(4, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(5, itString("Press space to go to the next section"));


	//	make fragments
	m_CubeFragment = g3dPrimitiveFragmentUtil::CreateTexturedCube(3.0f);
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(30.0f, 30.0f, 10, 10);

	//	setup fragment materials
	effPhongData* pData = NULL;
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	pData = dynamic_cast<effPhongData*>(m_CubeMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 0.5f));
	pData->m_NameDiffuse = "lblue0132.png";
	pData->ReloadTextures(locator);
	m_CubeTexture = pData->m_TextureDiffuse;

	pData = dynamic_cast<effPhongData*>(m_RectMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.0f, 0.8f, 0.8f, 1.0f));

	m_CubeFragment->SetMaterial( &m_CubeMat );
	m_RectFragment->SetMaterial( &m_RectMat );

	//	make models
	m_WorldRect = new g3dSceneNode(m_RectFragment);
	m_Root->AddChild( m_WorldRect ); 

	maMatrix4x4 world_mat;
	world_mat.MakeRotateX( -maConstants::c_fPI_Div_2 );

	const int num_z = 20;
	const int num_x = 20;
	const float z_delta = 5.0f;
	const float x_delta = 5.0f;
	const float first_z = -(num_z / 2) * z_delta;
	const float first_x = -(num_x / 2) * x_delta;

	int x_num, z_num;

	for( z_num = 0 ; z_num < num_z ; z_num++ )
	{
		float zval = float(z_num) * z_delta + first_z;

		for( x_num = 0 ; x_num < num_x ; x_num++ )
		{
			float xval = float(x_num) * x_delta + first_x;

			world_mat.MakeTranslate(xval, 5.0f, zval);
			g3dSceneNode* new_model = new g3dSceneNode(m_CubeFragment, world_mat);
			m_Root->AddChild(new_model);
			m_Cubes.push_back(new_model);
		}
	}

	//	set lights
	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light2 = g3dLightMgr::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));

	m_Scene->SetFog( g3dType::e_FogModeLinear, c_FogColor, 
		10.0f, 50.0f, c_FogDensity);
}

//====================================================================
//	Think
//====================================================================
void demG3dTestFog::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

//	int red = c_FogColor.GetRed() * 255.0f;
//	int green = c_FogColor.GetGreen() * 255.0f;
//	int blue = c_FogColor.GetBlue() * 255.0f;


	//	set the orientations of our cubes

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestFog::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup models
	//std::for_each(m_Cubes.begin(), m_Cubes.end(), g3dScene::RemoveModel);
	//g3dScene::RemoveModel(m_WorldRect);

	//	cleanup fragments
	delete m_CubeFragment;
	delete m_RectFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_CubeTexture);

	demG3dTestMode::DeInitialize();

//	g3dScene::EnableFog(false);
//	g3dScene::SetFogMode( g3dType::e_FogModeNone );

	m_Viewer.ClearTextMessages();

	delete m_Root;
	delete m_Scene;
}
