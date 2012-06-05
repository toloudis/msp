/********************************************************************************************\
**  mtrlHair.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlHair.hpp"

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
#include "Graphics/eff/effHairData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlHair::mtrlHair(mdlMaterialInfo& i_Data, 
				   effHairData* i_pShaderData,
				   const fsLocator &i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT0(m_pShaderData != NULL, "mtrlHair expected effHairData");

	m_Data.m_HairBaseColor.SetValue(m_pShaderData->m_HairBaseColor );
	m_Data.m_BumpMapScale.SetValue(m_pShaderData->m_BumpMapScale );
	m_Data.m_UScale.SetValue(m_pShaderData->m_UV.m_UScale );
	m_Data.m_VScale.SetValue(m_pShaderData->m_UV.m_VScale );
	m_Data.m_UTrans.SetValue(m_pShaderData->m_UV.m_UTrans );
	m_Data.m_VTrans.SetValue(m_pShaderData->m_UV.m_VTrans );
	m_Data.m_UVAngle.SetValue(m_pShaderData->m_UV.m_UVAngle );
	m_Data.m_Reflectivity.SetValue(m_pShaderData->m_Reflectivity );

	m_Data.m_TextureBase.SetValue(itString(m_pShaderData->m_NameBase.c_str()));
	m_Data.m_TextureAlpha.SetValue(itString(m_pShaderData->m_NameAlpha.c_str()));
	m_Data.m_TextureSpecularShift.SetValue(itString(m_pShaderData->m_NameSpecularShift.c_str()));
	m_Data.m_TextureSpecularMask.SetValue(itString(m_pShaderData->m_NameSpecularMask.c_str()));
	m_Data.m_TextureNormalMap.SetValue(itString(m_pShaderData->m_NameNormalMap.c_str()));

	m_Data.m_SpecularColor0.SetValue(m_pShaderData->m_SpecularColor0);
	m_Data.m_SpecularColor1.SetValue(m_pShaderData->m_SpecularColor1);
	m_Data.m_SpecularExponent0.SetValue(m_pShaderData->m_SpecularExponent0);
	m_Data.m_SpecularExponent1.SetValue(m_pShaderData->m_SpecularExponent1);
	m_Data.m_SpecularShift0.SetValue(m_pShaderData->m_SpecularShift0);
	m_Data.m_SpecularShift1.SetValue(m_pShaderData->m_SpecularShift1);

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_HairBaseColor);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_BumpMapScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UVAngle);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Reflectivity);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_SpecularColor0);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_SpecularColor1);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_SpecularExponent0);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_SpecularExponent1);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_SpecularShift0);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_SpecularShift1);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureBase);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureAlpha);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureSpecularShift);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureSpecularMask);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureNormalMap);

	RegisterData(i_TextureDir);
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlHair::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pBaseHairFileChooser->SetInitialDirectory( i_TextureDir );
	m_pBaseHairFileChooser->UpdateControl();

	m_pAlphaFileChooser->SetInitialDirectory( i_TextureDir );
	m_pAlphaFileChooser->UpdateControl();

	m_pSpecularShiftFileChooser->SetInitialDirectory( i_TextureDir );
	m_pSpecularShiftFileChooser->UpdateControl();

	m_pSpecularNoiseFileChooser->SetInitialDirectory( i_TextureDir );
	m_pSpecularNoiseFileChooser->UpdateControl();

	m_pNormalFileChooser->SetInitialDirectory( i_TextureDir );
	m_pNormalFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlHair::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_Data.m_TextureBase.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureBase.GetValue()));
	if (m_Data.m_TextureAlpha.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureAlpha.GetValue()));
	if (m_Data.m_TextureSpecularShift.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureSpecularShift.GetValue()));
	if (m_Data.m_TextureSpecularMask.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureSpecularMask.GetValue()));
	if (m_Data.m_TextureNormalMap.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureNormalMap.GetValue()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlHair::RegisterData(const fsLocator &i_TextureDir)
{
	prtyPropertyUIInfo* pPUII;
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_HairBaseColor), "Color", "Main hair color");
	AddProperty( pPUII );

	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_SpecularColor0), "Specular Highlight 1", "Color");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecularExponent0), "Specular Highlight 1", "specular power (shininess)");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(200.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecularShift0), "Specular Highlight 1", "shift highlight along hair");
	pRFUII->SetMinimum(-1.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_SpecularColor1), "Specular Highlight 2", "Color");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecularExponent1), "Specular Highlight 2", "specular power (shininess)");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(200.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecularShift1), "Specular Highlight 2", "shift highlight along hair");
	pRFUII->SetMinimum(-1.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );

	m_pBaseHairFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureBase), "Maps", "Base Hair Map");
	m_pBaseHairFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pBaseHairFileChooser );
	m_pAlphaFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureAlpha), "Maps", "Alpha Map");
	m_pAlphaFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pAlphaFileChooser );
	m_pSpecularShiftFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureSpecularShift), "Maps", "Specular Shift Map");
	m_pSpecularShiftFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pSpecularShiftFileChooser );
	m_pSpecularNoiseFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureSpecularMask), "Maps", "Specular Noise Mask");
	m_pSpecularNoiseFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pSpecularNoiseFileChooser );
	m_pNormalFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureNormalMap), "Maps", "Normal Map");
	m_pNormalFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pNormalFileChooser );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Reflectivity), "Environment", "Reflectivity");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_BumpMapScale), "Bump", "bump map scale");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(20.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_UScale), "Texture", "U scale");
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

	m_Data.m_HairBaseColor.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_BumpMapScale.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_UScale.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_VScale.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_UTrans.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_VTrans.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_UVAngle.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_Reflectivity.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_TextureBase.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::UpdateTextureBase));
	m_Data.m_TextureAlpha.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::UpdateTextureAlpha));
	m_Data.m_TextureSpecularShift.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::UpdateTextureSpecularShift));
	m_Data.m_TextureSpecularMask.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::UpdateTextureSpecularMask));
	m_Data.m_TextureNormalMap.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::UpdateTextureNormalMap));
	m_Data.m_SpecularColor0.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_SpecularColor1.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_SpecularExponent0.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_SpecularExponent1.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_SpecularShift0.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
	m_Data.m_SpecularShift1.AddCallback(new prtyCallbackWrapper<mtrlHair>(this, &mtrlHair::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlHair::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effHairData* pData = dynamic_cast<effHairData*>(m_Material.ShaderData());
		DBG_ASSERT0(pData != NULL, "mtrlHair expected effHairData");
		set_shader_data(pData);	// set into material template
	}
	set_shader_data(m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlHair::set_shader_data(effHairData* i_pData)
{
	i_pData->m_HairBaseColor = m_Data.m_HairBaseColor.GetValue();
	i_pData->m_BumpMapScale = m_Data.m_BumpMapScale.GetValue();
	i_pData->m_UV.m_UScale = m_Data.m_UScale.GetValue();
	i_pData->m_UV.m_VScale = m_Data.m_VScale.GetValue();
	i_pData->m_UV.m_UTrans = m_Data.m_UTrans.GetValue();
	i_pData->m_UV.m_VTrans = m_Data.m_VTrans.GetValue();
	i_pData->m_UV.m_UVAngle = m_Data.m_UVAngle.GetValue();
	i_pData->m_Reflectivity = m_Data.m_Reflectivity.GetValue();
	i_pData->m_SpecularColor0 = m_Data.m_SpecularColor0.GetValue();
	i_pData->m_SpecularColor1 = m_Data.m_SpecularColor1.GetValue();
	i_pData->m_SpecularExponent0 = m_Data.m_SpecularExponent0.GetValue();
	i_pData->m_SpecularExponent1 = m_Data.m_SpecularExponent1.GetValue();
	i_pData->m_SpecularShift0 = m_Data.m_SpecularShift0.GetValue();
	i_pData->m_SpecularShift1 = m_Data.m_SpecularShift1.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlHair::UpdateTextureBase(prtyProperty *i_pProperty, bool i_bDirty)
{
	effHairData* pData = dynamic_cast<effHairData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlHair expected effHairData");
	UpdateTexture(m_Data.m_TextureBase, i_bDirty,
		m_pShaderData->m_NameBase, m_pShaderData->m_TextureBase,
		pData->m_NameBase, pData->m_TextureBase);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlHair::UpdateTextureAlpha(prtyProperty *i_pProperty, bool i_bDirty)
{
	effHairData* pData = dynamic_cast<effHairData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlHair expected effHairData");
	UpdateTexture(m_Data.m_TextureAlpha, i_bDirty,
		m_pShaderData->m_NameAlpha, m_pShaderData->m_TextureAlpha,
		pData->m_NameAlpha, pData->m_TextureAlpha);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlHair::UpdateTextureSpecularShift(prtyProperty *i_pProperty, bool i_bDirty)
{
	effHairData* pData = dynamic_cast<effHairData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlHair expected effHairData");
	UpdateTexture(m_Data.m_TextureSpecularShift, i_bDirty, 
		m_pShaderData->m_NameSpecularShift, m_pShaderData->m_TextureSpecularShift,
		pData->m_NameSpecularShift, pData->m_TextureSpecularShift);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlHair::UpdateTextureSpecularMask(prtyProperty *i_pProperty, bool i_bDirty)
{
	effHairData* pData = dynamic_cast<effHairData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlHair expected effHairData");
	UpdateTexture(m_Data.m_TextureSpecularMask, i_bDirty,
		m_pShaderData->m_NameSpecularMask, m_pShaderData->m_TextureSpecularMask,
		pData->m_NameSpecularMask, pData->m_TextureSpecularMask);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlHair::UpdateTextureNormalMap(prtyProperty *i_pProperty, bool i_bDirty)
{
	effHairData* pData = dynamic_cast<effHairData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlHair expected effHairData");
	UpdateTexture(m_Data.m_TextureNormalMap, i_bDirty,
		m_pShaderData->m_NameNormalMap, m_pShaderData->m_TextureNormalMap,
		pData->m_NameNormalMap, pData->m_TextureNormalMap);
}
