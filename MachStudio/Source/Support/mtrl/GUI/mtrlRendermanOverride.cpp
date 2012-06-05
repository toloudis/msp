/********************************************************************************************\
**  mtrlRendermanOverride.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlRendermanOverride.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
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
#include "Graphics/eff/effRendermanOverrideData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlRendermanOverride::mtrlRendermanOverride(mdlMaterialInfo& i_Data, 
				   effRendermanOverrideData* i_pShaderData,
				   const fsLocator &i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT(m_pShaderData != NULL, "mtrlRendermanOverride expected effRendermanOverrideData");

	// Create thread-safe proxy for the material´s effect data.
	m_pShaderDataProxy.reset( new gpxEffectRendermanOverride(*m_pShaderData) );

	m_Data.m_ParamList.SetValue( m_pShaderData->m_ParamList );
	m_Data.m_AttributeList.SetValue( m_pShaderData->m_AttributeList );
	m_Data.m_ShaderLocation.SetValue(m_pShaderData->m_ShaderLocation);
	m_Data.m_bOverrideShader.SetValue( m_pShaderData->m_bOverrideShader );
	m_Data.m_bOverrideAttributes.SetValue( m_pShaderData->m_bOverrideAttributes );

	RegisterData(i_TextureDir); 
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlRendermanOverride::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlRendermanOverride::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlRendermanOverride::RegisterData(const fsLocator &i_TextureDir)
{
	prtyCheckBoxUIInfo * pCBUII;
	prtyTextBoxUIInfo* pPUII;
	
	pCBUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOverrideShader), "RenderMan Override", "Override Shader");
	AddProperty( pCBUII );

	m_pRendermanOverrideFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_ShaderLocation), "RenderMan Override", "Shader Location");
	AddProperty( m_pRendermanOverrideFileChooser );

	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_ParamList), "RenderMan Override", "Param List");
	pPUII->SetMultiline(true);
	pPUII->SetMultilineFixed(true);
	AddProperty( pPUII );

	pCBUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOverrideAttributes), "RenderMan Override", "Override Attributes");
	AddProperty( pCBUII );

	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_AttributeList), "RenderMan Override", "Attribute List");
	pPUII->SetMultiline(true);
	pPUII->SetMultilineFixed(true);
	AddProperty( pPUII );

	m_Data.m_ParamList.AddCallback(new prtyCallbackWrapper<mtrlRendermanOverride>(this, &mtrlRendermanOverride::Update));
	m_Data.m_AttributeList.AddCallback(new prtyCallbackWrapper<mtrlRendermanOverride>(this, &mtrlRendermanOverride::Update));
	m_Data.m_ShaderLocation.AddCallback(new prtyCallbackWrapper<mtrlRendermanOverride>(this, &mtrlRendermanOverride::Update));
	m_Data.m_bOverrideShader.AddCallback(new prtyCallbackWrapper<mtrlRendermanOverride>(this, &mtrlRendermanOverride::Update));
	m_Data.m_bOverrideAttributes.AddCallback(new prtyCallbackWrapper<mtrlRendermanOverride>(this, &mtrlRendermanOverride::Update));
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlRendermanOverride::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effRendermanOverrideData& rman_override_data = m_Material.RendermanOverrideParams();
		set_shader_data(rman_override_data);	// set into material template
		mtrlOperations::SetChunkDataChanged();
	}
	
	// use proxy to update material´s shader at a thread-safe time
	m_pShaderDataProxy->SetShaderLoc( m_Data.m_ShaderLocation.GetValue() );
	m_pShaderDataProxy->SetParamList(  m_Data.m_ParamList.GetValue().c_str() );
	m_pShaderDataProxy->SetAttributeList(  m_Data.m_AttributeList.GetValue().c_str() );
	m_pShaderDataProxy->SetOverrideShader( m_Data.m_bOverrideShader.GetValue() );
	m_pShaderDataProxy->SetOverrideAttributes( m_Data.m_bOverrideAttributes.GetValue() );
	
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlRendermanOverride::set_shader_data(effRendermanOverrideData& i_Data)
{
	i_Data.m_ShaderLocation = m_Data.m_ShaderLocation.GetValue();
	i_Data.m_ParamList = m_Data.m_ParamList.GetValue().c_str();	
	i_Data.m_AttributeList = m_Data.m_AttributeList.GetValue().c_str();	
	i_Data.m_bOverrideShader = m_Data.m_bOverrideShader.GetValue();
	i_Data.m_bOverrideAttributes = m_Data.m_bOverrideAttributes.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlRendermanOverride::UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty)
{
}