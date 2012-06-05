/*****************************************************************************
**	prjltIconSpot.cpp
**
**	3D Object that holds an icon for the projected light
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltIconSpot.hpp"

#include "Support/mnm/mnmPickMask.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/Cam/camCamera.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
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
	const float l_IconsScale = 0.0025f;
	const float c_PickPositionRadius = 1.55f;
	const float c_PickTargetRadius = 0.55f;
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
	// to the far plane.
	// percentage 0 = near, 1 = far.
	inline maPoint3d project_vertex(const maPoint3d &i_Pt, 
									const maPoint3d &i_Origin, 
									float i_Range,
									float i_Scale,
									bool i_bOrthographic,
									float i_Percentage = 1.0f)
	{
		if (i_bOrthographic)
			return i_Pt + (-i_Origin/i_Scale) * (i_Percentage*i_Range);
		else
			return i_Origin + (i_Pt - i_Origin) * ( (i_Percentage*i_Range + i_Scale) / i_Scale);
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconSpot::prjltIconSpot()
{
	// Material
	matMaterial *cone_mat = new matMaterial("Solid.fx");

	effSolidData* pData = dynamic_cast<effSolidData*>(cone_mat->GetEffectData());
	DBG_ASSERT(pData != NULL, "prjltIconSpot not using effSolid");
	pData->m_Color = maFloatRGBA(1.0f, 1.0f, 0.0f, 1.0f);

#if SHOW_FRUSTRUM
	matMaterial *mat = new matMaterial("Solid.fx");

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
	api3dObjectSimple *pObjectBase = new api3dObjectSimple(m_pFragment, mat);
	m_pObject = icnIconLayer::CreateIconSet(pObjectBase); // clones one icon per viewer
	m_pObject->SetGPUPickable(false);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());
#endif

	// Icon is cone shape (make initial size 1 and then scale it later)
	const float cone_radius = 1.0f;
	const float cone_height = 1.0f;
	g3dFragment *pConeFragment = CreateLineCone(cone_radius, cone_height, 36, cone_mat);

	// Set up fragment in object
	api3dObjectSimple *pConeBase = new api3dObjectSimple(pConeFragment, cone_mat);
	m_pCone = icnIconLayer::CreateIconSet(pConeBase); // clones one icon per viewer
	m_pCone->SetGPUPickable(false);
	m_pCone->SetWireframe(true);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	// Pickable spheres at position and target location
	api3dObjectSimple *pPickPositionBase = api3dShape::CreateSphere(maFloatRGBA(), 
		c_PickPositionRadius, 3, 5);
	pPickPositionBase->SetPickMask(mnmPickMask::c_Light);
	m_pPickPosition = icnIconLayer::CreateIconSet(pPickPositionBase); // clones one icon per viewer
	m_pPickPosition->SetRenderable(true);
//	m_pPickPosition->SetRenderable(false);
//	m_pPickPosition->SetPickHull(true);
	m_pPickPosition->SetGPUPickable(true);
	//api3dScene::AddObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());

	api3dObjectSimple *pPickTargetBase = api3dShape::CreateSphere(maFloatRGBA(1.0f, 0.9f, 0.9f, c_TargetTransparency), 
		c_PickTargetRadius, 3, 5);
	pPickTargetBase->SetPickMask(mnmPickMask::c_Light);
	m_pPickTarget = icnIconLayer::CreateIconSet(pPickTargetBase); // clones one icon per viewer
	m_pPickTarget->SetRenderable(true);
	//m_pPickTarget->SetPickHull(true);
	m_pPickTarget->SetGPUPickable(true);
	//api3dScene::AddObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());

	// create thread-safe proxies for the icon objects and fragments
	m_pConeProxy		  = new gpxIconSet(*m_pCone);
	m_pPickPositionProxy  = new gpxIconSet(*m_pPickPosition);
	m_pPickTargetProxy    = new gpxIconSet(*m_pPickTarget);
#if SHOW_FRUSTRUM
	m_pFragmentProxy      = new gpxFragment(*m_pFragment);
	m_pObjectProxy		  = new gpxIconSet(*m_pObject);
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconSpot::~prjltIconSpot()
{
	// delete proxies
	delete m_pConeProxy;
	delete m_pPickPositionProxy;
	delete m_pPickTargetProxy;

	//api3dScene::RemoveObject(m_pCone, mnmApp::GetIconsLayerIndex());
	delete m_pCone;
	//api3dScene::RemoveObject(m_pPickPosition, mnmApp::GetIconsLayerIndex());
	delete m_pPickPosition;
	//api3dScene::RemoveObject(m_pPickTarget, mnmApp::GetIconsLayerIndex());
	delete m_pPickTarget;

	// fragments owned by simple objects, we don't have to delete it

	
#if SHOW_FRUSTRUM
	delete m_pFragmentProxy;
	delete m_pObjectProxy;	//api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;
#endif
}

//--------------------------------------------------------------------
// Update vertices of morphable fragment match view
//--------------------------------------------------------------------
void  prjltIconSpot::Update(const maPoint3d &i_Pos, 
							  const maPoint3d &i_Target,
							  const maRotation &i_Orientation, //not correct for spot lights
							  float i_Scale,
							  float i_Tilt,
							  bool  i_bDirectional,
							  float i_Angle,
							  float i_InnerAngle,
							  float i_AspectRatio,
							  float i_Range,
							  matTexture* i_pTexture,
							  float i_TexturePosition)
{
	//DBG_LOG3("Update pos: %f %f %f", i_Pos.m_X, i_Pos.m_Y, i_Pos.m_Z);
	//DBG_LOG3("Update tgt: %f %f %f", i_Target.m_X, i_Target.m_Y, i_Target.m_Z);
#if SHOW_FRUSTRUM
	// Get up vector (can't be (0,1,0), if the pitch is 90)
	maVector3d cam_up(0,1,0);
	maVector3d view_diff = i_Target - i_Pos;
	if ( view_diff.m_Z * view_diff.m_Z + view_diff.m_X * view_diff.m_X  < maConstants::c_fEpsilon )
		cam_up.Set(0,0,1);

	// Compute tilted up vector
	if (i_Tilt != 0.0f)
	{
		maRotation rot(view_diff, maConstants::c_fAngleToRad * i_Tilt);
		rot.RotateVector(cam_up);
	}
	
	// spot lights have position at cone origin but g3dProjectedLights
	// have position on near plane, so have to adjust.
	float plane_dist =  i_Scale;
	maVector3d view_dir = i_Target - i_Pos;
	view_dir.Normalize();

	// Construct view matrix based on spot light..	
	maPoint3d pos = i_Pos + view_dir*plane_dist;
	maMatrix4x4 cam_matx;
	cam_matx.LookAt(pos, i_Target, cam_up);

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

	// Directional projected lights use orthographic frustrums and
	// therefore don't consider field of view
	if (i_bDirectional)
	{
		left *= 0.5f;
		up *= 0.5f;
	}
	else
	{
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
	}

	// The root of the projection is set back along the view direction,
	// it is not at the origin (at the light's position).
	float near_dist = view.Length();
	maPoint3d origin = -view;

	l_LocalVerts[0].Set(0,0,0);
	l_LocalVerts[1] =   left + up / i_AspectRatio;
	l_LocalVerts[2] = - left + up / i_AspectRatio;
	l_LocalVerts[3] = - left - up / i_AspectRatio;
	l_LocalVerts[4] =   left - up / i_AspectRatio;

	l_LocalVerts[5] =   project_vertex(l_LocalVerts[1], origin, i_Range, near_dist, i_bDirectional);
	l_LocalVerts[6] =   project_vertex(l_LocalVerts[2], origin, i_Range, near_dist, i_bDirectional);
	l_LocalVerts[7] =   project_vertex(l_LocalVerts[3], origin, i_Range, near_dist, i_bDirectional);
	l_LocalVerts[8] =   project_vertex(l_LocalVerts[4], origin, i_Range, near_dist, i_bDirectional);

	l_LocalVerts[9] = i_Target - pos; //i_Pos;	// long line to target

	// Update the circle
	int vind = c_NumFrustrumVerts;
	float delta = maConstants::c_fPI_Times_2 / (float)c_NumCircleVerts;
	float angle = 0;
	for (int i=0; i<c_NumCircleVerts; i++, angle+=delta)
	{
		l_LocalVerts[vind++] = circ_left * cosf(angle) + circ_up * sinf(angle);
	}

	m_pFragmentProxy->UpdateVertices(c_NumIconVerts, &(l_LocalVerts[0]));
	
	// Icon's fragment only handles relative display from position to target.
	// The object position is set to place the icon correctly.
	m_pObjectProxy->SetPosition(pos);
#endif

	// Rotate and position cone to point from position to target
	maRotation cone_rot;
	maVector3d light_dir = i_Target - i_Pos;
	cone_rot.SetValue(maVector3d(0,-1,0), light_dir);
	m_pConeProxy->SetPosition(i_Pos);
	m_pConeProxy->SetOrientation(cone_rot);
	
	float total_range = i_Range+i_Scale;
	float baseRadius = total_range*tan(maConstants::c_fAngleToRad*i_Angle*0.5f);
	m_pConeProxy->SetScale(maVector3d(baseRadius, total_range, baseRadius));


	// Pick fragments, rotate cone to stay in light plane
	m_pPickPositionProxy->SetPosition(i_Pos);
	// Target pick is just sphere at target position
	m_pPickTargetProxy->SetPosition(i_Target);
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
//void prjltIconSpot::GlobalScaleChanged( float i_Scale )
//{
//}

//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
//virtual 
void prjltIconSpot::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	if (m_pPickTarget)
	{
		float fTargetScale = i_Scale * icnIconScale::GetIconScaleForPosition(m_pPickTarget->GetPosition(), i_Camera);
		m_pPickTargetProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fTargetScale,fTargetScale,fTargetScale) );
		float fPositionScale = i_Scale * icnIconScale::GetIconScaleForPosition(m_pPickPosition->GetPosition(), i_Camera);
		m_pPickPositionProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fPositionScale,fPositionScale,fPositionScale) );
	}
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void prjltIconSpot::SetColor(const maFloatRGBA &i_Color)
{
	m_pConeProxy->SetColor(i_Color);
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void prjltIconSpot::SetRenderable(bool i_Renderable)
{
#if SHOW_FRUSTRUM
	m_pObjectProxy->SetRenderable(i_Renderable);
#endif
	m_pConeProxy->SetRenderable(i_Renderable);

	m_pPickPositionProxy->SetRenderable(i_Renderable);

	// Target changed to visible transparent icon
	//m_pPickTargetProxy->SetGPUPickable(i_Renderable);
	m_pPickTargetProxy->SetRenderable(i_Renderable);
}
bool prjltIconSpot::GetRenderable() const
{
	return m_pConeProxy->GetRenderable();
}

//----------------------------------------------------------------------------
//	Pickable sets whether the GPU pick icons should be pickable
//----------------------------------------------------------------------------
void prjltIconSpot::SetPickable(bool i_bPickable)
{
	if (i_bPickable != m_pPickPositionProxy->GetGPUPickable())
		m_pPickPositionProxy->SetGPUPickable(i_bPickable);
	if (i_bPickable != m_pPickTargetProxy->GetGPUPickable())
		m_pPickTargetProxy->SetGPUPickable(i_bPickable);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool prjltIconSpot::PositionContainsPickCode(envType::UInt32 i_PickCode) const
{
	// GPU picking is not proxied
	if (!m_pPickPosition->GetGPUPickable())
		return false;

	return m_pPickPosition->ContainsPickCode(i_PickCode);
}
bool prjltIconSpot::TargetContainsPickCode(envType::UInt32 i_PickCode) const
{
	// GPU picking is not proxied
	if (!m_pPickTarget->GetGPUPickable())
		return false;

	return m_pPickTarget->ContainsPickCode(i_PickCode);
}

//----------------------------------------------------------------------------
// Get current scale values for different views of the icons
//----------------------------------------------------------------------------
float	prjltIconSpot::GetPickPositionScale(int i_IconLayerIndex)
{
	return m_pPickPosition->GetLayerScale(i_IconLayerIndex).GetX();
}
float	prjltIconSpot::GetPickTargetScale(int i_IconLayerIndex)
{
	return m_pPickTargetProxy->GetLayerScale(i_IconLayerIndex).GetX();
}

