/********************************************************************************************\
**  mtrlLambert.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlLambert.hpp"

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
#include "Graphics/eff/effLambertData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlLambert::mtrlLambert(mdlMaterialInfo& i_MatData, 
					 effLambertData* i_pShaderData,
					 const fsLocator& i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_MatData),
	m_pShaderData(i_pShaderData)
{
	// Set values into properties
	m_Data.m_ColorAmbient.SetValue(m_pShaderData->m_ColorAmbient );
	m_Data.m_ColorDiffuse.SetValue(m_pShaderData->m_ColorDiffuse );
	m_Data.m_ColorEmissive.SetValue(m_pShaderData->m_ColorEmissive );
	m_Data.m_BumpMapScale.SetValue(m_pShaderData->m_BumpMapScale );
	m_Data.m_Transparency.SetValue(m_pShaderData->m_Transparency );
	m_Data.m_UScale.SetValue(m_pShaderData->m_UV.m_UScale );
	m_Data.m_VScale.SetValue(m_pShaderData->m_UV.m_VScale );
	m_Data.m_UTrans.SetValue(m_pShaderData->m_UV.m_UTrans );
	m_Data.m_VTrans.SetValue(m_pShaderData->m_UV.m_VTrans );
	m_Data.m_UVAngle.SetValue(m_pShaderData->m_UV.m_UVAngle );


	m_Data.m_TextureDiffuse.SetValue(itString(m_pShaderData->m_NameDiffuse.c_str()));
	m_Data.m_TextureNormalMap.SetValue(itString(m_pShaderData->m_NameNormalMap.c_str()));
	m_Data.m_TextureTransparencyMap.SetValue(itString(m_pShaderData->m_NameTransparencyMap.c_str()));

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_ColorAmbient);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_ColorDiffuse);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_ColorEmissive);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_BumpMapScale);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Transparency);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UVAngle);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureDiffuse);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureNormalMap);
	AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureTransparencyMap);

	RegisterData(i_TextureDir);
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlLambert::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pDiffuseFileChooser->SetInitialDirectory( i_TextureDir );
	m_pDiffuseFileChooser->UpdateControl();

	m_pNormalFileChooser->SetInitialDirectory( i_TextureDir );
	m_pNormalFileChooser->UpdateControl();

	m_pTransparencyFileChooser->SetInitialDirectory( i_TextureDir );
	m_pTransparencyFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlLambert::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_Data.m_TextureDiffuse.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureDiffuse.GetValue()));
	if (m_Data.m_TextureNormalMap.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureNormalMap.GetValue()));
	if (m_Data.m_TextureTransparencyMap.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureTransparencyMap.GetValue()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlLambert::RegisterData(const fsLocator& i_TextureDir)
{
	prtyPropertyUIInfo* pPUII;
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ColorAmbient), "Color", "Ambient Color of the object");
	AddProperty( pPUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ColorDiffuse), "Color", "Diffuse Color of the object");
	AddProperty( pPUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ColorEmissive), "Color", "Emissive Color of the object");
	AddProperty( pPUII );

	m_pDiffuseFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureDiffuse), "Maps", "Diffuse Map");
	m_pDiffuseFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pDiffuseFileChooser );
	m_pNormalFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureNormalMap), "Maps", "Normal Map");
	m_pNormalFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pNormalFileChooser );
	m_pTransparencyFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureTransparencyMap), "Maps", "Transparency Map");
	m_pTransparencyFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pTransparencyFileChooser );

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_BumpMapScale), "Bump", "Bump map scale");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(20.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_UScale), "Texture", "U Scale");
	pRFUII->SetMinimum(-100.0f);
	pRFUII->SetMaximum(100.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_VScale), "Texture", "V Scale");
	pRFUII->SetMinimum(-100.0f);
	pRFUII->SetMaximum(100.0f);
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
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Transparency), "Transparency", "Transparency");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );

	m_Data.m_ColorAmbient.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_ColorDiffuse.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_ColorEmissive.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_BumpMapScale.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_Transparency.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_UScale.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_VScale.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_UTrans.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_VTrans.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_UVAngle.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::Update));
	m_Data.m_TextureDiffuse.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::UpdateTextureDiffuse));
	m_Data.m_TextureNormalMap.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::UpdateTextureNormal));
	m_Data.m_TextureTransparencyMap.AddCallback(new prtyCallbackWrapper<mtrlLambert>(this, &mtrlLambert::UpdateTextureTransparency));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlLambert::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effLambertData* pData = dynamic_cast<effLambertData*>(m_Material.ShaderData());
		DBG_ASSERT0(pData != NULL, "mtrlLambert expected effLambertData");
		set_shader_data(pData);	// set into material template
	}
	set_shader_data(m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlLambert::set_shader_data(effLambertData* i_pData)
{
	i_pData->m_ColorAmbient = m_Data.m_ColorAmbient.GetValue();
	i_pData->m_ColorDiffuse = m_Data.m_ColorDiffuse.GetValue();
	i_pData->m_ColorEmissive = m_Data.m_ColorEmissive.GetValue();
	i_pData->m_BumpMapScale = m_Data.m_BumpMapScale.GetValue();
	i_pData->m_Transparency = m_Data.m_Transparency.GetValue();
	i_pData->m_UV.m_UScale = m_Data.m_UScale.GetValue();
	i_pData->m_UV.m_VScale = m_Data.m_VScale.GetValue();
	i_pData->m_UV.m_UTrans = m_Data.m_UTrans.GetValue();
	i_pData->m_UV.m_VTrans = m_Data.m_VTrans.GetValue();
	i_pData->m_UV.m_UVAngle = m_Data.m_UVAngle.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlLambert::UpdateTextureDiffuse(prtyProperty *i_pProperty, bool i_bDirty)
{
	effLambertData* pData = dynamic_cast<effLambertData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlLambert expected effLambertData");
	UpdateTexture(m_Data.m_TextureDiffuse, i_bDirty, 
		m_pShaderData->m_NameDiffuse, m_pShaderData->m_TextureDiffuse,
		pData->m_NameDiffuse, pData->m_TextureDiffuse);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlLambert::UpdateTextureNormal(prtyProperty *i_pProperty, bool i_bDirty)
{
	effLambertData* pData = dynamic_cast<effLambertData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlLambert expected effLambertData");
	UpdateTexture(m_Data.m_TextureNormalMap, i_bDirty,
		m_pShaderData->m_NameNormalMap, m_pShaderData->m_TextureNormalMap,
		pData->m_NameNormalMap, pData->m_TextureNormalMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlLambert::UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty)
{
	effLambertData* pData = dynamic_cast<effLambertData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlLambert expected effLambertData");
	UpdateTexture(m_Data.m_TextureTransparencyMap, i_bDirty,
		m_pShaderData->m_NameTransparencyMap, m_pShaderData->m_TextureTransparencyMap,
		pData->m_NameTransparencyMap, pData->m_TextureTransparencyMap);
}
