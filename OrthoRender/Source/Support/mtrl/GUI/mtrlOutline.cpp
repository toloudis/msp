/********************************************************************************************\
**  mtrlOutline.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlOutline.hpp"

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
#include "Graphics/eff/effOutlineData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlOutline::mtrlOutline(mdlMaterialInfo& i_Data, 
				   effOutlineData* i_pShaderData,
				   const fsLocator &i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT0(m_pShaderData != NULL, "mtrlOutline expected effOutlineData");

	m_Data.m_OutlineDepthScale.SetValue(m_pShaderData->m_OutlineDepthScale);
	m_Data.m_OutlineThreshold.SetValue(m_pShaderData->m_OutlineThreshold);
	m_Data.m_OutlineThickness.SetValue(m_pShaderData->m_OutlineThickness);
	m_Data.m_OutlineColor.SetValue(m_pShaderData->m_OutlineColor);

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).

	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineDepthScale );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineThreshold );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineThickness );
	AddColorChannel(m_Material.GetMaterialName(), m_Data.m_OutlineColor );
	AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_bUseDepths );
	AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_bUseNormals );

	RegisterData(i_TextureDir); 
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlOutline::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pOutlineFileChooser->SetInitialDirectory( i_TextureDir );
	m_pOutlineFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlOutline::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlOutline::RegisterData(const fsLocator &i_TextureDir)
{
	prtyRangedFloatUIInfo* pRFUII = NULL;

	AddProperty(new prtyCheckBoxUIInfo( &(m_Data.m_bUseDepths), "Outline", "Use Depths" ));

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineDepthScale), "Outline", "    Depth Scale");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(.1f);
	pRFUII->SetDecimalPlaces( 5 );
	AddProperty( pRFUII );

	AddProperty(new prtyCheckBoxUIInfo( &(m_Data.m_bUseNormals), "Outline", "Use Normals" ));

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineThreshold), "Outline", "    Threshold");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(4.0f);
	pRFUII->SetDecimalPlaces( 3 );
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineThickness), "Outline", "Thickness");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(5.0f);
	AddProperty( pRFUII );

	AddProperty(new prtyColorRGBAEditUIInfo(&(m_Data.m_OutlineColor), "Outline", "Color" ));

	//
	m_Data.m_OutlineDepthScale.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineThreshold.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineThickness.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineColor.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_bUseDepths.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_bUseNormals.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlOutline::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effOutlineData& outline_data = m_Material.OutlineParams();
		set_shader_data(outline_data);	// set into material template
	}
	set_shader_data(*m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlOutline::set_shader_data(effOutlineData& i_Data)
{
	i_Data.m_OutlineDepthScale = m_Data.m_OutlineDepthScale.GetValue();
	i_Data.m_OutlineThreshold = m_Data.m_OutlineThreshold.GetValue();
	i_Data.m_OutlineThickness = m_Data.m_OutlineThickness.GetValue();
	i_Data.m_OutlineColor = m_Data.m_OutlineColor.GetValue();
	i_Data.m_bUseDepths = m_Data.m_bUseDepths.GetValue();
	i_Data.m_bUseNormals = m_Data.m_bUseNormals.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlOutline::UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty)
{
	effOutlineData& outline_data = m_Material.OutlineParams();
}

