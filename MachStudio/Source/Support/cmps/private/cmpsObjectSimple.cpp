/*****************************************************************************
**	cmpsObjectSimple.cpp
**
**	Derived class, represents an element of geometry for a compass part. 
**	Adds on some convenience functions to the api3dObjectSimple api 
**	for setting color states on the compass part.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/private/cmpsObjectSimple.hpp"

#include "Core/Ma/maConstants.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/gpx/gpxIconSet.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"


//============================================================================
//============================================================================
namespace
{
	const maFloatRGBA c_Grey(0.5f, 0.5f, 0.5f, 0.25f);
	const maFloatRGBA c_Black(0.0f, 0.0f, 0.0f, 1.0f);
}


//--------------------------------------------------------------------
// constructor - this object assumes ownership of the arguments.
//	It will also assign the material to the fragment.
//--------------------------------------------------------------------
cmpsObjectSimple::cmpsObjectSimple(g3dFragment* i_pFragment, 
								   matMaterial *i_pMaterial,
								   cmpsRenderLayer::RenderLayer i_RenderLayer)
:	m_bActive(true),
	m_Factor(0.5f)
{
	api3dObjectSimple *pBaseObj = new api3dObjectSimple(i_pFragment, i_pMaterial);

	Init(pBaseObj, i_pMaterial, i_RenderLayer);
}

//--------------------------------------------------------------------
// constructor - this object assumes ownership of the arguments.
//	It will also assign the material to the fragment.
//--------------------------------------------------------------------
cmpsObjectSimple::cmpsObjectSimple(const maMatrix4x4 &i_Transform,
								   g3dFragment* i_pFragment, 
								   matMaterial *i_pMaterial,
								   cmpsRenderLayer::RenderLayer i_RenderLayer)
:	m_bActive(true),
	m_Factor(0.5f)
{
	api3dObjectSimple *pBaseObj = new api3dObjectSimple(i_Transform, i_pFragment, i_pMaterial);

	Init(pBaseObj, i_pMaterial, i_RenderLayer);
}
 

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsObjectSimple::~cmpsObjectSimple()
{
	delete m_pMaterialProxy;
	delete m_pObjectProxy;
	delete m_pObject;
}


//--------------------------------------------------------------------
// Add or remove object from api3dScene
//--------------------------------------------------------------------
//void cmpsObjectSimple::AddToScene(int i_Layer)
//{
//	// compass manipulators don´t go into scene anymore, they go into
//	// special icon layers per viewer
//	api3dScene::AddObject( m_pObject, i_Layer );
//}
//void cmpsObjectSimple::RemoveFromScene(int i_Layer)
//{
//	// compass manipulators don´t go into scene anymore, they go into
//	// special icon layers per viewer
//	api3dScene::RemoveObject( m_pObject, i_Layer );
//}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d  cmpsObjectSimple::GetPosition() const
{
	return m_pObjectProxy->GetPosition();
}
void  cmpsObjectSimple::SetPosition(const maPoint3d &i_Position)
{
	// check for changed value so that proxy isn´t always dirty
	//if (m_pObjectProxy->GetPosition() != i_Position)
	if ((m_pObjectProxy->GetPosition() - i_Position).LengthSqr() > maConstants::c_fEpsilon)
		m_pObjectProxy->SetPosition(i_Position);
}

//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation cmpsObjectSimple::GetOrientation() const
{
	return m_pObjectProxy->GetOrientation();
}
void  cmpsObjectSimple::SetOrientation(const maRotation &i_Rotation)
{
	// check for changed value so that proxy isn´t always dirty
	if (m_pObjectProxy->GetOrientation() != i_Rotation)
		m_pObjectProxy->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
maVector3d cmpsObjectSimple::GetScale() const
{
	return m_pObjectProxy->GetScale();
}
void  cmpsObjectSimple::SetScale(const maVector3d& i_Scale)
{
	// check for changed value so that proxy isn´t always dirty
	if (m_pObjectProxy->GetScale() != i_Scale)
	{
		m_pObjectProxy->SetScale(i_Scale);
	}
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void cmpsObjectSimple::SetRenderable(bool i_Renderable)
{
	// check for changed value so that proxy isn´t always dirty
	if (m_pObjectProxy->GetRenderable() != i_Renderable)
	{	
		m_pObjectProxy->SetRenderable(i_Renderable);
	}
}
bool cmpsObjectSimple::GetRenderable() const
{
	return m_pObjectProxy->GetRenderable();
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& cmpsObjectSimple::GetWorldBox() const
{
	return m_pObjectProxy->GetWorldBox();
}

//----------------------------------------------------------------------------
// Is the pick code given within the high and low pick codes assigned
// to this node?
//----------------------------------------------------------------------------
bool cmpsObjectSimple::ContainsPickCode(envType::UInt32 i_PickCode) const
{
	// No proxies for pick code
	return m_pObject->ContainsPickCode(i_PickCode);
}

//--------------------------------------------------------------------
// Set color for fragment only (don't set the base color)
//--------------------------------------------------------------------
void cmpsObjectSimple::ModifyColor(const maFloatRGBA &i_Color)
{
	if( i_Color != m_ColorData.m_Color )
	{
		m_ColorData.m_Color = i_Color;
		if (m_pMaterialProxy != NULL) {
			m_pMaterialProxy->SetEffectDataChanged();
		}
	}
/*
	if (i_Color != m_ColorData.m_ColorDiffuse)
	{
		m_ColorData.m_ColorEmissive = (i_Color*m_Factor);
		m_ColorData.m_ColorDiffuse = (i_Color); // store the "original" color in diffuse
		m_ColorData.m_ColorAmbient = (c_Black);
		m_ColorData.m_ColorSpecular = (c_Black);
		m_ColorData.m_Transparency = i_Color.GetAlpha();
		m_pMaterialProxy->SetEffectDataChanged();
	}
*/
}

