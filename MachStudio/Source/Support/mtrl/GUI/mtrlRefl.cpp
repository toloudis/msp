/********************************************************************************************\
**  mtrlRefl.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlRefl.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Tool/gpx/gpxEffectReflection.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlRefl::mtrlRefl(mdlMaterialInfo& i_Data, 
				   effReflectionMap* i_pShaderData,
				   const fsLocator& i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT(m_pShaderData != NULL, "mtrlRefl expected effReflData");

	m_Data.m_ReflMapResolution.SetValue(m_pShaderData->m_ReflMapResolution);
	m_Data.m_bAutoGenEnvMap.SetValue(m_pShaderData->m_bAutoGenEnvMap);
	m_Data.m_NearPlane.SetValue(m_pShaderData->m_NearPlane);
	m_Data.m_bIsPlanar.SetValue(m_pShaderData->m_bIsPlanar);

	// Create the proxy
	m_pShaderDataProxy.reset( new gpxEffectReflection(*m_pShaderData) );

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_NearPlane);

	// Do not populate UI if the shader isnt made to support this
	if (i_Data.GetShaderParams() != NULL)
	{
		matShaderEffect* eff = i_Data.GetShaderParams()->GetShader();
		if (eff != NULL)
		{
			if ( eff->HasReflectionMap() )
			{
				RegisterData(i_TextureDir);
			}
		}
	}
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlRefl::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlRefl::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlRefl::RegisterData(const fsLocator& i_TextureDir)
{
	prtyCheckBoxUIInfo* pPUII  = new prtyCheckBoxUIInfo(&(m_Data.m_bAutoGenEnvMap), "Reflection", "Generate reflection map at object position");
	AddProperty( pPUII );

	prtyComboBoxUIInfo* pCBUII;
	pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_ReflMapResolution), "Reflection", "Reflection Map Size");
	pCBUII->AddItem(std::string("64"), 0);
	pCBUII->AddItem(std::string("128"), 1);
	pCBUII->AddItem(std::string("256"), 2);
	pCBUII->AddItem(std::string("512"), 3);
	pCBUII->AddItem(std::string("1024"), 4);
	pCBUII->AddItem(std::string("2048"), 5);
	AddProperty( pCBUII );

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_NearPlane), "Reflection", "Near Plane distance");
	pRFUII->SetMinimum(0.1f);
	pRFUII->SetMaximum(100.0f);
	AddProperty( pRFUII );

	pPUII  = new prtyCheckBoxUIInfo(&(m_Data.m_bIsPlanar), "Reflection", "Auto-adjust normals for planar reflection");
	AddProperty( pPUII );

	m_Data.m_bAutoGenEnvMap.AddCallback(new prtyCallbackWrapper<mtrlRefl>(this, &mtrlRefl::UpdateReflectionMap));
	m_Data.m_ReflMapResolution.AddCallback(new prtyCallbackWrapper<mtrlRefl>(this, &mtrlRefl::UpdateReflectionMap));
	m_Data.m_bIsPlanar.AddCallback(new prtyCallbackWrapper<mtrlRefl>(this, &mtrlRefl::UpdateReflectionMap));
	m_Data.m_NearPlane.AddCallback(new prtyCallbackWrapper<mtrlRefl>(this, &mtrlRefl::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlRefl::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effReflectionMap* pData = &m_Material.ReflectionParams();
		// set into material template
		pData->m_NearPlane = m_Data.m_NearPlane.GetValue();
		mtrlOperations::SetChunkDataChanged();
	}

	// set into material's shader data through proxy
	m_pShaderDataProxy->SetNearPlane( m_Data.m_NearPlane.GetValue() );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlRefl::UpdateReflectionMap(prtyProperty *i_pProperty, bool i_bDirty)
{
	// stop any render threads for texture manager changes
	gpxRenderControl::ConfirmSingleThread();

	if (i_bDirty)
	{
		effReflectionMap* pData = &m_Material.ReflectionParams();
		pData->m_bAutoGenEnvMap = m_Data.m_bAutoGenEnvMap.GetValue();
		pData->m_ReflMapResolution = m_Data.m_ReflMapResolution.GetValue();
		pData->m_bIsPlanar = m_Data.m_bIsPlanar.GetValue();
		mtrlOperations::SetChunkDataChanged();
	}

	// These changes cannot be proxied because of the need to update the
	// render target.
	m_pShaderData->m_bAutoGenEnvMap = m_Data.m_bAutoGenEnvMap.GetValue();
	m_pShaderData->m_ReflMapResolution = m_Data.m_ReflMapResolution.GetValue();
	m_pShaderData->m_bIsPlanar = m_Data.m_bIsPlanar.GetValue();

	// this will propagate the data fields into the underlying matMaterial
	mtrlOperations::UpdateTargetRenderer();
}

