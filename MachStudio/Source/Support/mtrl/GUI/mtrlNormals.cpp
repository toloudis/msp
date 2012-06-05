/********************************************************************************************\
**  mtrlNormals.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlNormals.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"

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
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/eff/effNormalsData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlNormals::mtrlNormals(mdlMaterialInfo& i_Data, 
				   effNormalsData* i_pShaderData,
				   const fsLocator &i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT(m_pShaderData != NULL, "mtrlNormals expected effNormalsData");

	// Create thread-safe proxy for the material´s effect data.
	m_pShaderDataProxy.reset( new gpxEffectNormals(*m_pShaderData) );

	m_Data.m_BumpScale.SetValue(m_pShaderData->m_BumpScale);

	prtyTextureFileData tex;
	tex.m_TextureLocator = m_pShaderData->m_NameNormalMap;
	tex.m_CurrentCallback = m_Data.m_NormalMap.GetFullValue().m_CurrentCallback;  //preserve the current callback

	m_Data.m_NormalMap.SetValue(tex);
	m_Data.m_NormalMap.SetRevertValue(tex);

	//m_Data.m_BumpScale.SetValue(i_Data.GetNormalsParams().m_BumpScale);
	//m_Data.m_NormalMap.SetValue(i_Data.GetNormalsParams().m_NameNormalMap);

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_BumpScale);
	this->AddTextureChannel(m_Material.GetMaterialName(), m_Data.m_NormalMap);

	RegisterData(i_TextureDir); 
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlNormals::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	m_pNormalsFileChooser->SetInitialDirectory( i_TextureDir );
	m_pNormalsFileChooser->UpdateControl();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlNormals::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_Data.m_NormalMap.GetValue().GetNumNames() > 1)
		o_TextureList.push_back(m_Data.m_NormalMap.GetValue()); // fullpath
	else if (m_Data.m_NormalMap.GetValue().GetNumNames() == 1)
		o_TextureList.push_back(LocateTexture(m_Data.m_NormalMap.GetValue().GetLastName())); // filename needs locating
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlNormals::RegisterData(const fsLocator &i_TextureDir)
{
	m_pNormalsFileChooser = new prtyTextureFileChooserUIInfo(&(m_Data.m_NormalMap), "Normal Map", "Normal Map");
	m_pNormalsFileChooser->SetInitialDirectory( i_TextureDir );
	m_pNormalsFileChooser->SetDirectoryCategory("Textures");
	AddProperty( m_pNormalsFileChooser );

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_BumpScale), "Normal Map", "Bump Scale");
	pRFUII->SetMinimum(-20.0f);
	pRFUII->SetMaximum(20.0f);
	AddProperty( pRFUII );

	m_Data.m_BumpScale.AddCallback(new prtyCallbackWrapper<mtrlNormals>(this, &mtrlNormals::Update));
	m_Data.m_NormalMap.AddCallback(new prtyCallbackWrapper<mtrlNormals>(this, &mtrlNormals::UpdateMaskTexture));
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlNormals::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effNormalsData& normals_data = m_Material.NormalsParams();
		set_shader_data(normals_data);	// set into material template
		mtrlOperations::SetChunkDataChanged();
	}
	
	// use proxy to update material´s shader at a thread-safe time
	m_pShaderDataProxy->SetBumpScale( m_Data.m_BumpScale.GetValue() );
	
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlNormals::set_shader_data(effNormalsData& i_Data)
{
	i_Data.m_BumpScale = m_Data.m_BumpScale.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlNormals::UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty)
{
	effNormalsData& normals_data = m_Material.NormalsParams();
	UpdateTexture(m_Data.m_NormalMap, i_bDirty,
		m_pShaderData->m_NameNormalMap, m_pShaderData->m_pNormalMap,
		normals_data.m_NameNormalMap, normals_data.m_pNormalMap);
	if (i_bDirty)
		mtrlOperations::SetChunkDataChanged();
}