//--------------------------------------------------------------------
// Set color for fragment
//--------------------------------------------------------------------
void cmpsObjectSimple::SetColor(const maFloatRGBA &i_Color)
{
	ModifyColor( i_Color );

	m_Color = i_Color;
}

//--------------------------------------------------------------------
// Get base color
//--------------------------------------------------------------------
const maFloatRGBA& cmpsObjectSimple::GetColor()
{
	return m_Color;
}

//----------------------------------------------------------------------------
// ModifyEmissive - alters emissive color by given factor, used
//	to highlight parts.
//----------------------------------------------------------------------------
void cmpsObjectSimple::ModifyEmissive( const float i_EmissiveFactor )
{
/*
	if (m_bActive)
	{
		maFloatRGBA color = m_ColorData.m_ColorDiffuse;		// the diffuse contains the "original" color
		color.SetRed( color.GetRed() * i_EmissiveFactor );
		color.SetGreen( color.GetGreen() * i_EmissiveFactor );
		color.SetBlue( color.GetBlue() * i_EmissiveFactor );
		color.SetAlpha( color.GetAlpha() );
		m_ColorData.m_ColorEmissive = ( color );
		m_pMaterialProxy->SetEffectDataChanged();
	}
	m_Factor = i_EmissiveFactor;
*/
}

//--------------------------------------------------------------------
//	DisplayActive changes the color of the icon to represent
//		when the object is active or disabled
//--------------------------------------------------------------------
void cmpsObjectSimple::DisplayActive(bool i_bActive)
{
	if (m_bActive != i_bActive)
	{
		m_bActive = i_bActive;
		if (m_bActive)
		{
			m_ColorData.m_Color = m_Color;
/*
			m_ColorData.m_ColorDiffuse = (m_Color);
			m_ColorData.m_ColorEmissive = (m_Color*m_Factor);
			m_ColorData.m_Transparency = m_Color.GetAlpha();
*/
			if (m_pMaterialProxy != NULL) {
				m_pMaterialProxy->SetEffectDataChanged();
			}
		}
		else
		{
			m_ColorData.m_Color = c_Grey;
/*			m_ColorData.m_ColorDiffuse = (c_Grey);
			m_ColorData.m_ColorEmissive = (c_Black);
			m_ColorData.m_Transparency = c_Grey.GetAlpha();
*/
			if (m_pMaterialProxy != NULL) {
				m_pMaterialProxy->SetEffectDataChanged();
			}
		}
	}
}

//--------------------------------------------------------------------
//	SetLayerScale changes the scale of the compass for only a 
// given icon layer (a render panel)
//--------------------------------------------------------------------
void  cmpsObjectSimple::SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale)
{
	m_pObjectProxy->SetLayerScale(i_IconLayerIndex, i_Scale);
}
maPoint3d cmpsObjectSimple::GetLayerScale(int i_IconLayerIndex) const
{
	return m_pObjectProxy->GetLayerScale(i_IconLayerIndex);
}

//--------------------------------------------------------------------
//	SetLayerRenderable sets the compass visibility for only a given icon 
// layer (a render panel)
//--------------------------------------------------------------------
void  cmpsObjectSimple::SetLayerRenderable(int i_IconLayerIndex, bool i_bRender)
{
	m_pObjectProxy->SetLayerRenderable(i_IconLayerIndex, i_bRender);
}
						     
//--------------------------------------------------------------------
// shared constructor code
//--------------------------------------------------------------------
void cmpsObjectSimple::Init(api3dObjectSimple *i_pBaseObj, 
							matMaterial *i_pMaterial,
							cmpsRenderLayer::RenderLayer i_RenderLayer)
{
	// Consider the render layer and choose icon or manipulator layer
	if (i_RenderLayer == cmpsRenderLayer::e_Manipulators)
		m_pObject = icnIconLayer::CreateManipulatorSet(i_pBaseObj);
	else
		m_pObject = icnIconLayer::CreateIconSet(i_pBaseObj);

	// Compasses are picked from ray picking techniques, not from GPU picking
	m_pObject->SetGPUPickable(false);

	// Create proxy to buffer changes to the scene object when multithreading
	m_pObjectProxy = new gpxIconSet(*m_pObject);
			
	// Also create a proxy for the phong data so we can set the color safely
	effSolidData* pData = dynamic_cast<effSolidData*>(i_pMaterial->GetEffectData());
	if (pData == NULL) {
		m_pMaterialProxy = NULL;
		return;
	}
	DBG_ASSERT(pData != NULL, "cmpsObjectSimple not using effSolid");
	m_ColorData = *pData; // Make thread-safe copy of data 
	m_pMaterialProxy = new gpxSolidData(m_ColorData, *pData); // proxy associates the two data structs
}
