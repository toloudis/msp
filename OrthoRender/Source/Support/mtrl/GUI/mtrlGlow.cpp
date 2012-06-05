/********************************************************************************************\
**  mtrlGlow.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlGlow.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/eff/effGlowData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlGlow::mtrlGlow(mdlMaterialInfo& i_Data, 
				   effGlowData* i_pShaderData,
				   const fsLocator &i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT0(m_pShaderData != NULL, "mtrlGlow expected effGlowData");

	m_Data.m_GlowAmount.SetValue(m_pShaderData->m_GlowAmount);
	m_Data.m_GlowScale.SetValue(m_pShaderData->m_GlowScale.GetX(),
		m_pShaderData->m_GlowScale.GetY(),
		m_pShaderData->m_GlowScale.GetZ());
	m_Data.m_GlowSize.SetValue(m_pShaderData->m_GlowSize);
	m_Data.m_bConstantGlow.SetValue(m_pShaderData->m_bConstantGlow);
	m_Data.m_GlowMask.SetValue(itString(m_pShaderData->m_NameGlowMask.c_str()));

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_GlowAmount);
	//this->AddVectorChannel(m_Material.GetMaterialName(), m_Data.m_GlowScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_GlowSize);
	this->AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_bConstantGlow);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_GlowMask);

	RegisterData(i_TextureDir); 
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlGlow::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pGlowFileChooser->SetInitialDirectory( i_TextureDir );
	m_pGlowFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlGlow::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_Data.m_GlowMask.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_GlowMask.GetValue()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlGlow::RegisterData(const fsLocator &i_TextureDir)
{
	m_pGlowFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_GlowMask), "Glow", "Glow Mask");
	m_pGlowFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pGlowFileChooser );

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bConstantGlow), "Glow", "Ignore specular") );

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_GlowAmount), "Glow", "Glow amount");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(100.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_GlowSize), "Glow", "glow size");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(20.0f);
	AddProperty( pRFUII );

	prtyVector3dEditUpDownUIInfo* pV3UII;
	pV3UII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_GlowScale), "Glow", "blur scaling");
	AddProperty( pV3UII );

	//
	m_Data.m_GlowAmount.AddCallback(new prtyCallbackWrapper<mtrlGlow>(this, &mtrlGlow::Update));
	m_Data.m_GlowScale.AddCallback(new prtyCallbackWrapper<mtrlGlow>(this, &mtrlGlow::Update));
	m_Data.m_GlowSize.AddCallback(new prtyCallbackWrapper<mtrlGlow>(this, &mtrlGlow::Update));
	m_Data.m_bConstantGlow.AddCallback(new prtyCallbackWrapper<mtrlGlow>(this, &mtrlGlow::Update));
	m_Data.m_GlowMask.AddCallback(new prtyCallbackWrapper<mtrlGlow>(this, &mtrlGlow::UpdateMaskTexture));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlGlow::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effGlowData& glow_data = m_Material.GlowParams();
		set_shader_data(glow_data);	// set into material template
	}
	set_shader_data(*m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlGlow::set_shader_data(effGlowData& i_Data)
{
	i_Data.m_bConstantGlow = m_Data.m_bConstantGlow.GetValue();
	i_Data.m_GlowAmount = m_Data.m_GlowAmount.GetValue();
	i_Data.m_GlowScale = maVector4d(m_Data.m_GlowScale.GetValue().GetX(),
		m_Data.m_GlowScale.GetValue().GetY(),
		m_Data.m_GlowScale.GetValue().GetZ(),
		1);
	i_Data.m_GlowSize = m_Data.m_GlowSize.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlGlow::UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty)
{
	effGlowData& glow_data = m_Material.GlowParams();
	UpdateTexture(m_Data.m_GlowMask, i_bDirty,
		m_pShaderData->m_NameGlowMask, m_pShaderData->m_pGlowMask,
		glow_data.m_NameGlowMask, glow_data.m_pGlowMask);
}

