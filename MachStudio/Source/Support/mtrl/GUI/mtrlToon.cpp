/********************************************************************************************\
**  mtrlToon.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlToon.hpp"

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
#include "Graphics/eff/effToonData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlToon::mtrlToon(mdlMaterialInfo& i_MatData, 
					 effToonData* i_pShaderData,
					 const fsLocator& i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_MatData),
	m_pShaderData(i_pShaderData)
{
	// Set values into properties
	m_Data.m_ColorAmbient.SetValue(m_pShaderData->m_ColorAmbient );
	m_Data.m_ColorDiffuse.SetValue(m_pShaderData->m_ColorDiffuse );
	m_Data.m_ColorSpecular.SetValue(m_pShaderData->m_ColorSpecular );
	m_Data.m_ColorMidtone.SetValue(m_pShaderData->m_ColorMidtone );
	m_Data.m_Transparency.SetValue(m_pShaderData->m_Transparency );
	m_Data.m_UScale.SetValue(m_pShaderData->m_UV.m_UScale );
	m_Data.m_VScale.SetValue(m_pShaderData->m_UV.m_VScale );
	m_Data.m_UTrans.SetValue(m_pShaderData->m_UV.m_UTrans );
	m_Data.m_VTrans.SetValue(m_pShaderData->m_UV.m_VTrans );
	m_Data.m_UVAngle.SetValue(m_pShaderData->m_UV.m_UVAngle );

	m_Data.m_Transition1.SetValue(m_pShaderData->m_Transition1 );
	m_Data.m_Transition2.SetValue(m_pShaderData->m_Transition2 );
	m_Data.m_Transition3.SetValue(m_pShaderData->m_Transition3 );

	m_Data.m_TextureDiffuse.SetValue(itString(m_pShaderData->m_NameDiffuse.c_str()));
	m_Data.m_TextureGradientMap.SetValue(itString(m_pShaderData->m_NameGradientMap.c_str()));
	m_Data.m_TextureTransparencyMap.SetValue(itString(m_pShaderData->m_NameTransparencyMap.c_str()));

	m_Data.m_SpecularEnable.SetValue(m_pShaderData->m_SpecularEnable);
	m_Data.m_Smoothness.SetValue(m_pShaderData->m_Smoothness );

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	AddColorChannel(m_Material.GetMaterialName(), m_Data.m_ColorAmbient);
	AddColorChannel(m_Material.GetMaterialName(), m_Data.m_ColorMidtone);
	AddColorChannel(m_Material.GetMaterialName(), m_Data.m_ColorDiffuse);
	AddColorChannel(m_Material.GetMaterialName(), m_Data.m_ColorSpecular);

	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Transition1);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Transition2);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Transition3);

	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Transparency);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UScale);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VScale);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UTrans);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_VTrans);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_UVAngle);
	AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureDiffuse);
	AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureGradientMap);
	AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_TextureTransparencyMap);

	AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_SpecularEnable );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_Smoothness);

	RegisterData(i_TextureDir);
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlToon::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pDiffuseFileChooser->SetInitialDirectory( i_TextureDir );
	m_pDiffuseFileChooser->UpdateControl();

	m_pGradientFileChooser->SetInitialDirectory( i_TextureDir );
	m_pGradientFileChooser->UpdateControl();

	m_pTransparencyFileChooser->SetInitialDirectory( i_TextureDir );
	m_pTransparencyFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlToon::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_Data.m_TextureDiffuse.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureDiffuse.GetValue()));
	if (m_Data.m_TextureGradientMap.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureGradientMap.GetValue()));
	if (m_Data.m_TextureTransparencyMap.GetValue().GetLength() > 0)
		o_TextureList.push_back(LocateTexture(m_Data.m_TextureTransparencyMap.GetValue()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlToon::RegisterData(const fsLocator& i_TextureDir)
{
	prtyPropertyUIInfo* pPUII;
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ColorAmbient), "Color", "Ambient Color of the object");
	AddProperty( pPUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ColorMidtone), "Color", "Midtone Color of the object");
	AddProperty( pPUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ColorDiffuse), "Color", "Diffuse Color of the object");
	AddProperty( pPUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ColorSpecular), "Color", "Specular Color of the object");
	AddProperty( pPUII );

	m_pDiffuseFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureDiffuse), "Maps", "Diffuse Map");
	m_pDiffuseFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pDiffuseFileChooser );
	m_pGradientFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureGradientMap), "Maps", "Gradient Map (1D)");
	m_pGradientFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pGradientFileChooser );
	m_pTransparencyFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_TextureTransparencyMap), "Maps", "Transparency Map");
	m_pTransparencyFileChooser->SetInitialDirectory( i_TextureDir );
	AddProperty( m_pTransparencyFileChooser );

	prtyRangedFloatUIInfo* pRFUII = NULL;
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
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Transition1), "Color Transition", "Ambient to Diffuse");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Transition2), "Color Transition", "Diffuse to Midtone");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Transition3), "Color Transition", "Midtone to Specular");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Smoothness), "Color Transition", "Smoothness");
	pRFUII->SetMinimum(10.0f);
	pRFUII->SetMaximum(1000.0f);
	AddProperty( pRFUII );

	prtyCheckBoxUIInfo* pCBUII = NULL;
	pCBUII  = new prtyCheckBoxUIInfo(&(m_Data.m_SpecularEnable), "Color", "Specular Enable");
	AddProperty( pCBUII );

	m_Data.m_ColorAmbient.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_ColorMidtone.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_ColorDiffuse.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_ColorSpecular.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_Transparency.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_UScale.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_VScale.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_UTrans.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_VTrans.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_UVAngle.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_Transition1.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_Transition2.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_Transition3.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_TextureDiffuse.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::UpdateTextureDiffuse));
	m_Data.m_TextureGradientMap.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::UpdateTextureGradient));
	m_Data.m_TextureTransparencyMap.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::UpdateTextureTransparency));
	m_Data.m_SpecularEnable.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
	m_Data.m_Smoothness.AddCallback(new prtyCallbackWrapper<mtrlToon>(this, &mtrlToon::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlToon::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effToonData* pData = dynamic_cast<effToonData*>(m_Material.ShaderData());
		DBG_ASSERT0(pData != NULL, "mtrlToon expected effToonData");
		set_shader_data(pData);	// set into material template
	}
	set_shader_data(m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlToon::set_shader_data(effToonData* i_pData)
{
	i_pData->m_ColorAmbient = m_Data.m_ColorAmbient.GetValue();
	i_pData->m_ColorMidtone = m_Data.m_ColorMidtone.GetValue();
	i_pData->m_ColorDiffuse = m_Data.m_ColorDiffuse.GetValue();
	i_pData->m_ColorSpecular = m_Data.m_ColorSpecular.GetValue();
	i_pData->m_Transparency = m_Data.m_Transparency.GetValue();
	i_pData->m_UV.m_UScale = m_Data.m_UScale.GetValue();
	i_pData->m_UV.m_VScale = m_Data.m_VScale.GetValue();
	i_pData->m_UV.m_UTrans = m_Data.m_UTrans.GetValue();
	i_pData->m_UV.m_VTrans = m_Data.m_VTrans.GetValue();
	i_pData->m_UV.m_UVAngle = m_Data.m_UVAngle.GetValue();
	i_pData->m_SpecularEnable = m_Data.m_SpecularEnable.GetValue();
	i_pData->m_Transition1 = m_Data.m_Transition1.GetValue();
	i_pData->m_Transition2 = m_Data.m_Transition2.GetValue();
	i_pData->m_Transition3 = m_Data.m_Transition3.GetValue();
	i_pData->m_Smoothness = m_Data.m_Smoothness.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlToon::UpdateTextureDiffuse(prtyProperty *i_pProperty, bool i_bDirty)
{
	effToonData* pData = dynamic_cast<effToonData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlToon expected effToonData");
	UpdateTexture(m_Data.m_TextureDiffuse, i_bDirty, 
		m_pShaderData->m_NameDiffuse, m_pShaderData->m_TextureDiffuse,
		pData->m_NameDiffuse, pData->m_TextureDiffuse);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlToon::UpdateTextureGradient(prtyProperty *i_pProperty, bool i_bDirty)
{
	effToonData* pData = dynamic_cast<effToonData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlToon expected effToonData");
	UpdateTexture(m_Data.m_TextureGradientMap, i_bDirty,
		m_pShaderData->m_NameGradientMap, m_pShaderData->m_TextureGradientMap,
		pData->m_NameGradientMap, pData->m_TextureGradientMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlToon::UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty)
{
	effToonData* pData = dynamic_cast<effToonData*>(m_Material.ShaderData());
	DBG_ASSERT0(pData != NULL, "mtrlToon expected effToonData");
	UpdateTexture(m_Data.m_TextureTransparencyMap, i_bDirty,
		m_pShaderData->m_NameTransparencyMap, m_pShaderData->m_TextureTransparencyMap,
		pData->m_NameTransparencyMap, pData->m_TextureTransparencyMap);
}
