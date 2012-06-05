/********************************************************************************************\
**  mtrlUVTransform.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlUVTransform.hpp"

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
#include "Graphics/mat/matShaderEffect.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlUVTransform::mtrlUVTransform(mdlMaterialInfo& i_Data, 
				   effUVTransform* i_pShaderData,
				   const fsLocator &i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT0(m_pShaderData != NULL, "mtrlUVTransform expected effUVTransform");

	m_Data.m_UScale.SetValue(m_pShaderData->m_UScale );
	m_Data.m_VScale.SetValue(m_pShaderData->m_VScale );
	m_Data.m_UTrans.SetValue(m_pShaderData->m_UTrans );
	m_Data.m_VTrans.SetValue(m_pShaderData->m_VTrans );
	m_Data.m_UVAngle.SetValue(m_pShaderData->m_UVAngle );

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UVAngle);

	RegisterData(i_TextureDir); 
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlUVTransform::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlUVTransform::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlUVTransform::RegisterData(const fsLocator &i_TextureDir)
{
	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_UScale), "Texture", "U Scale");
	pRFUII->SetMinimum(-10.0f);
	pRFUII->SetMaximum(10.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_VScale), "Texture", "V Scale");
	pRFUII->SetMinimum(-10.0f);
	pRFUII->SetMaximum(10.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_UTrans), "Texture", "U Translate");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_VTrans), "Texture", "V Translate");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_UVAngle), "Texture", "UV Rotation angle");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	AddProperty( pRFUII );

	//
	m_Data.m_UScale.AddCallback(new prtyCallbackWrapper<mtrlUVTransform>(this, &mtrlUVTransform::Update));
	m_Data.m_VScale.AddCallback(new prtyCallbackWrapper<mtrlUVTransform>(this, &mtrlUVTransform::Update));
	m_Data.m_UTrans.AddCallback(new prtyCallbackWrapper<mtrlUVTransform>(this, &mtrlUVTransform::Update));
	m_Data.m_VTrans.AddCallback(new prtyCallbackWrapper<mtrlUVTransform>(this, &mtrlUVTransform::Update));
	m_Data.m_UVAngle.AddCallback(new prtyCallbackWrapper<mtrlUVTransform>(this, &mtrlUVTransform::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlUVTransform::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effUVTransform& glow_data = m_Material.UVTransform();
		set_shader_data(glow_data);	// set into material template
	}
	set_shader_data(*m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlUVTransform::set_shader_data(effUVTransform& i_Data)
{
	i_Data.m_UScale = m_Data.m_UScale.GetValue();
	i_Data.m_VScale = m_Data.m_VScale.GetValue();
	i_Data.m_UTrans = m_Data.m_UTrans.GetValue();
	i_Data.m_VTrans = m_Data.m_VTrans.GetValue();
	i_Data.m_UVAngle = m_Data.m_UVAngle.GetValue();
}
