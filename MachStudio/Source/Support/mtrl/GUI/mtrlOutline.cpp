/********************************************************************************************\
**  mtrlOutline.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlOutline.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
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
	DBG_ASSERT(m_pShaderData != NULL, "mtrlOutline expected effOutlineData");

	// Create thread-safe proxy for the material´s effect data.
	m_pShaderDataProxy.reset( new gpxEffectOutline(*m_pShaderData) );

	m_Data.m_OutlineDepthScale.SetValue(m_pShaderData->m_OutlineDepthScale);
	m_Data.m_OutlineMinAngle.SetValue(m_pShaderData->m_OutlineMinAngle);
	m_Data.m_OutlineMaxAngle.SetValue(m_pShaderData->m_OutlineMaxAngle);
	m_Data.m_OutlineThickness.SetValue(m_pShaderData->m_OutlineThickness);
	m_Data.m_OutlineMinWidth.SetValue(m_pShaderData->m_OutlineMinWidth);
	m_Data.m_OutlineMaxWidth.SetValue(m_pShaderData->m_OutlineMaxWidth);
	m_Data.m_OutlineColor.SetValue(m_pShaderData->m_OutlineColor);
	m_Data.m_bUseDepths.SetValue(m_pShaderData->m_bUseDepths);
	m_Data.m_bUseNormals.SetValue(m_pShaderData->m_bUseNormals);

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).

	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineDepthScale );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineMinAngle );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineMaxAngle );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineThickness );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineMinWidth );
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_OutlineMaxWidth );
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

	//m_pOutlineFileChooser->SetInitialDirectory( i_TextureDir );
	//m_pOutlineFileChooser->UpdateControl();
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

	AddProperty(new prtyCheckBoxUIInfo( &(m_Data.m_bUseDepths), "Outline", "Enable Silhouette" ));

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineDepthScale), "Outline", "Depth Scale");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces( 5 );
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineThickness), "Outline", "Thickness");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(5.0f);
	AddProperty( pRFUII );

	AddProperty(new prtyCheckBoxUIInfo( &(m_Data.m_bUseNormals), "Outline", "Enable Interior" ));

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineMinAngle), "Outline", "Min Angle");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces( 2 );
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineMaxAngle), "Outline", "Max Angle");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces( 2 );
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineMinWidth), "Outline", "Min Width");
	pRFUII->SetMinimum(-10.0f);
	pRFUII->SetMaximum(10.0f);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_OutlineMaxWidth), "Outline", "Max Width");
	pRFUII->SetMinimum(-10.0f);
	pRFUII->SetMaximum(10.0f);
	AddProperty( pRFUII );

	AddProperty(new prtyColorRGBAEditUIInfo(&(m_Data.m_OutlineColor), "Outline", "Color" ));

	//
	m_Data.m_OutlineDepthScale.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineMinAngle.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineMaxAngle.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineThickness.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineMinWidth.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
	m_Data.m_OutlineMaxWidth.AddCallback(new prtyCallbackWrapper<mtrlOutline>(this, &mtrlOutline::Update));
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
		mtrlOperations::SetChunkDataChanged();
	}

	// use proxy to update material´s shader at a thread-safe time
	m_pShaderDataProxy->SetOutlineDepthScale( m_Data.m_OutlineDepthScale.GetValue() );
	m_pShaderDataProxy->SetOutlineMinAngle( m_Data.m_OutlineMinAngle.GetValue() );
	m_pShaderDataProxy->SetOutlineMaxAngle( m_Data.m_OutlineMaxAngle.GetValue() );
	m_pShaderDataProxy->SetOutlineThickness( m_Data.m_OutlineThickness.GetValue() );
	m_pShaderDataProxy->SetOutlineMinWidth( m_Data.m_OutlineMinWidth.GetValue() );
	m_pShaderDataProxy->SetOutlineMaxWidth( m_Data.m_OutlineMaxWidth.GetValue() );
	m_pShaderDataProxy->SetOutlineColor( m_Data.m_OutlineColor.GetValue() );
	m_pShaderDataProxy->SetUseDepths( m_Data.m_bUseDepths.GetValue() );
	m_pShaderDataProxy->SetUseNormals( m_Data.m_bUseNormals.GetValue() );

}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlOutline::set_shader_data(effOutlineData& i_Data)
{
	i_Data.m_OutlineDepthScale = m_Data.m_OutlineDepthScale.GetValue();
	i_Data.m_OutlineMinAngle = m_Data.m_OutlineMinAngle.GetValue();
	i_Data.m_OutlineMaxAngle = m_Data.m_OutlineMaxAngle.GetValue();
	i_Data.m_OutlineThickness = m_Data.m_OutlineThickness.GetValue();
	i_Data.m_OutlineMinWidth = m_Data.m_OutlineMinWidth.GetValue();
	i_Data.m_OutlineMaxWidth = m_Data.m_OutlineMaxWidth.GetValue();
	i_Data.m_OutlineColor = m_Data.m_OutlineColor.GetValue();
	i_Data.m_bUseDepths = m_Data.m_bUseDepths.GetValue();
	i_Data.m_bUseNormals = m_Data.m_bUseNormals.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlOutline::UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty)
{
	effOutlineData& outline_data = m_Material.OutlineParams();
	if (i_bDirty)
		mtrlOperations::SetChunkDataChanged();
}

