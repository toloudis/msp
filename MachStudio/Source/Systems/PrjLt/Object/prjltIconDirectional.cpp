/*****************************************************************************
**	prjltIconDirectional.cpp
**
**	3D Object that holds an icon for the directional light
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltIconDirectional.hpp"

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
	const float c_PickSphereRadius = 4.0f;
	const float c_PositionTransparency = 0.3f;

	const float l_CircleRadius = 0.25f;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconDirectional::prjltIconDirectional()
{
	// Material
	matMaterial *cone_mat = new matMaterial("Solid.fx");
	matMaterial *cyl_mat = new matMaterial("Solid.fx");

	effSolidData* pData = dynamic_cast<effSolidData*>(cone_mat->GetEffectData());
	DBG_ASSERT(pData != NULL, "prjltIconDirectional not using effSolid");
	pData->m_Color = maFloatRGBA(1.0f, 1.0f, 0.0f, 1.0f);
	pData = dynamic_cast<effSolidData*>(cyl_mat->GetEffectData());
	DBG_ASSERT(pData != NULL, "prjltIconDirectional not using effSolid");
	pData->m_Color = maFloatRGBA(1.0f, 1.0f, 0.0f, 1.0f);


	// Icon is arrow shape with cone point and cylinder stem
	const float cone_radius = 1.5f;
	const float cone_height = 3.0f;
	g3dFragment *pConeFragment = CreateCone(cone_radius, cone_height, 12, cone_mat);

	// Set up fragment in object
	maRotation orient_to_zaxis(maConstants::c_fPI_Div_2,0,0);
	maMatrix4x4 cone_matx = orient_to_zaxis.GetMatrix();
	cone_matx.TranslateBy(0, 0, 1.5f*cone_height);
	api3dObjectSimple *pConeBase = new api3dObjectSimple(cone_matx,pConeFragment, cone_mat);
	m_pCone = icnIconLayer::CreateIconSet(pConeBase); // clones one icon per viewer
	m_pCone->SetGPUPickable(false);
	m_pCone->SetWireframe(true);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	const float cyl_radius = 0.5f;
	const float cyl_height = 3.0f;
	g3dFragment *pCylinderFragment = g3dPrimitiveFragmentUtil::CreateCylinder(cyl_radius, cyl_radius, cyl_height, 12);
	pCylinderFragment->SetMaterial(cyl_mat);

	maMatrix4x4 cyl_matx = orient_to_zaxis.GetMatrix();
	cyl_matx.TranslateBy(0, 0, -cyl_height);
	api3dObjectSimple *pCylinderBase = new api3dObjectSimple(cyl_matx,pCylinderFragment, cyl_mat);
	m_pCylinder = icnIconLayer::CreateIconSet(pCylinderBase); // clones one icon per viewer
	m_pCylinder->SetGPUPickable(false);
	m_pCylinder->SetWireframe(true);

	api3dObjectSimple *pPickPositionBase = api3dShape::CreateSphere(maFloatRGBA(1.0f, 0.9f, 0.9f, c_PositionTransparency), 
		c_PickSphereRadius, 3, 5);
	pPickPositionBase->SetPickMask(mnmPickMask::c_Light);
	m_pPickPosition = icnIconLayer::CreateIconSet(pPickPositionBase); // clones one icon per viewer
	m_pPickPosition->SetRenderable(false);
	m_pPickPosition->SetGPUPickable(true);
	m_pPickPosition->SetPickHull(true);

	// create thread-safe proxies for the icon objects and fragments
	m_pConeProxy		  = new gpxIconSet(*m_pCone);
	m_pCylinderProxy	  = new gpxIconSet(*m_pCylinder);
	m_pPickPositionProxy  = new gpxIconSet(*m_pPickPosition);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconDirectional::~prjltIconDirectional()
{
	// delete proxies
	delete m_pConeProxy;
	delete m_pCylinderProxy;
	delete m_pPickPositionProxy;

	delete m_pCone;
	delete m_pCylinder;
	delete m_pPickPosition;

	// fragments owned by simple objects, we don't have to delete it
}

//--------------------------------------------------------------------
// Update vertices of morphable fragment match view
//--------------------------------------------------------------------
void  prjltIconDirectional::Update(const maPoint3d &i_Pos, 
							  const maPoint3d &i_Target,		 //not correct for directional lights
							  const maRotation &i_Orientation,
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

	// Rotate and position cone to point tip in direction of light, like head of arrow
	m_pConeProxy->SetPosition(i_Pos);
	m_pConeProxy->SetOrientation(i_Orientation);
	m_pCylinderProxy->SetPosition(i_Pos);
	m_pCylinderProxy->SetOrientation(i_Orientation);
	
	// Position pick is just sphere at position
	m_pPickPositionProxy->SetPosition(i_Pos);
}
//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
//virtual 
void prjltIconDirectional::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	if (m_pPickPosition)
	{
		float fPositionScale = i_Scale * icnIconScale::GetIconScaleForPosition(m_pPickPosition->GetPosition(), i_Camera);
		m_pPickPositionProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fPositionScale,fPositionScale,fPositionScale) );
		m_pConeProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fPositionScale,fPositionScale,fPositionScale) );
		m_pCylinderProxy->SetLayerScale( i_IconLayerIndex,  maVector3d(fPositionScale,fPositionScale,fPositionScale) );
	}
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void prjltIconDirectional::SetColor(const maFloatRGBA &i_Color)
{
	m_pConeProxy->SetColor(i_Color);
	m_pCylinderProxy->SetColor(i_Color);
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void prjltIconDirectional::SetRenderable(bool i_Renderable)
{
	m_pConeProxy->SetRenderable(i_Renderable);
	m_pCylinderProxy->SetRenderable(i_Renderable);

	// Set pickable state from renderable state for the invisible pick icons
	m_pPickPositionProxy->SetPickHull(i_Renderable);
}
bool prjltIconDirectional::GetRenderable() const
{
	return m_pConeProxy->GetRenderable();
}

//----------------------------------------------------------------------------
//	Pickable sets whether the GPU pick icons should be pickable
//----------------------------------------------------------------------------
void prjltIconDirectional::SetPickable(bool i_bPickable)
{
	if (i_bPickable != m_pPickPositionProxy->GetGPUPickable())
		m_pPickPositionProxy->SetGPUPickable(i_bPickable);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool prjltIconDirectional::PositionContainsPickCode(envType::UInt32 i_PickCode) const
{
	// GPU picking is not proxied
	if (!m_pPickPosition->GetGPUPickable())
		return false;

	return m_pPickPosition->ContainsPickCode(i_PickCode);
}
bool prjltIconDirectional::TargetContainsPickCode(envType::UInt32 i_PickCode) const
{
	// no target in this icon
	return false;
}

//----------------------------------------------------------------------------
// Get current scale values for different views of the icons
//----------------------------------------------------------------------------
float	prjltIconDirectional::GetPickPositionScale(int i_IconLayerIndex)
{
	return m_pPickPosition->GetLayerScale(i_IconLayerIndex).GetX();
}
float	prjltIconDirectional::GetPickTargetScale(int i_IconLayerIndex)
{
	// no target, just use position
	return m_pPickPosition->GetLayerScale(i_IconLayerIndex).GetX();
}

