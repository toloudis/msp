/*****************************************************************************
**  tstModeBumpMaterialTest.cpp
**
**  See tstModeBumpMaterialTest.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "tstModeBumpMaterialTest.hpp"
#include "tstLightMgr.hpp"

#include "camCameraMgr.hpp"
#include "g3dTriMeshBumpFrag.hpp"

#include "appCharEvent.hpp"
#include "appSimTime.hpp"
#include "entImport.hpp"
#include "entModelTemplate.hpp"
#include "g2dScreen.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dRendererMgr.hpp"
#include "g3dScene.hpp"
#include "g3dSingleLightRendering.hpp"
#include "geoOBBox.hpp"
#include "geoOBCylinder.hpp"
#include "inDeviceMgr.hpp"
#include "maConstants.hpp"
#include "maFunctions.hpp"
#include "matPlainTexture.hpp"
#include "matTextureManager.hpp"
#include "scCamera.hpp"
#include "scObject.hpp"

namespace
{

	int l_MouseX = 0;
	int l_MouseY = 0;

	bool l_Current = false;
	bool l_GoldBase = false;

	const float lc_fRadius = 5.0f;
	const float lc_fHeight = 10.0f;

//====================================================================
//	CreateTexturedSphere makes a sphere-like object which has texture
//	coordinates.
//====================================================================
g3dFragment*  create_textured_sphere(	float i_Radius, 
										int i_LatDiv,
										int i_LongDiv,
										matMaterial *m_Mat)
{
	DBG_ASSERT0(i_Radius > 0, "Sphere radius must be greater than 0");
	DBG_ASSERT0(i_LatDiv > 1, "Sphere must have at least two lateral divisions");
	DBG_ASSERT0(i_LongDiv > 2, "Sphere must have at least three longitudinal divisions");

	typedef std::vector<maPoint3d> Row;

	//	make the latitude rows
	//
	const float lat_angle_delta = maConstants::c_fPI / float(i_LatDiv);
	const float long_angle_delta = maConstants::c_fPI_Times_2 / float(i_LongDiv);
	const int num_rows = i_LatDiv - 1;
	const int num_columns = i_LongDiv;
	int row_num;

	std::vector<maPoint3d> row_list;
	std::vector<maPoint3d> normals;
	std::vector<maPoint2d> texture_coords;
	int vertex_row_base = 0;

	for( row_num = 0 ; row_num < num_rows ; row_num++ )
	{
		float theta = lat_angle_delta * float(row_num + 1);
		float sin_theta = sin(theta);
		float cos_theta = cos(theta);

		int column_num;

		for( column_num = 0 ; column_num < num_columns ; column_num++ )
		{
			float phi = long_angle_delta * float(column_num);
			float sin_phi = sin(phi);
			float cos_phi = cos(phi);
			float x = cos_phi * sin_theta;
			float z = sin_phi * sin_theta;
			float y = cos_theta;
			maPoint3d cur(x, y, z);
			row_list.push_back(cur * i_Radius);
			normals.push_back(cur);
			texture_coords.push_back(maPoint2d(theta / maConstants::c_fPI, phi / maConstants::c_fPI_Times_2));
		}
	}

	//	The top point's vertex number is (num_rows * num_columns)
	//	The bottom point's vertex number is (num_rows * num_columns) + 1
	int top_index = num_rows * num_columns;
	int bot_index = top_index + 1;
	DBG_ASSERT0(top_index == row_list.size(), "Miscount");
	row_list.push_back(maPoint3d(0, i_Radius, 0));
	row_list.push_back(maPoint3d(0, -i_Radius, 0));
	normals.push_back(maPoint3d(0, 1.0f, 0));
	normals.push_back(maPoint3d(0, -1.0f, 0));
	texture_coords.push_back(maPoint2d(0, 0));
	texture_coords.push_back(maPoint2d(1, 0));

	std::vector<unsigned short> indices;

	int num_strips = num_rows - 1;
	
	// make indices for the center strips (if necessary)	
	for( row_num = 0 ; row_num < num_strips ; ++row_num )
	{
		int column_num;
		
		int row_index_base = row_num * i_LongDiv * 2 * 3;
		int cur_index_base = row_index_base;
		for( column_num = 0 ; column_num < num_columns ; column_num++ )
		{
			int next = (column_num + 1) % num_columns;
			int v0 = row_num * num_columns + column_num;
			int v1 = (row_num+1) * num_columns + column_num;
			int v2 = (row_num+1) * num_columns + next;
			int v3 = row_num * num_columns + next;
			indices.push_back(v0);
			indices.push_back(v3);
			indices.push_back(v2);
			indices.push_back(v2);
			indices.push_back(v1);
			indices.push_back(v0);
		}
	}

	// make the top cap
	int column_num;
	
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		indices.push_back( top_index );
		indices.push_back( (column_num + 1) % num_columns);
		indices.push_back( column_num );
	}

	// make the bottom cap
	int bot_row_base = (num_rows-1) * num_columns;
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		indices.push_back( bot_index );
		indices.push_back( bot_row_base + column_num );
		indices.push_back( bot_row_base + ((column_num+1) % num_columns) );
	}

	g3dFragment* ret_val = new g3dTriMeshBumpFrag(	&(row_list[0]),
													&(normals[0]),
													&(texture_coords[0]),
													row_list.size(),
													&(indices[0]),
													indices.size(),
													m_Mat );
	return ret_val;
}

}


//====================================================================
// Construction -- Inititialize the LandingSequenceScriptMgr
//====================================================================
tstModeBumpMaterialTest::tstModeBumpMaterialTest()
:	tstMode( 4, false )
{
}

//====================================================================
// Destruction -- cleanup LandingSequenceScriptMgr
//====================================================================
tstModeBumpMaterialTest::~tstModeBumpMaterialTest()
{
}

//====================================================================
//	The mode should do it's per frame "work" in the Think function.
//====================================================================
void tstModeBumpMaterialTest::Think()
{
	// base class
	tstMode::Think();

	if (e_Continue != GetTerminateCondition() )
	{
		return; // we're terminated
	}

	float frame_time = appSimTime::GetTime();
	float theta = frame_time * maConstants::c_fAngleToRad * 30.0f;
	float phi = frame_time * maConstants::c_fAngleToRad * 60.0f;
	maRotation rot(maVector3d(0,1,0), -phi);
	m_ExtrObj->SetOrientation(rot);

	float x, y, z;
	x = 15.0f * sin(theta);
	y = 10.0f + 4.0f * sin(phi);
	z = 15.0f * cos(theta);
	maPoint3d light1_pos(x, y, z);
	m_RedPoint->SetPosition(light1_pos);

	// output
	SetDebugString(0, itString("Bump mapping with specular map"));
	char buffer[128];
	::sprintf(buffer, "Bump Scale : %f (Use b and B to change)", m_Material.GetBumpMapScale());
	SetDebugString(1, itString(buffer));
	SetDebugString(2, itString("1,2,3 toggle lights"));
	SetDebugString(3, itString("s - specular map, d - diffuse bump"));

	// render
	this->Render();
}

//====================================================================
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of tstModeAttract should remember to call
//	tstModeAttract::Initialize() at the beginning of their Initialize
//	function.
//====================================================================
void tstModeBumpMaterialTest::Initialize()
{
	tstMode::Initialize();

	g3dSingleLightRendering::SetDoSingleLightRendering(true);

	// Set up material
	fsLocator tex_loc;
	tex_loc.Push("data");
	//tex_loc.Push("orientvn.nmp");
	tex_loc.Push("bumpmap.png");
	m_BumpTexture = matTextureManager::LoadTexture(tex_loc);
	tex_loc.Pop();
	tex_loc.Push("specmap.bmp");
	m_Texture = matTextureManager::LoadTexture(tex_loc);
	tex_loc.Pop();
	tex_loc.Push("base.bmp");
	m_GoldTexture = matTextureManager::LoadTexture(tex_loc);
	//tex_loc.Pop();
	//tex_loc.Push("gradient.png");
	//m_GradientTexture = matTextureManager::LoadTexture(tex_loc);

	//m_Material.AddTextureTop( m_Texture );
	m_Material.AddTextureTop( m_BumpTexture );	// normal map
	m_Material.AddTextureTop( m_GoldTexture );	// base texture
	m_Material.AddTextureTop( m_Texture );		// specular map
	//m_Material.AddTextureTop( m_GradientTexture );	// specular gradient
	//m_Material.AddTextureTop( m_GradientTexture );	// specular gradient
	m_Material.SetHasSpecular(true);
	m_Material.SetSpecularPower(20);
	//m_Material.SetSpecialEffect(matShaderIndex::e_BumpDiffuseSpecMap);
	m_Material.SetSpecialEffect(matShaderIndex::e_BumpDiffuse);

	m_ExtrFrag = create_textured_sphere(lc_fRadius, 32, 32, &m_Material);
	m_ExtrFrag->SetCastsShadow(true);

	m_ExtrObj = new scObject(m_ExtrFrag);
	m_ExtrObj->SetPosition(maPoint3d(0,7,0));
	g3dScene::AddToWorldRoot(m_ExtrObj->GetBase());

	// Base rectangle
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(50.0f, 50.0f, 40, 40);
	m_RectMat.SetDiffuse(maFloatRGBA(0.6f, 1.0f, 0.6f, 1.0f));
	m_RectMat.SetSpecular(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_RectMat.SetHasSpecular(true);
	m_RectMat.SetSpecularPower(700);
	m_RectFragment->SetMaterial(&m_RectMat);
	maRotation rot(maVector3d(1,0,0), -maConstants::c_fPI_Div_2 );
	m_BaseRect = new scObject(m_RectFragment);
	g3dScene::AddToWorldRoot(m_BaseRect->GetBase());
	m_BaseRect->SetOrientation(rot);

	//	set lights
	m_Light1 = tstLightMgr::CreateDirectionalLight();
	m_Light2 = tstLightMgr::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.9f, 0.9f, 0.9f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.1f, 0.4f, 0.6f, 1.0f));

	m_Light1->SetCastsShadow(true);
	m_Light2->SetCastsShadow(true);

	m_RedPoint = tstLightMgr::CreatePointLight();
	m_RedPoint->SetIntensity(maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	m_RedPoint->SetCastsShadow(true);

	m_Light2->Disable();
	m_RedPoint->Disable();

/*	// position the camera back to see objects
	camCameraMgr::GetCamera().LookAt( maVector3d(0.0f, 13.1f, 5.0f),
									maPoint3d(0.0f, 1.1f, 0.0f),
									maVector3d(0.0f, 1.0f, 0.0f ) );
	// reset camera
	camCameraMgr::SetCurrentManip( camCameraMgr::e_Orbit );
*/
	l_Current = true;
}

