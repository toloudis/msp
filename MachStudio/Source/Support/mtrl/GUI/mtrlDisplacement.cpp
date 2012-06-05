/********************************************************************************************\
**  mtrlDisplacement.cpp
**
**
**  StudioGPU
**  Copyright(C) 2009 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlDisplacement.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Graphics/eff/effDisplacementData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/smdl/private/smdlTessellatorBase.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlDisplacement::mtrlDisplacement(mdlMaterialInfo& i_Data, 
				   effDisplacementData* i_pShaderData,
				   const fsLocator& i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT(m_pShaderData != NULL, "mtrlDisplacement expected effDisplacementData");

	// Create thread-safe proxy for the material´s effect data.
	m_pShaderDataProxy.reset( new gpxEffectDisplacement(*m_pShaderData) );

	prtyTextureFileData tex;
	tex.m_TextureLocator = m_pShaderData->m_NameDisplacementMap;
	tex.m_CurrentCallback = m_Data.m_DisplacementMap.GetFullValue().m_CurrentCallback;  //preserve the current callback

	m_Data.m_DisplacementMap.SetValue(tex);
	m_Data.m_DisplacementMap.SetRevertValue(tex);
	
	
	m_Data.m_DisplacementScale.SetValue(m_pShaderData->m_Scale);
	m_Data.m_DisplacementBias.SetValue(m_pShaderData->m_Bias);
	m_Data.m_DisplacementBlur.SetValue(m_pShaderData->m_Blur);
	m_Data.m_TessellationValue.SetValue(m_pShaderData->m_TessellationValue);
	float size = m_pShaderData->m_ObjUVScale.GetX();
//	float aspect = m_pShaderData->m_ObjUVScale.GetY() / size;
	m_Data.m_ObjTextureSize.SetValue( size );
//	m_Data.m_ObjTextureAspect.SetValue( aspect );

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).

	AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_DisplacementMap);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_DisplacementScale);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_DisplacementBias);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_DisplacementBlur);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_TessellationValue);
	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_ObjTextureSize);
//	AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_ObjTextureAspect);

	RegisterData(i_TextureDir);
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlDisplacement::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pDisplacementFileChooser->SetInitialDirectory( i_TextureDir );
	m_pDisplacementFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlDisplacement::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_Data.m_DisplacementMap.GetValue().GetNumNames() > 1)
		o_TextureList.push_back(m_Data.m_DisplacementMap.GetValue()); // fullpath
	else if (m_Data.m_DisplacementMap.GetValue().GetNumNames() == 1)
		o_TextureList.push_back(LocateTexture(m_Data.m_DisplacementMap.GetValue().GetLastName())); // filename needs locating
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlDisplacement::RegisterData(const fsLocator& i_TextureDir)
{
	prtyRangedFloatUIInfo* pRFUII = NULL;

	m_pDisplacementFileChooser = new prtyTextureFileChooserUIInfo(&(m_Data.m_DisplacementMap), "Displacement", "Displacement Map");
	m_pDisplacementFileChooser->SetDirectoryCategory("Textures");
	AddProperty( m_pDisplacementFileChooser );

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_DisplacementScale), "Displacement", "Scale" );
	pRFUII->SetMinimum(-1.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_DisplacementBias), "Displacement", "Bias" );
	pRFUII->SetMinimum(-1.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_DisplacementBlur), "Displacement", "Smoothness" );
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_TessellationValue), "Displacement", "Tessellation Amount" );
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(12.0f);
	AddProperty( pRFUII );

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_ObjTextureSize), "Displacement", "Object UV Size" );
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	AddProperty( pRFUII );
/*
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_ObjTextureAspect), "Displacement", "Object UV Aspect" );
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	AddProperty( pRFUII );
*/

	m_Data.m_DisplacementMap.AddCallback(new prtyCallbackWrapper<mtrlDisplacement>(this, &mtrlDisplacement::UpdateDisplacementMap));
	m_Data.m_DisplacementScale.AddCallback(new prtyCallbackWrapper<mtrlDisplacement>(this, &mtrlDisplacement::Update));
	m_Data.m_DisplacementBias.AddCallback(new prtyCallbackWrapper<mtrlDisplacement>(this, &mtrlDisplacement::Update));
	m_Data.m_DisplacementBlur.AddCallback(new prtyCallbackWrapper<mtrlDisplacement>(this, &mtrlDisplacement::Update));
	m_Data.m_TessellationValue.AddCallback(new prtyCallbackWrapper<mtrlDisplacement>(this, &mtrlDisplacement::Update));
	m_Data.m_ObjTextureSize.AddCallback(new prtyCallbackWrapper<mtrlDisplacement>(this, &mtrlDisplacement::Update));
//	m_Data.m_ObjTextureAspect.AddCallback(new prtyCallbackWrapper<mtrlDisplacement>(this, &mtrlDisplacement::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlDisplacement::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
#if(SGPU_APP != MS_CORE)
	if (i_bDirty)
	{
		effDisplacementData& Data = m_Material.DisplacementParams();
		set_shader_data(Data);	// set into material template
		mtrlOperations::SetChunkDataChanged();
	}

	// use proxy to update material´s shader at a thread-safe time
	m_pShaderDataProxy->SetScale( m_Data.m_DisplacementScale.GetValue() );
	m_pShaderDataProxy->SetBias( m_Data.m_DisplacementBias.GetValue() );
	m_pShaderDataProxy->SetBlur( m_Data.m_DisplacementBlur.GetValue() );
	m_pShaderDataProxy->SetTessellationValue( m_Data.m_TessellationValue.GetValue() );
	float size = m_Data.m_ObjTextureSize.GetValue();
	m_pShaderDataProxy->SetObjUVScale( maVector2d( size, size ) );
#endif
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlDisplacement::set_shader_data(effDisplacementData& i_Data)
{
	i_Data.m_Scale = m_Data.m_DisplacementScale.GetValue();
	i_Data.m_Bias = m_Data.m_DisplacementBias.GetValue();
	i_Data.m_Blur = m_Data.m_DisplacementBlur.GetValue();
	i_Data.m_TessellationValue = m_Data.m_TessellationValue.GetValue();
	float size = m_Data.m_ObjTextureSize.GetValue();
//	float aspect = m_Data.m_ObjTextureAspect.GetValue();
	i_Data.m_ObjUVScale = maVector2d( size, size );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlDisplacement::UpdateDisplacementMap(prtyProperty *i_pProperty, bool i_bDirty)
{
#if(SGPU_APP != MS_CORE)
	effDisplacementData& disp_data = m_Material.DisplacementParams();
	UpdateTexture(m_Data.m_DisplacementMap, i_bDirty,
		m_pShaderData->m_NameDisplacementMap, m_pShaderData->m_pDisplacementMap,
		disp_data.m_NameDisplacementMap, disp_data.m_pDisplacementMap);
	if (i_bDirty)
		mtrlOperations::SetChunkDataChanged();
#endif
}

