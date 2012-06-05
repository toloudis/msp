/********************************************************************************************\
**  mtrlGlow.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlGlow.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"

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
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
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
	DBG_ASSERT(m_pShaderData != NULL, "mtrlGlow expected effGlowData");

	// Create thread-safe proxy for effect data.
	m_pShaderDataProxy.reset( new gpxEffectGlow(*m_pShaderData) );

	m_Data.m_GlowAmount.SetValue(m_pShaderData->m_GlowAmount);
	m_Data.m_GlowScale.Set(m_pShaderData->m_GlowScale.GetX(),
		m_pShaderData->m_GlowScale.GetY(),
		m_pShaderData->m_GlowScale.GetZ());
	m_Data.m_GlowSize.SetValue(m_pShaderData->m_GlowSize);
	m_Data.m_bConstantGlow.SetValue(m_pShaderData->m_bConstantGlow);

	prtyTextureFileData tex;
	tex.m_TextureLocator = m_pShaderData->m_NameGlowMask;
	tex.m_CurrentCallback = m_Data.m_GlowMask.GetFullValue().m_CurrentCallback;  //preserve the current callback

	m_Data.m_GlowMask.SetValue(tex);
	m_Data.m_GlowMask.SetRevertValue(tex);
	
	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_GlowAmount);
	this->AddVectorChannel(m_Material.GetMaterialName(), m_Data.m_GlowScale);
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
	if (m_Data.m_GlowMask.GetValue().GetNumNames() > 1)
		o_TextureList.push_back(m_Data.m_GlowMask.GetValue()); // fullpath
	else if (m_Data.m_GlowMask.GetValue().GetNumNames() == 1)
		o_TextureList.push_back(LocateTexture(m_Data.m_GlowMask.GetValue().GetLastName())); // filename needs locating
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlGlow::RegisterData(const fsLocator &i_TextureDir)
{
	m_pGlowFileChooser = new prtyTextureFileChooserUIInfo(&(m_Data.m_GlowMask), "Glow", "Glow Mask");
	//m_pGlowFileChooser->SetInitialDirectory( i_TextureDir );
	m_pGlowFileChooser->SetDirectoryCategory("Textures");
	//m_pGlowFileChooser->AddItem(m_pGlowFileChooser->e_Ramp);
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
		mtrlOperations::SetChunkDataChanged();
	}

	// use proxy to update material´s shader at a thread-safe time
	m_pShaderDataProxy->SetConstantGlow( m_Data.m_bConstantGlow.GetValue() );
	m_pShaderDataProxy->SetGlowAmount( m_Data.m_GlowAmount.GetValue() );
	m_pShaderDataProxy->SetGlowScale( maVector4d(m_Data.m_GlowScale.GetValue().GetX(),
												m_Data.m_GlowScale.GetValue().GetY(),
												m_Data.m_GlowScale.GetValue().GetZ(),
												1) );
	m_pShaderDataProxy->SetGlowSize( m_Data.m_GlowSize.GetValue() );
	 
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
	std::string callback( m_Data.m_GlowMask.GetFullValue().m_CurrentCallback );
/*
	//if the button on one of the texture options was clicked we need to pass
	//the necessary info to the manager of that operation
	if( m_Data.m_GlowMask.GetFullValue().m_bButtonPressed )
	{
		//reset our texture locator
		prtyTextureFileData val;
		val.m_TextureLocator = m_Data.m_GlowMask.GetValue();

		//decide which callback was executed
		if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Paint) )
		{
			//do paint manager operations
			m_bEnabledRamp = false;
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
		{
			NotifyRampUI();
			m_bEnabledRamp = true;
			m_pEnvironment->RampChanged(m_Data.m_RampData);
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Reset) )
		{
			callback = prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture);
			m_bEnabledRamp = false;
			m_Data.m_RampData = rmpData();
			
			m_pEnvironment->SetDiffuseTexture(NULL);
		}

		val.m_CurrentCallback = callback;
		m_Data.m_GlowMask.SetValueWithoutNotify(val);
	}
	*/
	
	UpdateTexture(m_Data.m_GlowMask, i_bDirty,
		m_pShaderData->m_NameGlowMask, m_pShaderData->m_pGlowMask,
		glow_data.m_NameGlowMask, glow_data.m_pGlowMask);

	if (i_bDirty)
		mtrlOperations::SetChunkDataChanged();
}

