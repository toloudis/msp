/********************************************************************************************\
**  mtrlTextureFilter.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlTextureFilter.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Graphics/eff/effTextureFilterData.hpp"
//#include "Graphics/mat/matShaderEffect.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlTextureFilter::mtrlTextureFilter(mdlMaterialInfo& i_Data, 
				   effTextureFilterData* i_pShaderData,
				   const fsLocator &i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data),
	m_pShaderData(i_pShaderData)
{
	m_Data.m_bEnableMipmap.SetValue(m_pShaderData->m_bEnableMipmap );

	// Create thread-safe proxy as association between the data and the material´s effect data.
	m_pShaderDataProxy.reset( new gpxEffectTextureFilter(*m_pShaderData) );

	RegisterData(i_TextureDir); 
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlTextureFilter::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlTextureFilter::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	// no texture to be returned for this object type
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlTextureFilter::RegisterData(const fsLocator &i_TextureDir)
{
	prtyCheckBoxUIInfo* pCBUII = NULL;
	pCBUII  = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableMipmap), "Texture Filtering", "Enable Mipmapping");
	AddProperty( pCBUII );
	
	//
	m_Data.m_bEnableMipmap.AddCallback(new prtyCallbackWrapper<mtrlTextureFilter>(this, &mtrlTextureFilter::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlTextureFilter::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effTextureFilterData& texture_data = m_Material.TextureFilterParams();
		set_shader_data(texture_data);	// set into material template
		mtrlOperations::SetChunkDataChanged();
		mtrlOperations::ReloadTextures(texture_data.m_bEnableMipmap);
	}

	// use proxy to update material´s shader at a thread-safe time
	m_pShaderDataProxy->SetFilter(m_Data.m_bEnableMipmap.GetValue());
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlTextureFilter::set_shader_data(effTextureFilterData& i_Data)
{
	i_Data.m_bEnableMipmap = m_Data.m_bEnableMipmap.GetValue();
}
