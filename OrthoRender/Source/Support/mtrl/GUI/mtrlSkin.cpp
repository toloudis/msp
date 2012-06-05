/********************************************************************************************\
**  mtrlSkin.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlSkin.hpp"

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
#include "Graphics/eff/effSkinData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlSkin::mtrlSkin(mdlMaterialInfo& i_Data, 
				   effSkinData* i_pShaderData,
				   const fsLocator& i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT0(m_pShaderData != NULL, "mtrlSkin expected effSkinData");

	m_Data.m_SpecColor.SetValue(m_pShaderData->m_SpecColor);
	m_Data.m_SpecPower.SetValue(m_pShaderData->m_SpecPower);
	m_Data.m_SpecGloss.SetValue(m_pShaderData->m_SpecGloss);
	m_Data.m_SpecFresnel.SetValue(m_pShaderData->m_SpecFresnel);
	m_Data.m_FresnelPower.SetValue(m_pShaderData->m_FresnelPower);
	m_Data.m_FresnelGloss.SetValue(m_pShaderData->m_FresnelGloss);
	m_Data.m_TransColIn.SetValue(m_pShaderData->m_TransColIn);
	m_Data.m_TransColOut.SetValue(m_pShaderData->m_TransColOut);
	m_Data.m_TransColBack.SetValue(m_pShaderData->m_TransColBack);
	m_Data.m_TransMultiplier.SetValue(m_pShaderData->m_TransMultiplier);
	m_Data.m_TransRampOff.SetValue(m_pShaderData->m_TransRampOff);
	m_Data.m_MicroScale.SetValue(m_pShaderData->m_MicroScale);

	m_Data.m_DiffTex.SetValue(itString(m_pShaderData->m_NameDiffTex.c_str()));
	m_Data.m_NormalTex.SetValue(itString(m_pShaderData->m_NameNormalTex.c_str()));
	m_Data.m_MicroTex.SetValue(itString(m_pShaderData->m_NameMicroTex.c_str()));
	m_Data.m_SpecTex.SetValue(itString(m_pShaderData->m_NameSpecTex.c_str()));
	m_Data.m_SpecPowerTex.SetValue(itString(m_pShaderData->m_NameSpecPowerTex.c_str()));
	m_Data.m_TransTex.SetValue(itString(m_pShaderData->m_NameTransTex.c_str()));
	m_Data.m_CubeMapTex.SetValue(itString(m_pShaderData->m_NameCubeMapTex.c_str()));
	m_Data.m_ReflectFactorTex.SetValue(itString(m_pShaderData->m_NameReflectFactorTex.c_str()));
	m_Data.m_TransparencyTex.SetValue(itString(m_pShaderData->m_NameTransparencyTex.c_str()));

	m_Data.m_BumpMapScale.SetValue(m_pShaderData->m_BumpMapScale );
	m_Data.m_Transparency.SetValue(m_pShaderData->m_Transparency );
	m_Data.m_UScale.SetValue(m_pShaderData->m_UV.m_UScale );
	m_Data.m_VScale.SetValue(m_pShaderData->m_UV.m_VScale );
	m_Data.m_UTrans.SetValue(m_pShaderData->m_UV.m_UTrans );
	m_Data.m_VTrans.SetValue(m_pShaderData->m_UV.m_VTrans );
	m_Data.m_UVAngle.SetValue(m_pShaderData->m_UV.m_UVAngle );

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_SpecColor);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_SpecPower);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_SpecGloss);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_SpecFresnel);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_FresnelPower);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_FresnelGloss);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_TransColIn);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_TransColOut);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_TransColBack);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_TransMultiplier);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_TransRampOff);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_MicroScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_BumpMapScale);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Transparency);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VTrans);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UVAngle);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_DiffTex);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_NormalTex);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_MicroTex);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_SpecTex);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_SpecPowerTex);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TransTex);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_CubeMapTex);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_ReflectFactorTex);
	AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TransparencyTex);

	RegisterData(i_TextureDir);
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlSkin::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pDiffuseFileChooser->SetInitialDirectory( i_TextureDir );
	m_pDiffuseFileChooser->UpdateControl();

	m_pNormalFileChooser->SetInitialDirectory( i_TextureDir );
	m_pNormalFileChooser->UpdateControl();

	m_pSpecularColorFileChooser->SetInitialDirectory( i_TextureDir );
	m_pSpecularColorFileChooser->UpdateControl();

	m_pSpecularPowerFileChooser->SetInitialDirectory( i_TextureDir );
	m_pSpecularPowerFileChooser->UpdateControl();

	m_pTranslucencyFileChooser->SetInitialDirectory( i_TextureDir );
	m_pTranslucencyFileChooser->UpdateControl();

	m_pMicroStructureFileChooser->SetInitialDirectory( i_TextureDir );
	m_pMicroStructureFileChooser->UpdateControl();

	m_pCubeMapFileChooser->SetInitialDirectory( i_TextureDir );
	m_pCubeMapFileChooser->UpdateControl();

	m_pReflectFactorFileChooser->SetInitialDirectory( i_TextureDir );
	m_pReflectFactorFileChooser->UpdateControl();

	m_pTransparencyFileChooser->SetInitialDirectory( i_TextureDir );
	m_pTransparencyFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlSkin::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_Data.m_DiffTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_DiffTex.GetValue()));
	if (m_Data.m_NormalTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_NormalTex.GetValue()));
	if (m_Data.m_MicroTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_MicroTex.GetValue()));
	if (m_Data.m_SpecTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_SpecTex.GetValue()));
	if (m_Data.m_SpecPowerTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_SpecPowerTex.GetValue()));
	if (m_Data.m_TransTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TransTex.GetValue()));
	if (m_Data.m_CubeMapTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_CubeMapTex.GetValue()));
	if (m_Data.m_ReflectFactorTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_ReflectFactorTex.GetValue()));
	if (m_Data.m_TransparencyTex.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TransparencyTex.GetValue()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::RegisterData(const fsLocator &i_TextureDir)
{
	prtyPropertyUIInfo* pPUII = NULL;
	prtyRangedFloatUIInfo* pRFUII = NULL;

	m_pDiffuseFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_DiffTex), "Maps", "Diffuse Skin Map");
	m_pDiffuseFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pDiffuseFileChooser );
	m_pNormalFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_NormalTex), "Maps", "Normal Map");
	m_pNormalFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pNormalFileChooser );
	m_pCubeMapFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_CubeMapTex), "Maps", "Static Environment Map");
	m_pCubeMapFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pCubeMapFileChooser );
	m_pReflectFactorFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_ReflectFactorTex), "Maps", "Reflection mask Map");
	m_pReflectFactorFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pReflectFactorFileChooser );


	m_pSpecularColorFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_SpecTex), "Specular", "Specular Color Map (gloss in alpha)");
	m_pSpecularColorFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pSpecularColorFileChooser );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_SpecColor), "Specular", "Specular Color");
	AddProperty( pPUII );

	m_pSpecularPowerFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_SpecPowerTex), "Specular", "Specular Power Map");
	m_pSpecularPowerFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pSpecularPowerFileChooser );

	m_pTransparencyFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TransparencyTex), "Maps", "Transparency Map");
	m_pTransparencyFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pTransparencyFileChooser );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecPower), "Specular", "Specular Power");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecGloss), "Specular", "Specular Glossiness");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecFresnel), "Specular", "Fresnel");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_FresnelPower), "Specular", "Fresnel Power");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_FresnelGloss), "Specular", "Fresnel Gloss");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_TransColIn), "Translucency", "Unscattered color");
	AddProperty( pPUII );

	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_TransColOut), "Translucency", "Melanin color");
	AddProperty( pPUII );

	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_TransColBack), "Translucency", "Hemoglobin color");
	AddProperty( pPUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_TransMultiplier), "Translucency", "Translucent Power");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(20.0f);
	pRFUII->SetNumTicks(2000);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_TransRampOff), "Translucency", "Translucent Ramp Off");
	pRFUII->SetMinimum(-5.0f);
	pRFUII->SetMaximum(5.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );
	m_pTranslucencyFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TransTex), "Translucency", "Translucency Map");
	m_pTranslucencyFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pTranslucencyFileChooser );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_MicroScale), "Micro Structure", "Micro Structure UV Scale");
	pRFUII->SetMinimum(-1000.0f);
	pRFUII->SetMaximum(1000.0f);
	pRFUII->SetNumTicks(2000);
	AddProperty( pRFUII );

	m_pMicroStructureFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_MicroTex), "Micro Structure", "Micro Structure Normal Map");
	m_pMicroStructureFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pMicroStructureFileChooser );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_BumpMapScale), "Texture", "bump map scale");
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
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Transparency), "Transparency", "Transparency");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );


	m_Data.m_BumpMapScale.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_Transparency.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_UScale.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_VScale.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_UTrans.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_VTrans.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_UVAngle.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_DiffTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureDiff));
	m_Data.m_SpecTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureSpec));
	m_Data.m_SpecPowerTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureSpecPower));
	m_Data.m_NormalTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureNormal));
	m_Data.m_TransTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureTrans));
	m_Data.m_MicroTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureMicro));
	m_Data.m_CubeMapTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureCubeMap));
	m_Data.m_ReflectFactorTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureReflectFactor));
	m_Data.m_TransparencyTex.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::UpdateTextureTransparency));

	m_Data.m_SpecColor.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_SpecPower.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_SpecGloss.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_SpecFresnel.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_FresnelPower.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_FresnelGloss.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_TransColIn.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_TransColOut.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_TransColBack.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_TransMultiplier.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_TransRampOff.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
	m_Data.m_MicroScale.AddCallback(new prtyCallbackWrapper<mtrlSkin>(this, &mtrlSkin::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
		DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
		set_shader_data(pData);	// set into material template
	}
	set_shader_data(m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlSkin::set_shader_data(effSkinData* i_pData)
{
	i_pData->m_BumpMapScale = m_Data.m_BumpMapScale.GetValue();
	i_pData->m_Transparency = m_Data.m_Transparency.GetValue();
	i_pData->m_UV.m_UScale = m_Data.m_UScale.GetValue();
	i_pData->m_UV.m_VScale = m_Data.m_VScale.GetValue();
	i_pData->m_UV.m_UTrans = m_Data.m_UTrans.GetValue();
	i_pData->m_UV.m_VTrans = m_Data.m_VTrans.GetValue();
	i_pData->m_UV.m_UVAngle = m_Data.m_UVAngle.GetValue();
	i_pData->m_SpecColor = m_Data.m_SpecColor.GetValue();
	i_pData->m_SpecPower = m_Data.m_SpecPower.GetValue();
	i_pData->m_SpecGloss = m_Data.m_SpecGloss.GetValue();
	i_pData->m_SpecFresnel = m_Data.m_SpecFresnel.GetValue();
	i_pData->m_FresnelPower = m_Data.m_FresnelPower.GetValue();
	i_pData->m_FresnelGloss = m_Data.m_FresnelGloss.GetValue();
	i_pData->m_TransColIn = m_Data.m_TransColIn.GetValue();
	i_pData->m_TransColOut = m_Data.m_TransColOut.GetValue();
	i_pData->m_TransColBack = m_Data.m_TransColBack.GetValue();
	i_pData->m_TransMultiplier = m_Data.m_TransMultiplier.GetValue();
	i_pData->m_TransRampOff = m_Data.m_TransRampOff.GetValue();
	i_pData->m_MicroScale = m_Data.m_MicroScale.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureDiff(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_DiffTex, i_bDirty,
		m_pShaderData->m_NameDiffTex, m_pShaderData->m_DiffTex,
		pData->m_NameDiffTex, pData->m_DiffTex);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureSpec(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_SpecTex, i_bDirty,
		m_pShaderData->m_NameSpecTex, m_pShaderData->m_SpecTex,
		pData->m_NameSpecTex, pData->m_SpecTex);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureSpecPower(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_SpecPowerTex, i_bDirty,
		m_pShaderData->m_NameSpecPowerTex, m_pShaderData->m_SpecPowerTex,
		pData->m_NameSpecPowerTex, pData->m_SpecPowerTex);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureNormal(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_NormalTex, i_bDirty,
		m_pShaderData->m_NameNormalTex, m_pShaderData->m_NormalTex,
		pData->m_NameNormalTex, pData->m_NormalTex);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureTrans(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_TransTex, i_bDirty,
		m_pShaderData->m_NameTransTex, m_pShaderData->m_TransTex,
		pData->m_NameTransTex, pData->m_TransTex);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureMicro(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_MicroTex, i_bDirty,
		m_pShaderData->m_NameMicroTex, m_pShaderData->m_MicroTex,
		pData->m_NameMicroTex, pData->m_MicroTex);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureCubeMap(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_CubeMapTex, i_bDirty,
		m_pShaderData->m_NameCubeMapTex, m_pShaderData->m_CubeMapTex,
		pData->m_NameCubeMapTex, pData->m_CubeMapTex);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureReflectFactor(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_ReflectFactorTex, i_bDirty,
		m_pShaderData->m_NameReflectFactorTex, m_pShaderData->m_ReflectFactorTex,
		pData->m_NameReflectFactorTex, pData->m_ReflectFactorTex);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSkin::UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty)
{
	effSkinData* pData = dynamic_cast<effSkinData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlSkin expected effSkinData");
	UpdateTexture(m_Data.m_TransparencyTex, i_bDirty,
		m_pShaderData->m_NameTransparencyTex, m_pShaderData->m_TransparencyTex,
		pData->m_NameTransparencyTex, pData->m_TransparencyTex);
}