//====================================================================
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//====================================================================
void tstModeBumpMaterialTest::DeInitialize()
{
	// Do Cleanup here
	l_Current = false;

	//	cleanup objects
	delete m_ExtrObj;

	//	cleanup fragments
	delete m_ExtrFrag;

	//	cleanup lights
	tstLightMgr::DestroyLight(m_Light1);
	tstLightMgr::DestroyLight(m_Light2);

	matTextureManager::ReleaseTexture(m_BumpTexture);
	matTextureManager::ReleaseTexture(m_Texture);
	matTextureManager::ReleaseTexture(m_GoldTexture);
	//matTextureManager::ReleaseTexture(m_GradientTexture);

	//	let the base deinitialize
	tstMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void tstModeBumpMaterialTest::ReceiveCharEvent(appCharEvent& i_Event)
{
	if( !l_Current ) return;

	switch (i_Event.GetChar())
	{
	case itString::CharType('b'):
		m_Material.SetBumpMapScale( m_Material.GetBumpMapScale() * 0.75f);
		break;
	case itString::CharType('B'):
		m_Material.SetBumpMapScale( m_Material.GetBumpMapScale() * 1.25f);
		break;
	case itString::CharType('g'):
		l_GoldBase = !l_GoldBase;
		m_Material.ReplaceTexture(1, l_GoldBase ? m_GoldTexture : m_Texture);
		break;
	case itString::CharType('1'):
		if (m_Light1->IsEnabled())
			m_Light1->Disable();
		else
			m_Light1->Enable();
		break;
	case itString::CharType('2'):
		if (m_Light2->IsEnabled())
			m_Light2->Disable();
		else
			m_Light2->Enable();
		break;
	case itString::CharType('3'):
		if (m_RedPoint->IsEnabled())
			m_RedPoint->Disable();
		else
			m_RedPoint->Enable();
		break;
	case itString::CharType('s'):
		m_Material.SetSpecialEffect(matShaderIndex::e_BumpSpecularMap);
		break;
	case itString::CharType('d'):
		m_Material.SetSpecialEffect(matShaderIndex::e_BumpDiffuse);
		break;
	}
}