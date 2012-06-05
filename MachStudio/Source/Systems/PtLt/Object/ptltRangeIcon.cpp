/*****************************************************************************
**	ptltRangeIcon.cpp
**
**	3D Object that display range of falloff
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltRangeIcon.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptltRangeIcon::ptltRangeIcon()
{
	// Material
	matMaterial *mat = new matMaterial("Solid.fx");
	DBG_ASSERT( mat != NULL, "Could not load Solid.fx");

	effSolidData* pData = dynamic_cast<effSolidData*>(mat->GetEffectData());
	DBG_ASSERT(pData != NULL, "ptltRangeIcon not using effSolid");

	pData->m_Color = maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);

	// Create fragment
	m_pFragment = g3dPrimitiveFragmentUtil::CreateReverseTexturedSphere(1.0f, 6, 6);

	// Set up fragment in object
	api3dObjectSimple *pObjectBase = new api3dObjectSimple(m_pFragment, mat);
	m_pObject = icnIconLayer::CreateIconSet(pObjectBase); // clones one icon per viewer
	m_pObject->SetGPUPickable(false);
	m_pObject->SetWireframe(true);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	// create thread-safe proxy for the icon
	m_pObjectProxy = new gpxSceneObject(*m_pObject);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptltRangeIcon::~ptltRangeIcon()
{
	delete m_pObjectProxy;

	//api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;

	// fragments owned by simple objects, we don't have to delete it
}

//--------------------------------------------------------------------
// Update vertices of morphable fragment match view
//--------------------------------------------------------------------
void  ptltRangeIcon::Update(const maPoint3d &i_Pos, 
							float i_Range,
							float i_Percent)
{
	//DBG_LOG3("Update pos: %f %f %f", i_Pos.m_X, i_Pos.m_Y, i_Pos.m_Z);

	m_pObjectProxy->SetPosition(i_Pos);
	m_pObjectProxy->SetUniformScale(i_Range);

	m_pObjectProxy->SetColor(maFloatRGBA(i_Percent, i_Percent, i_Percent, 1.0f));
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void ptltRangeIcon::SetRenderable(bool i_Renderable)
{
	m_pObjectProxy->SetRenderable(i_Renderable);
}
bool ptltRangeIcon::GetRenderable() const
{
	return m_pObjectProxy->GetRenderable();
}
