/*****************************************************************************
**	cmpsObjectSimple.cpp
**
**	Derived class, represents an element of geometry for a compass part. 
**	Adds on some convenience functions to the api3dObjectSimple api 
**	for setting color states on the compass part.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsObjectSimple.hpp"

#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/mat/matMaterial.hpp"


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
cmpsObjectSimple::cmpsObjectSimple(g3dFragment* i_pFragment, matMaterial *i_pMaterial)
:	api3dObjectSimple(i_pFragment, i_pMaterial),
	m_bActive(true),
	m_Factor(0.5f)
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsObjectSimple::~cmpsObjectSimple()
{
}

//--------------------------------------------------------------------
// Set color for fragment only (don't set the base color)
//--------------------------------------------------------------------
void cmpsObjectSimple::ModifyColor(const maFloatRGBA &i_Color)
{
	effPhongData* pData = dynamic_cast<effPhongData*>(this->Material()->GetEffectData());
	DBG_ASSERT0(pData != NULL, "cmpsObjectSimple not using effPhong");

	pData->m_ColorEmissive = (i_Color*m_Factor);
	pData->m_ColorDiffuse = (i_Color); // store the "original" color in diffuse
	pData->m_ColorAmbient = (c_Black);
	pData->m_ColorSpecular = (c_Black);
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
	if (m_bActive)
	{
		effPhongData* pData = dynamic_cast<effPhongData*>(this->Material()->GetEffectData());
		DBG_ASSERT0(pData != NULL, "cmpsObjectSimple not using effPhong");
		
		maFloatRGBA color = pData->m_ColorDiffuse;		// the diffuse contains the "original" color
		color.SetRed( color.GetRed() * i_EmissiveFactor );
		color.SetGreen( color.GetGreen() * i_EmissiveFactor );
		color.SetBlue( color.GetBlue() * i_EmissiveFactor );
		color.SetAlpha( color.GetAlpha() );
		pData->m_ColorEmissive = ( color );
	}
	m_Factor = i_EmissiveFactor;
}

//--------------------------------------------------------------------
//	DisplayActive changes the color of the icon to represent
//		when the object is active or disabled
//--------------------------------------------------------------------
void cmpsObjectSimple::DisplayActive(bool i_bActive)
{
	m_bActive = i_bActive;

	effPhongData* pData = dynamic_cast<effPhongData*>(this->Material()->GetEffectData());
	DBG_ASSERT0(pData != NULL, "cmpsObjectSimple not using effPhong");

	if (m_bActive)
	{
		pData->m_ColorDiffuse = (m_Color);
		pData->m_ColorEmissive = (m_Color*m_Factor);
	}
	else
	{
		pData->m_ColorDiffuse = (c_Grey);
		pData->m_ColorEmissive = (c_Black);
	}
}
