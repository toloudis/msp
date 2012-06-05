/*****************************************************************************
**	prjltIconObject.cpp
**
**	3D Object that holds an icon for the projected light
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Systems/PrjLt/Object/prjltIconObject.hpp"

#include "MainApp/mnmApp.hpp"

#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScale.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"


namespace
{
	const float c_PickSphereRadius = 0.4f;	
	const float c_TargetTransparency = 0.3f;

	const float l_CircleRadius = 0.25f;
	const int c_NumCircleVerts = 24;
	const int c_NumCircleInds = 2 * 24;

	const int c_NumFrustrumVerts = 10;
	const int c_NumFrustrumInds = 26;

	const int c_NumIconVerts = c_NumFrustrumVerts + c_NumCircleVerts;
	const int c_NumIconInds = c_NumFrustrumInds + c_NumCircleInds;

	maPoint3d l_LocalVerts[c_NumIconVerts];

	// Project the given vertex on the near plane
	// to the far plane
	inline maPoint3d project_vertex(const maPoint3d &i_Pt, 
									const maPoint3d &i_Origin, 
									float i_Range,
									float i_Scale)
	{
		return i_Origin + (i_Pt - i_Origin) * ((i_Range + i_Scale) / i_Scale);
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconObject::prjltIconObject()
{
	// Material
	matMaterial *mat = new matMaterial("Phong.fx");

	effPhongData* pData = dynamic_cast<effPhongData*>(mat->GetEffectData());
	DBG_ASSERT0(pData != NULL, "prjltIconObject not using effPhong");
	pData->m_ColorEmissive = maFloatRGBA(1.0f, 1.0f, 0.0f, 1.0f);

	// create truncated pyramid shape with vert 0 as light position and the
	// other eight vertices as the near and far planes of the light's frustrum
	std::vector<maPoint3d> line_verts(c_NumIconVerts, maPoint3d(0,0,0));
	std::vector<maPoint3d> line_normals(c_NumIconVerts, maPoint3d(0,1,0));
	std::vector<unsigned short> indices(c_NumIconInds);
	indices[0] = 1; indices[1] = 5;
	indices[2] = 2; indices[3] = 6;
	indices[4] = 3; indices[5] = 7;
	indices[6] = 4; indices[7] = 8;

	indices[8] = 1; indices[9] = 2;
	indices[10] = 2; indices[11] = 3;
	indices[12] = 3; indices[13] = 4; 
	indices[14] = 4; indices[15] = 1;

	indices[16] = 5; indices[17] = 6;
	indices[18] = 6; indices[19] = 7;
	indices[20] = 7; indices[21] = 8; 
	indices[22] = 8; indices[23] = 5;

	// Then, add long line from position to target
	indices[24] = 0; indices[25] = 9;

	// Then create a circle around the projected light's position
	int ind = c_NumFrustrumInds, vind = c_NumFrustrumVerts;
	for (int i=0; i<c_NumCircleVerts; i++)
	{
		indices[ind++] = vind + i;
		indices[ind++] = vind + ((i+1) % c_NumCircleVerts);
	}

	// Create fragment
	const bool morphable = true;
	m_pFragment = g3dFragmentCreate::CreateLineList(&(line_verts[0]), &(line_normals[0]), c_NumIconVerts,
		&(indices[0]), c_NumIconInds, mat, morphable );

	// Set up fragment in object
	m_pObject = new api3dObjectSimple(m_pFragment, mat);
	m_pObject->SetGPUPickable(false);
	api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	// Create a polygon fragment in order to apply texture to the
	// projection plane
	m_pTextureFrag = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(1.0f, 1.0f, 1, 1, morphable);
	m_pTextureFrag->SetDoubleSided(true);

	// Material
	matMaterial *pTexMat = new matMaterial("Phong.fx");
	pData = dynamic_cast<effPhongData*>(pTexMat->GetEffectData());
	DBG_ASSERT0(pData != NULL, "prjltIconObject not using effPhong");
	const float c_Transparency = 0.3f;
	//pSolidData->m_Color.Set( 1.0f, 1.0f, 1.0f, c_Transparency );
	pData->m_ColorEmissive = maFloatRGBA(1.0f, 1.0f, 1.0f, c_Transparency);
	pData->m_ColorDiffuse = maFloatRGBA(0.0f, 0.0f, 0.0f, c_Transparency);
	pData->m_ColorAmbient = maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f);
	pTexMat->ForceTransparency( true );

	// Set up fragment in object
	m_pTextureObject = new api3dObjectSimple(m_pTextureFrag, pTexMat);
	m_pTextureObject->SetGPUPickable(false);
	api3dScene::AddObject(m_pTextureObject, mnmApp::GetIconsLayerIndex());
	m_pTextureObject->SetRenderable( false ); // not visible until we get a texture

	// Pickable circle (flat cone) and sphere at position and target location
	m_pPickPosition = api3dShape::CreateCone(maFloatRGBA(), 
		l_CircleRadius, 0.3f, 8);
	m_pPickPosition->SetRenderable(false);
	m_pPickPosition->SetPickHull(true);
	m_pPickPosition->SetGPUPickable(true);
	api3dScene::AddObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());
	m_pPickTarget = api3dShape::CreateSphere(maFloatRGBA(1.0f, 0.9f, 0.9f, c_TargetTransparency), 
		c_PickSphereRadius, 3, 5);
	m_pPickTarget->SetUniformScale( api3dScale::GetGlobalScale() );
	//m_pPickTarget->SetRenderable(false);
	//m_pPickTarget->SetPickHull(true);
	m_pPickTarget->SetGPUPickable(true);
	api3dScene::AddObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconObject::~prjltIconObject()
{
	api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;
	api3dScene::RemoveObject(m_pTextureObject, mnmApp::GetIconsLayerIndex());
	delete m_pTextureObject;
	api3dScene::RemoveObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());
	delete m_pPickPosition;
	api3dScene::RemoveObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());
	delete m_pPickTarget;

	// fragments owned by simple objects, we don't have to delete it
}

//--------------------------------------------------------------------
// Update vertices of morphable fragment match view
//--------------------------------------------------------------------
void  prjltIconObject::Update(const maPoint3d &i_Pos, 
							  const maPoint3d &i_Target,
							  float i_Scale,
							  float i_Tilt,
							  float i_Angle,
							  float i_AspectRatio,
							  float i_Range,
							  matTexture* i_pTexture)
{
	//DBG_LOG3("Update pos: %f %f %f", i_Pos.m_X, i_Pos.m_Y, i_Pos.m_Z);
	//DBG_LOG3("Update tgt: %f %f %f", i_Target.m_X, i_Target.m_Y, i_Target.m_Z);

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

	maVector3d left(cam_matx.m_Mat[0], cam_matx.m_Mat[1], cam_matx.m_Mat[2]);
	maVector3d up  (cam_matx.m_Mat[4], cam_matx.m_Mat[5], cam_matx.m_Mat[6]);
	maVector3d view(-cam_matx.m_Mat[8], -cam_matx.m_Mat[9], -cam_matx.m_Mat[10]);

	// Store the unit size vectors for the circle icon later
	//maVector3d circ_left = left * l_CircleRadius, circ_up = up * l_CircleRadius;

	// scale down the vectors to make a smaller icon
	//const float c_IconScale = 0.5f;

	// Apply the scale and range
	left	*= i_Scale;
	up		*= i_Scale;
	view	*= i_Scale;

	// Store the scaled vectors for the circle icon later
	maVector3d circ_left = left * l_CircleRadius, circ_up = up * l_CircleRadius;

	// Incorporate field of view into icon
	float tan_fov = tanf((i_Angle / 2.0f) * maConstants::c_fAngleToRad);
	//if (i_Angle > 90)
	//{
	//	// In large angles, shorten view vector
	//	view /= tan_fov;
	//}
	//else if (i_Angle < 90)
	//{
	//	// In small angles, shorten left and up vectors
		left *= tan_fov;
		up *= tan_fov;
	//}

	// The root of the projection is set back along the view direction,
	// it is not at the origin (at the light's position).
	float near_dist = view.Length();
	maPoint3d origin = -view;

	l_LocalVerts[0].Set(0,0,0);
	l_LocalVerts[1] =   left + up / i_AspectRatio;
	l_LocalVerts[2] = - left + up / i_AspectRatio;
	l_LocalVerts[3] = - left - up / i_AspectRatio;
	l_LocalVerts[4] =   left - up / i_AspectRatio;

	l_LocalVerts[5] =   project_vertex(l_LocalVerts[1], origin, i_Range, near_dist);
	l_LocalVerts[6] =   project_vertex(l_LocalVerts[2], origin, i_Range, near_dist);
	l_LocalVerts[7] =   project_vertex(l_LocalVerts[3], origin, i_Range, near_dist);
	l_LocalVerts[8] =   project_vertex(l_LocalVerts[4], origin, i_Range, near_dist);

	l_LocalVerts[9] = i_Target - i_Pos;	// long line to target

	// Update the circle
	int vind = c_NumFrustrumVerts;
	float delta = maConstants::c_fPI_Times_2 / (float)c_NumCircleVerts;
	float angle = 0;
	for (int i=0; i<c_NumCircleVerts; i++, angle+=delta)
	{
		l_LocalVerts[vind++] = circ_left * cosf(angle) + circ_up * sinf(angle);
	}

	m_pFragment->UpdateVertices(c_NumIconVerts, &(l_LocalVerts[0]));

	// Show texture object only when the texture pointer is non-NULL
	effPhongData* pData = dynamic_cast<effPhongData*>(m_pTextureFrag->GetMaterial()->GetEffectData());
	if (pData)
	{
		pData->m_TextureDiffuse = i_pTexture;

		// Update the corners of the texture rectangle also
		maPoint3d rect_verts[4];
		rect_verts[0] =  l_LocalVerts[3];
		rect_verts[1] =  l_LocalVerts[4];
		rect_verts[2] =  l_LocalVerts[2];
		rect_verts[3] =  l_LocalVerts[1];
		m_pTextureFrag->UpdateVertices(4, &(rect_verts[0]));

		m_pTextureObject->SetRenderable( m_pObject->GetRenderable() && (i_pTexture != NULL));
	}


	// Icon's fragment only handles relative display from position to target.
	// The object position is set to place the icon correctly.
	m_pObject->SetPosition(i_Pos);
	m_pTextureObject->SetPosition(i_Pos);

	// Pick fragments, rotate cone to stay in light plane
	maRotation cone_rot, orient_rot;
	orient_rot.SetValue(maVector3d(0,1,0), maVector3d(0,0,1));
	cone_rot.SetValue(cam_matx);
	m_pPickPosition->SetOrientation(cone_rot * orient_rot); 
	m_pPickPosition->SetScale(maVector3d(i_Scale, 1, i_Scale));
	m_pPickPosition->SetPosition(i_Pos);
	// Target pick is just sphere at target position
	m_pPickTarget->SetPosition(i_Target);
	m_pPickTarget->SetUniformScale( api3dScale::GetGlobalScale() );
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void prjltIconObject::SetColor(const maFloatRGBA &i_Color)
{
	m_pObject->SetColor(i_Color);
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void prjltIconObject::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);

	effPhongData* pData = dynamic_cast<effPhongData*>(m_pTextureFrag->GetMaterial()->GetEffectData());
	if (pData)
	{
		m_pTextureObject->SetRenderable( i_Renderable && (pData->m_TextureDiffuse != NULL));
	}

	// Set pickable state from renderable state for the invisible pick icons
	m_pPickPosition->SetGPUPickable(i_Renderable);

	// Target changed to visible transparent icon
	//m_pPickTarget->SetGPUPickable(i_Renderable);
	m_pPickTarget->SetRenderable(i_Renderable);
}
bool prjltIconObject::GetRenderable() const
{
	return m_pObject->GetRenderable();
}

//----------------------------------------------------------------------------
//	Pickable sets whether the GPU pick icons should be pickable
//----------------------------------------------------------------------------
void prjltIconObject::SetPickable(bool i_bPickable)
{
	m_pPickPosition->SetGPUPickable(i_bPickable);
	m_pPickTarget->SetGPUPickable(i_bPickable);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool prjltIconObject::PositionContainsPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_pPickPosition->GetGPUPickable())
		return false;

	return m_pPickPosition->ContainsPickCode(i_PickCode);
}
bool prjltIconObject::TargetContainsPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_pPickTarget->GetGPUPickable())
		return false;

	return m_pPickTarget->ContainsPickCode(i_PickCode);
}

