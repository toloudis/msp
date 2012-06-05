/*****************************************************************************
**  demG3dTestVertexColoring.hpp
**
**		This mode displays a demonstration/test of vertex coloring
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestVertexColoring.hpp"

#include "appTime.hpp"
#include "appSimTime.hpp"
#include "demModeManager.hpp"
#include "g2dRGBColor.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLayer.hpp"
#include "g3dLightManager.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "maConstants.hpp"
#include "triMeshPreLitFrag.hpp"

#include <algorithm>

namespace
{
	maFloatRGBA l_DiffuseColors[3];
	int l_ColorIndices[3] = { 0, 1, 2 };
}

//====================================================================
//====================================================================
demG3dTestVertexColoring::demG3dTestVertexColoring(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL),
	m_Fragment( 0 ),
	m_pColorAnim1( NULL ),
	m_pColorAnim2( NULL ),
	m_pColorAnim3( NULL )
{
}

//====================================================================
//====================================================================
demG3dTestVertexColoring::~demG3dTestVertexColoring()
{
}

//====================================================================
//====================================================================
void demG3dTestVertexColoring::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	bool prelit = true;
	g3dLayer *prelit_layer = new g3dLayer(m_Root, g3dLayer::e_ZBuffer,
		g3dLayer::e_World, g3dLayer::e_Multiplicative, true, false, prelit);
	m_Scene = new g3dScene(prelit_layer); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// Vertices
	maPoint3d vertices[4];
	vertices[0].Set(  0.0f,  0.5f, 0.0f );
	vertices[1].Set( -0.58f, -0.5f, 0.0f );
	vertices[2].Set(  0.58f, -0.5f, 0.0f );

	// Normals
	maPoint3d normals[4];
	normals[0].Set( 0, 0, 1 );
	normals[1].Set( 0, 0, 1 );
	normals[2].Set( 0, 0, 1 );

	maPoint2d uvs[4];
	uvs[0].Set(0.0f, 0.0f);
	uvs[0].Set(0.0f, 1.0f);
	uvs[0].Set(1.0f, 0.0f);
	uvs[0].Set(1.0f, 1.0f);

	// Indices
	unsigned short indices[3];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// Diffuse Colors
	l_DiffuseColors[0].Set( 1, 0, 0, 1 );
	l_DiffuseColors[1].Set( 0, 1, 0, 1 );
	l_DiffuseColors[2].Set( 0, 0, 1, 1 );

	// Specular Colors
	maFloatRGBA specular_colors[4];
	specular_colors[0].Set( 0, 0, 0, 1 );
	specular_colors[1].Set( 0, 0, 0, 1 );
	specular_colors[2].Set( 0, 0, 0, 1 );

	// Set up material
	m_Material.SetAmbient( 0, 0, 0, 1 );
	m_Material.SetDiffuse( 0, 0, 0, 1 );
	m_Material.SetEmissive( 0, 0, 0, 1 );
	m_Material.SetHasSpecular( false );
	matMaterial* pMaterial = &m_Material;

	// Create the fragment (triMesh doesn't have diffuse and specular coloring)
	//m_Fragment = new triMeshPreLitFrag( vertices, l_DiffuseColors,
	//			uvs, normals, specular_colors, 3,
	//			indices, 3, pMaterial, true );
	m_Fragment = new triMeshPreLitFrag( vertices, l_DiffuseColors,
				uvs, 3, indices, 3, pMaterial, true );

	// Create a scene node for it
	g3dSceneNode *model = new g3dSceneNode();
	model->SetFragment(m_Fragment);
	m_Root->AddChild(model);

	// Set camera
	SetRadius( 3 );
	SetPitch( 90.0f * maConstants::c_fAngleToRad );
	SetYaw( 0.0f * maConstants::c_fAngleToRad );

	// Set lights
	m_pLight = g3dLightManager::CreateDirectionalLight();
	m_pLight->SetDirection( maVector3d( 0, 0, -1.0f ) );
	m_pLight->SetIntensity( maFloatRGBA( 1, 1, 1, 1 ) );
	m_pLight->Enable();

	g3dLightManager::SetAmbient( maFloatRGBA( 0.25f, 0.25f, 0.25f, 0.25f ));

	// Set up color anims
	m_pColorAnim1 = new anKeyAnimation<maFloatRGBA>( maFloatRGBA( 1, 0, 0, 1) );
	m_pColorAnim1->AddKey( 3.0f, maFloatRGBA( 0, 1, 0, 1 ));
	m_pColorAnim1->AddKey( 6.0f, maFloatRGBA( 0, 0, 1, 1 ));
	m_pColorAnim1->AddKey( 9.0f, maFloatRGBA( 1, 0, 0, 1 ));
	m_pColorAnim1->SetLooping( true );
	m_pColorAnim1->SetReversing( false );

	m_pColorAnim2 = new anKeyAnimation<maFloatRGBA>( maFloatRGBA( 0, 1, 0, 1) );
	m_pColorAnim2->AddKey( 3.0f, maFloatRGBA( 0, 0, 1, 1 ));
	m_pColorAnim2->AddKey( 6.0f, maFloatRGBA( 1, 0, 0, 1 ));
	m_pColorAnim2->AddKey( 9.0f, maFloatRGBA( 0, 1, 0, 1 ));
	m_pColorAnim2->SetLooping( true );
	m_pColorAnim2->SetReversing( false );

	m_pColorAnim3 = new anKeyAnimation<maFloatRGBA>( maFloatRGBA( 0, 0, 1, 1) );
	m_pColorAnim3->AddKey( 3.0f, maFloatRGBA( 1, 0, 0, 1 ));
	m_pColorAnim3->AddKey( 6.0f, maFloatRGBA( 0, 1, 0, 1 ));
	m_pColorAnim3->AddKey( 9.0f, maFloatRGBA( 0, 0, 1, 1 ));
	m_pColorAnim3->SetLooping( true );
	m_pColorAnim3->SetReversing( false );
}

//====================================================================
//	Think
//====================================================================
void demG3dTestVertexColoring::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	l_DiffuseColors[0] = m_pColorAnim1->GetValue( frame_time );
	l_DiffuseColors[1] = m_pColorAnim2->GetValue( frame_time );
	l_DiffuseColors[2] = m_pColorAnim3->GetValue( frame_time );

	// Can't modify colors in scene graph format
//	g3dFragmentManager::ModifyColors( m_Fragment,
//									  l_DiffuseColors,
//									  l_ColorIndices,
//									  3,
//									  NULL,
//									  NULL,
//									  0 );


	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestVertexColoring::DeInitialize()
{
	// destroy anims
	delete m_pColorAnim1;
	delete m_pColorAnim2;
	delete m_pColorAnim3;

	//	cleanup lights
	g3dLightManager::DestroyLight(m_pLight);

	//	cleanup models
	delete m_Fragment;

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
