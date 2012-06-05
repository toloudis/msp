/*****************************************************************************
**	ptltRangeIcon.cpp
**
**	3D Object that display range of falloff
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltRangeIcon.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScale.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"


namespace
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptltRangeIcon::ptltRangeIcon()
{
	// Material
	matMaterial *mat = new matMaterial("Phong.fx");

	effPhongData* pData = dynamic_cast<effPhongData*>(mat->GetEffectData());
	DBG_ASSERT0(pData != NULL, "ptltRangeIcon not using effPhong");
	pData->m_ColorDiffuse = maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	pData->m_ColorAmbient = maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	pData->m_ColorEmissive = maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	pData->m_ColorSpecular = maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);

	// Create fragment
	m_pFragment = g3dPrimitiveFragmentUtil::CreateReverseTexturedSphere(1.0f, 6, 6);

	// Set up fragment in object
	m_pObject = new api3dObjectSimple(m_pFragment, mat);
	m_pObject->SetGPUPickable(false);
	m_pObject->SetWireframe(true);
	api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptltRangeIcon::~ptltRangeIcon()
{
	api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
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

	m_pObject->SetPosition(i_Pos);
	m_pObject->SetUniformScale(i_Range);

	m_pObject->SetColor(maFloatRGBA(i_Percent, i_Percent, i_Percent, 1.0f));
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void ptltRangeIcon::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);
}
bool ptltRangeIcon::GetRenderable() const
{
	return m_pObject->GetRenderable();
}
