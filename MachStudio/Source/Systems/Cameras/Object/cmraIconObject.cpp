/*****************************************************************************
**	cmraIconObject.cpp
**
**	3D Object that holds an icon for the camera representing its view
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraIconObject.hpp"

#include "Support/mnm/mnmPickMask.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/icn/icnIconScale.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/gpx/gpxFragment.hpp"
#include "Tool/gpx/gpxIconSet.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"

//============================================================================
//============================================================================
namespace
{
	const float c_PickTargetRadius = 0.4f;	
	const float c_PickPositionRadius = 2.5f;	
	const float c_TargetTransparency = 0.3f;

	const int c_NumIconVerts = 5+8; // 5 for pyramid, 8 for box
	const int c_NumIconInds = 16+24; // 16 for pyramid, 24 for box

	const int c_NumDOFIconVerts = 4; // 4 verts per quad

	const float c_IconSize = 1.0f;
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
	matMaterial *mat = new matMaterial("Solid.fx");
	effSolidData* pData = dynamic_cast<effSolidData*>(mat->GetEffectData());
	DBG_ASSERT(pData != NULL, "cmraIconObject not using effSolid");
	pData->m_Color = maFloatRGBA(0.38f, 0.65f, 0.95f, 1.0f);

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
	api3dObjectSimple *pObjectBase = new api3dObjectSimple(m_pFragment, mat);
	m_pObject = icnIconLayer::CreateIconSet(pObjectBase); // clones one icon per viewer
	m_pObject->SetGPUPickable(false);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	// create 4 distinct DOF planes, different colors. 
	// if these were not different colors, then we could do this with the main fragment.
	std::vector<maPoint3d> dof_line_verts(c_NumDOFIconVerts, maPoint3d(0,0,0));
	api3dObjectSimple *pDOFObjectBase[4];
	for (int i = 0; i < 4; i++)
	{
		//m_pDOFObject[i] = api3dShape::CreateLineList(maFloatRGBA(0.25f + 0.75f*(float)(3-i)/3.0f,
		//	0.25f + 0.75f*(float)(3-i)/3.0f, 0.0f, 1.0f),
		//	&(dof_line_verts[0]), c_NumDOFIconVerts, true, true);
		//color code blur and focal
		if(i==0 || i==3)
		{
			pDOFObjectBase[i] = api3dShape::CreateLineList(maFloatRGBA(0.13f, 0.31f, 0.52f, 1.0f),
				&(dof_line_verts[0]), c_NumDOFIconVerts, true, true);
		}
		else
		{
			pDOFObjectBase[i] = api3dShape::CreateLineList(maFloatRGBA(0.38f, 0.65f, 0.95f, 1.0f),
				&(dof_line_verts[0]), c_NumDOFIconVerts, true, true);
		}
		m_pDOFObject[i] = icnIconLayer::CreateIconSet(pDOFObjectBase[i]); // clones one icon per viewer
		m_pDOFObject[i]->SetGPUPickable(false);
		//api3dScene::AddObject(m_pDOFObject[i], mnmApp::GetIconsLayerIndex());
	}

	// Pickable spheres at position and target location
	api3dObjectSimple *pPickPositionBase = api3dShape::CreateSphere(maFloatRGBA(), 
		c_PickPositionRadius, 8, 8); // make position bigger, more like box of camera
	pPickPositionBase->SetPickMask(mnmPickMask::c_Camera);
	m_pPickPosition = icnIconLayer::CreateIconSet(pPickPositionBase); // clones one icon per viewer
	//api3dScene::AddObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());
	m_pPickPosition->SetRenderable(false);
	m_pPickPosition->SetPickHull(true);
	m_pPickPosition->SetGPUPickable(true);

	api3dObjectSimple *pPickTargetBase = api3dShape::CreateSphere(maFloatRGBA(0.38f, 0.65f, 0.95f, c_TargetTransparency), 
		c_PickTargetRadius, 2, 4);
	pPickTargetBase->SetPickMask(mnmPickMask::c_Camera);
	m_pPickTarget = icnIconLayer::CreateIconSet(pPickTargetBase); // clones one icon per viewer
	//api3dScene::AddObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());
	//m_pPickTarget->SetRenderable(false);
	//m_pPickTarget->SetPickHull(true);
	m_pPickTarget->SetGPUPickable(true);

	// create thread-safe proxies for the icon objects and fragments
	m_pFragmentProxy	  = new gpxFragment(*m_pFragment);
	m_pObjectProxy		  = new gpxIconSet(*m_pObject);
	for (int i = 0; i < 4; i++)
	{
		m_pDOFObjectProxy[i]  = new gpxIconSet(*m_pDOFObject[i]);
		m_pDOFFragmentProxy[i]  = new gpxFragment(*pDOFObjectBase[i]->Fragment());
	}
	m_pPickPositionProxy  = new gpxIconSet(*m_pPickPosition);
	m_pPickTargetProxy	  = new gpxIconSet(*m_pPickTarget);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraIconObject::~cmraIconObject()
{
	// delete proxies
	delete m_pFragmentProxy;
	delete m_pObjectProxy;
	for (int i = 0; i < 4; i++)
	{
		delete m_pDOFObjectProxy[i];
		delete m_pDOFFragmentProxy[i];
	}
	delete m_pPickPositionProxy;
	delete m_pPickTargetProxy;

	for (int i = 0; i < 4; i++)
	{
		//api3dScene::RemoveObject(m_pDOFObject[i], mnmApp::GetIconsLayerIndex());
		delete m_pDOFObject[i];
	}
	//api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;

	//api3dScene::RemoveObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());
	delete m_pPickPosition;
	//api3dScene::RemoveObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());
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

	DBG_ASSERT( m_pFragment->GetVertexFormat() == g3dType::e_BumpTex1Vertex, "Only g3dType::BumpTex1Vertex format implemented" );
	DBG_ASSERT( c_NumIconVerts == m_pFragment->GetNumVertices(), "Number of icon vertices is incorrect" );

	m_Position	= i_Pos;
	m_Target	= i_Target;
	m_Tilt		= i_Tilt;
	m_FieldOfView	= i_FieldOfView;
	m_AspectRatio	= i_AspectRatio;
	m_NearBlurDist	= i_NearBlurDist;
	m_NearFocalDist	= i_NearFocalDist;
	m_FarFocalDist	= i_FarFocalDist;
	m_FarBlurDist	= i_FarBlurDist;

	m_pObjectProxy->SetPosition(i_Pos);

	// NOTE: This needs to be a AlterVertices() function in base fragment class

	//DBG_LOG6("UpdIcon pos: %6.3f %6.3f %6.3f   tgt: %6.3f %6.3f %6.3f", i_Pos.m_X, i_Pos.m_Y, i_Pos.m_Z, i_Target.m_X, i_Target.m_Y, i_Target.m_Z );

	// Get up vector (can't be (0,1,0), if the pitch is 90)
	maVector3d cam_up(0,1,0);
	maVector3d view_diff = i_Target - i_Pos;
	if ( view_diff.m_Z * view_diff.m_Z + view_diff.m_X * view_diff.m_X  < maConstants::c_fEpsilon )
		cam_up.Set(0,0,1);

	// Compute tilted up vector
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
	float leftScale = c_IconSize;
	float upScale = c_IconSize;
	float viewScale = c_IconSize;

	// Incorporate field of view into icon
	float tan_fov = tanf((i_FieldOfView / 2.0f) * maConstants::c_fAngleToRad);
	if (i_FieldOfView > 90)
	{
		// In large angles, shorten view vector
		viewScale /= tan_fov;
	}
	else if (i_FieldOfView < 90)
	{
		// In small angles, shorten left and up vectors
		leftScale *= tan_fov;
		upScale *= tan_fov;
	}


	// Only put offset into vertex (position comes from transformation)
	// Vertex 0 stays (0,0,0)

	//temp Scaled vectors
	maVector3d sLeft = left * leftScale;
	maVector3d sUp = up * upScale;
	maVector3d sView = view * viewScale;

	// Position of four corners of view plane
	std::vector<maPoint3d> line_verts(c_NumIconVerts);
	line_verts[1] =  sView + sLeft * i_AspectRatio + sUp;
	line_verts[2] =  sView - sLeft * i_AspectRatio + sUp;
	line_verts[3] =  sView - sLeft * i_AspectRatio - sUp;
	line_verts[4] =  sView + sLeft * i_AspectRatio - sUp;
	// Position of eight corners of camera box
	line_verts[5] =  sLeft + sUp;
	line_verts[6] =  -1 * sLeft + sUp;
	line_verts[7] =  -1 * sLeft - sUp;
	line_verts[8] =  sLeft - sUp;
	line_verts[9] =  - 2 * sView + sLeft + sUp;
	line_verts[10] =  - 2 * sView - sLeft + sUp;
	line_verts[11] =  - 2 * sView - sLeft - sUp;
	line_verts[12] =  - 2 * sView + sLeft - sUp;
	m_pFragmentProxy->UpdateVertices(c_NumIconVerts, &line_verts[0] );


	// reposition DOF planes:
	float distances[4] = {i_NearBlurDist, i_NearFocalDist, i_FarFocalDist, i_FarBlurDist};
	maPoint3d dof_line_verts[c_NumDOFIconVerts];
	for (int i = 0; i < 4; i++)
	{
		m_pDOFObjectProxy[i]->SetPosition(i_Pos);

		const float gScale = 1.0f / c_IconSize;

		// Position of four corners of DOF near blur plane.
		// The (1.0f/c_IconScale) factor compensates for the view, left, and up vectors 
		// being premultiplied above. We need these distances to be true, unscaled.
		dof_line_verts[0] =  gScale * (view + left * i_AspectRatio + up) * distances[i];
		dof_line_verts[1] =  gScale * (view - left * i_AspectRatio + up) * distances[i];
		dof_line_verts[2] =  gScale * (view - left * i_AspectRatio - up) * distances[i];
		dof_line_verts[3] =  gScale * (view + left * i_AspectRatio - up) * distances[i];
		m_pDOFFragmentProxy[i]->UpdateVertices(c_NumDOFIconVerts, &dof_line_verts[0]);
	}

	// Position pick is just sphere at camera position
	m_pPickPositionProxy->SetPosition(i_Pos);
	// Target pick is just sphere at target position
	m_pPickTargetProxy->SetPosition(i_Target);
}

//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
void cmraIconObject::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	// Camera icon has two parts that scale separately,
	// the position icons and the target icon
	float fPosScale = i_Scale * icnIconScale::GetIconScaleForPosition(m_pPickPosition->GetPosition(), i_Camera);
	m_pObjectProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fPosScale,fPosScale,fPosScale) );
	m_pPickPositionProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fPosScale,fPosScale,fPosScale) );
	
	float fTargetScale = i_Scale * icnIconScale::GetIconScaleForPosition(m_pPickTarget->GetPosition(), i_Camera);
	m_pPickTargetProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fTargetScale,fTargetScale,fTargetScale) );
}

//----------------------------------------------------------------------------
//	Set global scale into icon
//----------------------------------------------------------------------------
//void cmraIconObject::SetUniformScale(float i_Scale)
//{
//	m_pObject->SetUniformScale( i_Scale );
////	for (int i = 0; i < 4; i++)
////		m_pDOFObject[i]->SetUniformScale( i_Scale );	//don't scale DOF planes (they need to be in world space)
//	m_pPickPosition->SetUniformScale( i_Scale );
//	m_pPickTarget->SetUniformScale( i_Scale );
//}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void cmraIconObject::SetRenderable(bool i_Renderable)
{
	m_pObjectProxy->SetRenderable(i_Renderable);
	for (int i = 0; i < 4; i++)
		m_pDOFObjectProxy[i]->SetRenderable( i_Renderable );

	// Set pickable state from renderable state for the invisible pick icons
	m_pPickPositionProxy->SetPickHull(i_Renderable);

	// Target changed to visible transparent icon
	//m_pPickTargetProxy->SetGPUPickable(i_Renderable);
	m_pPickTargetProxy->SetRenderable(i_Renderable);
}
bool cmraIconObject::GetRenderable() const
{
	return m_pObjectProxy->GetRenderable();
}

//----------------------------------------------------------------------------
//	Pickable sets whether the GPU pick icons should be pickable
//----------------------------------------------------------------------------
void cmraIconObject::SetPickable(bool i_bPickable)
{
	if (i_bPickable != m_pPickPositionProxy->GetGPUPickable())
		m_pPickPositionProxy->SetGPUPickable(i_bPickable);
	if (i_bPickable != m_pPickTargetProxy->GetGPUPickable())
		m_pPickTargetProxy->SetGPUPickable(i_bPickable);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool cmraIconObject::PositionContainsPickCode(envType::UInt32 i_PickCode) const
{
	// GPU picking is not proxied
	if (!m_pPickPosition->GetGPUPickable())
		return false;

	return m_pPickPosition->ContainsPickCode(i_PickCode);
}
bool cmraIconObject::TargetContainsPickCode(envType::UInt32 i_PickCode) const
{
	// GPU picking is not proxied
	if (!m_pPickTarget->GetGPUPickable())
		return false;

	return m_pPickTarget->ContainsPickCode(i_PickCode);
}

//----------------------------------------------------------------------------
// Get current scale values for different views of the icons
//----------------------------------------------------------------------------
float	cmraIconObject::GetCameraObjectScale(int i_IconLayerIndex)
{
	return c_PickPositionRadius * m_pObjectProxy->GetLayerScale(i_IconLayerIndex).GetX();
}
float	cmraIconObject::GetPickTargetScale(int i_IconLayerIndex)
{
	return c_PickTargetRadius * m_pPickTargetProxy->GetLayerScale(i_IconLayerIndex).GetX();
}
