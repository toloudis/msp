/*****************************************************************************
**	cmraIconObject.cpp
**
**	3D Object that holds an icon for the camera representing its view
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraIconObject.hpp"

#include "MainApp/mnmApp.hpp"


#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#undef GetObject // for api3dObjectSimple to compile(?!)
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScale.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"


//============================================================================
//============================================================================
namespace
{
	const float c_PickSphereRadius = 0.4f;	
	const float c_TargetTransparency = 0.3f;

	const int c_NumIconVerts = 5+8; // 5 for pyramid, 8 for box
	const int c_NumIconInds = 16+24; // 16 for pyramid, 24 for box

	const int c_NumDOFIconVerts = 4; // 4 verts per quad
}


//============================================================================
//============================================================================
cmraIconObject::cmraIconObject()
:	m_FieldOfView(90),
	m_AspectRatio(4.0f / 3.0f),
	m_NearBlurDist(1),
	m_NearFocalDist(1),
	m_FarFocalDist(1),
	m_FarBlurDist(1)
{
	// Material
	matMaterial *mat = new matMaterial("Phong.fx");
	effPhongData* pData = dynamic_cast<effPhongData*>(mat->GetEffectData());
	DBG_ASSERT0(pData != NULL, "cmraIconObject not using effPhong");
	pData->m_ColorEmissive = maFloatRGBA(1.0f, 1.0f, 0.0f, 1.0f);

	std::vector<maPoint3d> line_verts(c_NumIconVerts, maPoint3d(0,0,0));
	std::vector<maPoint3d> line_normals(c_NumIconVerts, maPoint3d(0,1,0));
	std::vector<unsigned short> indices(c_NumIconInds);
	// create pyramic shape with vert 0 as camera position and the
	// other four vertices as the plane of the camera
	indices[0] = 0; indices[1] = 1;
	indices[2] = 0; indices[3] = 2;
	indices[4] = 0; indices[5] = 3;
	indices[6] = 0; indices[7] = 4;
	indices[8] = 1; indices[9] = 2;
	indices[10] = 2; indices[11] = 3;
	indices[12] = 3; indices[13] = 4;
	indices[14] = 4; indices[15] = 1;
	// Then add eight more vertices to create a box shape for the camera
	//	behind the view icon. Starts at vertex index 5.
	indices[16] = 9; indices[17] = 10;
	indices[18] = 10; indices[19] = 11;
	indices[20] = 11; indices[21] = 12;
	indices[22] = 12; indices[23] = 9;
	indices[24] = 5; indices[25] = 9;
	indices[26] = 6; indices[27] = 10;
	indices[28] = 7; indices[29] = 11;
	indices[30] = 8; indices[31] = 12;
	indices[32] = 5; indices[33] = 6;
	indices[34] = 6; indices[35] = 7;
	indices[36] = 7; indices[37] = 8;
	indices[38] = 8; indices[39] = 5;
	
	// Create fragment
	m_pFragment = g3dFragmentCreate::CreateLineList(&(line_verts[0]), &(line_normals[0]),
		c_NumIconVerts, &(indices[0]), c_NumIconInds, mat, true);

	// Set up fragment in object
	m_pObject = new api3dObjectSimple(m_pFragment, mat);
	m_pObject->SetUniformScale( api3dScale::GetGlobalScale() );
	m_pObject->SetGPUPickable(false);
	api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	// create 4 distinct DOF planes, different colors. 
	// if these were not different colors, then we could do this with the main fragment.
	std::vector<maPoint3d> dof_line_verts(c_NumDOFIconVerts, maPoint3d(0,0,0));
	for (int i = 0; i < 4; i++)
	{
		m_pDOFObject[i] = api3dShape::CreateLineList(maFloatRGBA(0.25f + 0.75f*(float)(3-i)/3.0f,
			0.25f + 0.75f*(float)(3-i)/3.0f, 0.0f, 1.0f),
			&(dof_line_verts[0]), c_NumDOFIconVerts, true, true);
		// No need to use global scale here. The coordinates of these objects
		// will use true distances.
		//m_pDOFObject[i]->SetUniformScale( api3dScale::GetGlobalScale() );
		m_pDOFObject[i]->SetGPUPickable(false);
		api3dScene::AddObject(m_pDOFObject[i], mnmApp::GetIconsLayerIndex());
	}

	// Pickable spheres at position and target location
	m_pPickPosition = api3dShape::CreateSphere(maFloatRGBA(), 
		c_PickSphereRadius, 8, 8);
	m_pPickPosition->SetUniformScale( api3dScale::GetGlobalScale() );
	api3dScene::AddObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());
	m_pPickPosition->SetRenderable(false);
	m_pPickPosition->SetPickHull(true);
	m_pPickPosition->SetGPUPickable(true);
	m_pPickTarget = api3dShape::CreateSphere(maFloatRGBA(1.0f, 1.0f, 0.7f, c_TargetTransparency), 
		c_PickSphereRadius, 2, 4);
	m_pPickTarget->SetUniformScale( api3dScale::GetGlobalScale() );
	api3dScene::AddObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());
	//m_pPickTarget->SetRenderable(false);
	//m_pPickTarget->SetPickHull(true);
	m_pPickTarget->SetGPUPickable(true);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraIconObject::~cmraIconObject()
{
	for (int i = 0; i < 4; i++)
	{
		api3dScene::RemoveObject(m_pDOFObject[i], mnmApp::GetIconsLayerIndex());
		delete m_pDOFObject[i];
	}
	api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;

	api3dScene::RemoveObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());
	delete m_pPickPosition;
	api3dScene::RemoveObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());
	delete m_pPickTarget;

	// fragment owned by template, we don't have to delete it
}

//--------------------------------------------------------------------
// Update vertices of morphable fragment match view
//--------------------------------------------------------------------
void  cmraIconObject::Update(const maPoint3d &i_Pos, 
							 const maPoint3d &i_Target,
							 float i_Tilt,
							 float i_FieldOfView,
							 float i_AspectRatio,
							 float i_NearBlurDist, float i_NearFocalDist, 
							 float i_FarFocalDist, float i_FarBlurDist)
{
	if (   (m_Position == i_Pos)
		&& (m_Target == i_Target) 
		&& (m_Tilt == i_Tilt) 
		&& (m_FieldOfView == i_FieldOfView) 
		&& (m_AspectRatio == i_AspectRatio) 
		&& (m_NearBlurDist == i_NearBlurDist) 
		&& (m_NearFocalDist == i_NearFocalDist) 
		&& (m_FarFocalDist == i_FarFocalDist) 
		&& (m_FarBlurDist == i_FarBlurDist) 
		)
	{
		// no need to update the icon object
		return;
	}

	DBG_ASSERT0( m_pFragment->GetVertexFormat() == g3dType::e_BumpTex1Vertex, "Only g3dType::BumpTex1Vertex format implemented" );
	DBG_ASSERT0( c_NumIconVerts == m_pFragment->GetNumVertices(), "Number of icon vertices is incorrect" );

	m_Position	= i_Pos;
	m_Target	= i_Target;
	m_Tilt		= i_Tilt;
	m_FieldOfView	= i_FieldOfView;
	m_AspectRatio	= i_AspectRatio;
	m_NearBlurDist	= i_NearBlurDist;
	m_NearFocalDist	= i_NearFocalDist;
	m_FarFocalDist	= i_FarFocalDist;
	m_FarBlurDist	= i_FarBlurDist;

	m_pObject->SetPosition(i_Pos);

	// NOTE: This needs to be a AlterVertices() function in base fragment class

	//DBG_LOG6("UpdIcon pos: %6.3f %6.3f %6.3f   tgt: %6.3f %6.3f %6.3f", i_Pos.m_X, i_Pos.m_Y, i_Pos.m_Z, i_Target.m_X, i_Target.m_Y, i_Target.m_Z );

	// Compute tilted up vector
	maVector3d cam_up(0,1,0);
	if (i_Tilt != 0.0f)
	{
		maRotation rot(i_Target - i_Pos, maConstants::c_fAngleToRad * i_Tilt);
		rot.RotateVector(cam_up);
	}

	// Construct view matrix
	maMatrix4x4 cam_matx;
	cam_matx.LookAt(i_Pos, i_Target, cam_up);
//	cam_matx.Invert();
//	maVector3d left	= cam_matx * maVector3d(1,0,0);
//	maVector3d up	= cam_matx * maVector3d(0,1,0);
//	maVector3d view	= cam_matx * maVector3d(0,0,1);
	maVector3d left(cam_matx.m_Mat[0], cam_matx.m_Mat[1], cam_matx.m_Mat[2]);
	maVector3d up  (cam_matx.m_Mat[4], cam_matx.m_Mat[5], cam_matx.m_Mat[6]);
	maVector3d view(-cam_matx.m_Mat[8], -cam_matx.m_Mat[9], -cam_matx.m_Mat[10]);

	// scale down the vectors to make a smaller icon
	const float c_IconScale = 0.5f;
	left	*= c_IconScale;
	up		*= c_IconScale;
	view	*= c_IconScale;

	// Incorporate field of view into icon
	float tan_fov = tanf((i_FieldOfView / 2.0f) * maConstants::c_fAngleToRad);
	if (i_FieldOfView > 90)
	{
		// In large angles, shorten view vector
		view /= tan_fov;
	}
	else if (i_FieldOfView < 90)
	{
		// In small angles, shorten left and up vectors
		left *= tan_fov;
		up *= tan_fov;
	}

	// Lock the vertex buffer
	g3dType::BumpTex1Vertex* tex0_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>( m_pFragment->Lock() );

	// Only put offset into vertex (position comes from transformation)

	// Vertex 0 stays (0,0,0)

	// Position of four corners of view plane
	tex0_vertices[1].m_Vertex =  view + left * i_AspectRatio + up;
	tex0_vertices[2].m_Vertex =  view - left * i_AspectRatio + up;
	tex0_vertices[3].m_Vertex =  view - left * i_AspectRatio - up;
	tex0_vertices[4].m_Vertex =  view + left * i_AspectRatio - up;
	// Position of eight corners of camera box
	tex0_vertices[5].m_Vertex =  left + up;
	tex0_vertices[6].m_Vertex =  -1 * left + up;
	tex0_vertices[7].m_Vertex =  -1 * left - up;
	tex0_vertices[8].m_Vertex =  left - up;
	tex0_vertices[9].m_Vertex =  - 2 * view + left + up;
	tex0_vertices[10].m_Vertex =  - 2 * view - left + up;
	tex0_vertices[11].m_Vertex =  - 2 * view - left - up;
	tex0_vertices[12].m_Vertex =  - 2 * view + left - up;



	maAxisBox bbox;
	for (int i=0; i<c_NumIconVerts; i++)
	{
		bbox.Union(tex0_vertices[i].m_Vertex);
	}

	m_pFragment->Unlock();

	m_pFragment->SetBoundingBox(bbox);

	// reposition DOF planes:

	g3dFragment* frag = NULL;

	float distances[4] = {i_NearBlurDist, i_NearFocalDist, i_FarFocalDist, i_FarBlurDist};
	for (int i = 0; i < 4; i++)
	{
		m_pDOFObject[i]->SetPosition(i_Pos);

		// Lock the vertex buffer
		frag = (m_pDOFObject[i]->Fragment());
		tex0_vertices = reinterpret_cast<g3dType::BumpTex1Vertex*>( frag->Lock() );
		// Position of four corners of DOF near blur plane.
		// The (1.0f/c_IconScale) factor compensates for the view, left, and up vectors 
		// being premultiplied above. We need these distances to be true, unscaled.
		tex0_vertices[0].m_Vertex =  (1.0f/c_IconScale) * (view + left * i_AspectRatio + up) * distances[i];
		tex0_vertices[1].m_Vertex =  (1.0f/c_IconScale) * (view - left * i_AspectRatio + up) * distances[i];
		tex0_vertices[2].m_Vertex =  (1.0f/c_IconScale) * (view - left * i_AspectRatio - up) * distances[i];
		tex0_vertices[3].m_Vertex =  (1.0f/c_IconScale) * (view + left * i_AspectRatio - up) * distances[i];
		// compute bounds
		maAxisBox bbox;
		for (int j=0; j<c_NumDOFIconVerts; j++)
		{
			bbox.Union(tex0_vertices[j].m_Vertex);
		}
		frag->Unlock();
		frag->SetBoundingBox(bbox);
	}

	// Position pick is just sphere at camera position
	m_pPickPosition->SetPosition(i_Pos);
	// Target pick is just sphere at target position
	m_pPickTarget->SetPosition(i_Target);
}

//----------------------------------------------------------------------------
//	Set global scale into icon
//----------------------------------------------------------------------------
void cmraIconObject::SetUniformScale(float i_Scale)
{
	m_pObject->SetUniformScale( i_Scale );
	for (int i = 0; i < 4; i++)
		m_pDOFObject[i]->SetUniformScale( i_Scale );
	m_pPickPosition->SetUniformScale( i_Scale );
	m_pPickTarget->SetUniformScale( i_Scale );
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void cmraIconObject::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);
	for (int i = 0; i < 4; i++)
		m_pDOFObject[i]->SetRenderable( i_Renderable );

	// Set pickable state from renderable state for the invisible pick icons
	m_pPickPosition->SetGPUPickable(i_Renderable);

	// Target changed to visible transparent icon
	//m_pPickTarget->SetGPUPickable(i_Renderable);
	m_pPickTarget->SetRenderable(i_Renderable);
}
bool cmraIconObject::GetRenderable() const
{
	return m_pObject->GetRenderable();
}

//----------------------------------------------------------------------------
//	Pickable sets whether the GPU pick icons should be pickable
//----------------------------------------------------------------------------
void cmraIconObject::SetPickable(bool i_bPickable)
{
	m_pPickPosition->SetGPUPickable(i_bPickable);
	m_pPickTarget->SetGPUPickable(i_bPickable);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool cmraIconObject::PositionContainsPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_pPickPosition->GetGPUPickable())
		return false;

	return m_pPickPosition->ContainsPickCode(i_PickCode);
}
bool cmraIconObject::TargetContainsPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_pPickTarget->GetGPUPickable())
		return false;

	return m_pPickTarget->ContainsPickCode(i_PickCode);
}
