/*****************************************************************************
**  demG3dTestAmbientOcclusion.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestAmbientOcclusion.hpp"


#include "appTime.hpp"
#include "dbgLog.hpp"
#include "demModeManager.hpp"
#include "effShaderUtilWin.hpp"
#include "envSTLHelpers.hpp"
#include "fsLocator.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dLayer.hpp"
#include "g3dPointLight.hpp"
#include "g3dScene.hpp"
#include "geoKDTree.hpp"
#include "geoPickedPoint.hpp"
#include "maFunctions.hpp"
#include "matTextureMgr.hpp"
#include "maConstants.hpp"
#include "mayFragInfoSinkKDTree.hpp"
#include "mayImport.hpp"
#include "gfPaths.hpp"
#include "tmeshFrag.hpp"


namespace
{
const int c_NumShaderMats = 1;
const float c_TimePerShader = 2.0f;
bool l_bRotateShader = true;


//====================================================================
//====================================================================
unsigned int convert_color(	float i_Occlusion )
{
	unsigned int alpha = 255;
	float fcolor = i_Occlusion * 255;
	maFunctions::Clamp(fcolor, 0.0f, 255.0f);

	unsigned int color =      ( alpha << 24 ) 
							| ( (unsigned int) fcolor << 16 ) 
							| ( (unsigned int) fcolor << 8 )
							| ( (unsigned int) fcolor );
				
	return color;
}

// Compute set of rays to use for all vertices
void get_rays(std::vector<maPoint3d> &o_Rays)
{
	const int c_NumRays = 255; //32;
	o_Rays.resize(c_NumRays);
	for (int r=0; r<c_NumRays; r++)
	{
		maVector3d vec(maFunctions::FloatRand(-1, 1),
						maFunctions::FloatRand(-1, 1),
						maFunctions::FloatRand(-1, 1));
		vec.Normalize();
		o_Rays[r] = vec;
	}

}

// Computes ambient occlusion term using KDTree for occlusion
void compute_ambient_shading(g3dFragment *i_pFragment, geoKDTree &i_Tree, 
							 const std::vector<maPoint3d> &i_Rays)
{
	// convert to colored, bump mapped fragment type
	tmeshFrag *mesh_frag = dynamic_cast<tmeshFrag*>(i_pFragment);
	DBG_ASSERT0( mesh_frag->GetVertexStride() == sizeof( g3dType::BumpLitTex1Vertex ), "Only g3dType::BumpLitTex1Vertex format implemented" );

	g3dType::BumpLitTex1Vertex* bump_vertices = reinterpret_cast<g3dType::BumpLitTex1Vertex*>( mesh_frag->Lock() );
	int num_frag_verts = mesh_frag->GetNumVertices();

	int perc = num_frag_verts / 10;
	int per_cnt = 0;

	for (int j=0; j<num_frag_verts; j++)
	{
		if (j > per_cnt * perc)
		{
			per_cnt++;
			DBG_WARNING1("%d", per_cnt*10);
		}	

		// Simple shader to test rendering
		//float occl = bump_vertices[j].m_Normal.m_Y * 0.5f + 0.5f;
		//bump_vertices[j].m_Color = convert_color(occl);

		// Cast multiple rays to average occlusion term
		int num_rays = i_Rays.size();
		int count = 0;
		for (int r=0; r<num_rays; r++)
		{
			maVector3d vec = i_Rays[r];

			// We only want rays that are in the hemisphere pointing
			// away from the normal
			if (bump_vertices[j].m_Normal * vec < 0)
				vec *= -1.0f;

			// Construct a ray to test occlusion
			geoPickedPoint pick_pt;
			const float c_Offset = 0.01f; // slightly offset ray start along normal
			maPoint3d ray_start = bump_vertices[j].m_Vertex + bump_vertices[j].m_Normal * c_Offset;
			const float c_Length = 9999.9f; // long ray, will get clipped by kdTree's bbox
			maPoint3d ray_end = bump_vertices[j].m_Vertex + vec * c_Length;
			if (!i_Tree.ComputeRayIntersection(	ray_start, ray_end, pick_pt))
				count++;
		}

		// convert occlusion term to number between 0-1
		float occl = count / float(num_rays);
		bump_vertices[j].m_Color = convert_color(occl);
	}

	mesh_frag->Unlock();
}

}

//====================================================================
//====================================================================
demG3dTestAmbientOcclusion::demG3dTestAmbientOcclusion(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_pEffect(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestAmbientOcclusion::~demG3dTestAmbientOcclusion()
{
}

//====================================================================
//====================================================================
void demG3dTestAmbientOcclusion::Initialize()
{
	// effect load test
	fsLocator eff_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	eff_loc.Push("data");
	eff_loc.Push("Ambient.fx");
	//eff_loc.Push("Single-tex0.fx");
	//eff_loc.Push("velvety.fx");
	m_pEffect = effShaderUtilWin::CompileEffect(eff_loc);
	m_ShaderMat.SetShaderEffect( m_pEffect ); 

	demG3dTestMode::Initialize();

	// zoom in with camera by default
	this->SetRadius(5.0f);

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Ambient Occlusion."));
	m_Viewer.SetTextMessage(1, itString("Each vertex is colored to represent the amount of ambient light occluded "));
	m_Viewer.SetTextMessage(2, itString("from self-shadowing."));
	m_Viewer.SetTextMessage(3, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(4, itString("Press space to go to the next section"));

	// Make kdTree sink to grab triangles
	//geoKDTree kdTree;
	//mayFragInfoSinkKDTree tree_sink(kdTree);

	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push("prelit-torus.mx");
	//locator.Push("bigship.mx");
	std::vector<g3dFragment*> shipFragments, shipLowResFragments, shipHighResFragments;
	mayImport::LoadWorldFragments(locator, fsResourceFinderDir(fsLocator()),
							shipFragments,
							shipLowResFragments,
							shipHighResFragments,
							m_Materials,
							m_Textures);
	m_ShipFragment = shipHighResFragments[0];

	// Set up kdTree based on size of fragment
	//maAxisBox box = m_ShipFragment->GetBoundingBox();
	//float diag = box.GetRadius() / 16.0f;
	//kdTree.SetMinBox(diag, diag, diag);
	//kdTree.Create();

	// Light ambient colors
	//std::vector<maPoint3d> rays;
	//get_rays(rays);
	//compute_ambient_shading(m_ShipFragment, kdTree, rays);

	//	make models
	m_Ship = new g3dSceneNode(m_ShipFragment);
	m_Root->AddChild(m_Ship);
	m_Ship->SetRenderable(true);
	m_ShipFragment->SetMaterial( &m_ShaderMat );

	//	set lights
	//m_Light1 = g3dLightMgr::CreateDirectionalLight();
	//m_Light2 = g3dLightMgr::CreateDirectionalLight();
	m_Light3 = g3dLightMgr::CreatePointLight();
	//m_Light4 = g3dLightMgr::CreatePointLight();

	//m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	//m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));
	m_Light3->SetPosition(maVector3d(0, -5, 0));
	//m_Light4->SetPosition(maVector3d(0, 5, 0));

	//m_Light1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	//m_Light2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));
	m_Light3->SetIntensity(maFloatRGBA(0.9f, 0.1f, 0.1f, 1.0f));
	//m_Light4->SetIntensity(maFloatRGBA(0.1f, 0.9f, 0.1f, 1.0f));

	//m_Light1->Enable();
	//m_Light2->Enable();
	m_Light3->Enable();
	//m_Light4->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestAmbientOcclusion::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();


	//	set the orientations of our lights
	float theta = frame_time * maConstants::c_fAngleToRad * 30.0f;
	float phi = frame_time * maConstants::c_fAngleToRad * 60.0f;

	float x, y, z;

	x = 15.0f * sin(theta);
	y = 10.0f + 4.0f * sin(phi);
	z = 15.0f * cos(theta);
	maPoint3d light1_pos(x, y, z);

	//x = 15.0f * sin(theta + maConstants::c_fPI);
	//y = 8.0f + 4.0f * sin(phi + maConstants::c_fPI);
	//z = 15.0f * cos(theta + maConstants::c_fPI);
	//maPoint3d light2_pos(x, y, z);

	m_Light3->SetPosition(light1_pos);
	//m_Light4->SetPosition(light2_pos);

	//	set the orientations of our objects
	//maMatrix4x4& matrix1 = m_Ship->GetTransform();
	////matrix1.MakeScale(0.5f,0.5f,0.5f);
	//matrix1.MakeRotate( -frame_time * 30.0f * maConstants::c_fAngleToRad,
	//						maVector3d(0, 1, 0));


	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestAmbientOcclusion::DeInitialize()
{
	delete m_pEffect;

	//	cleanup lights
	//g3dLightMgr::DestroyLight(m_Light1);
	//g3dLightMgr::DestroyLight(m_Light2);
	g3dLightMgr::DestroyLight(m_Light3);
	//g3dLightMgr::DestroyLight(m_Light4);

	//	cleanup models
	//g3dScene::RemoveModel(m_Ship);

	//	cleanup fragments
	delete m_ShipFragment;

	// delete materials
	envSTLHelpers::DeleteContainer(m_Materials);

	//	release textures
	envSTLHelpers::ForAll(m_Textures, matTextureMgr::ReleaseTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
