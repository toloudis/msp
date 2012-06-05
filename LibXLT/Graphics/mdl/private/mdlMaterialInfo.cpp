/****************************************************************************\
**	mdlMaterialInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Graphics/Eff/effDisplacementData.hpp"
#include "Graphics/eff/effGlowData.hpp"
#include "Graphics/eff/effNormalsData.hpp"
#include "Graphics/Eff/effOutlineData.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/eff/effRendermanOverrideData.hpp"
#include "Graphics/eff/effTextureFilterData.hpp"
#include "Graphics/g2d/g2dPFD.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlMaterialInfo::mdlMaterialInfo()
:	m_bHasGlow(false), m_bHasOutline(false), m_bOutlineVisible(false),
	m_bHasReflection(false), m_bHasDisplacement(false), m_bHasRendermanOverride(false),
	m_bIsActive(true), m_RefCount(0)
{
	// allocate the base material layer, always there.	
	m_MaterialLayers.resize(1);
	
	// allocate normal map data, always there.
	m_NormalsData.reset(new effNormalsData());
	m_TextureFilterData.reset(new effTextureFilterData());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlMaterialInfo::mdlMaterialInfo(const mdlMaterialInfo& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//--------------------------------------------------------------------
// Assignment operator clones shader data
//--------------------------------------------------------------------
mdlMaterialInfo& mdlMaterialInfo::operator = (const mdlMaterialInfo& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	m_MaterialName = i_CopyFrom.m_MaterialName;
	m_LibraryFilename = i_CopyFrom.m_LibraryFilename;
	m_bIsActive = i_CopyFrom.m_bIsActive;
	m_RefCount = i_CopyFrom.m_RefCount;

	// Copy over each material layer
	const int num_layers = i_CopyFrom.m_MaterialLayers.size();
	m_MaterialLayers.resize(num_layers);
	for (int i=0; i<num_layers; i++)
	{
		// Clone shader data - is this necessary?
		if (i_CopyFrom.m_MaterialLayers[i].m_ShaderParams)
			m_MaterialLayers[i].m_ShaderParams.reset( i_CopyFrom.m_MaterialLayers[i].m_ShaderParams->Clone() );
		else
			m_MaterialLayers[i].m_ShaderParams.reset();

		m_MaterialLayers[i].m_UVTransform = i_CopyFrom.m_MaterialLayers[i].m_UVTransform;
	}

	m_bHasGlow = i_CopyFrom.m_bHasGlow;
	if (i_CopyFrom.m_GlowData)
		m_GlowData.reset( new effGlowData(*i_CopyFrom.m_GlowData) );
	else
		m_GlowData.reset();

	if (i_CopyFrom.m_NormalsData)
		m_NormalsData.reset( new effNormalsData(*i_CopyFrom.m_NormalsData) );
	else
		m_NormalsData.reset();

	if (i_CopyFrom.m_TextureFilterData)
		m_TextureFilterData.reset(new effTextureFilterData(*i_CopyFrom.m_TextureFilterData));
	else
		m_TextureFilterData.reset();

	m_bHasOutline = i_CopyFrom.m_bHasOutline;
	m_bOutlineVisible = i_CopyFrom.m_bOutlineVisible;
	if (i_CopyFrom.m_OutlineData)
		m_OutlineData.reset( new effOutlineData(*i_CopyFrom.m_OutlineData) );
	else
		m_OutlineData.reset();

	m_bHasReflection = i_CopyFrom.m_bHasReflection;
	if (i_CopyFrom.m_ReflectionData)
		m_ReflectionData.reset( new effReflectionMap(*i_CopyFrom.m_ReflectionData) );
	else
		m_ReflectionData.reset();

	m_bHasDisplacement = i_CopyFrom.m_bHasDisplacement;
	if (i_CopyFrom.m_DisplacementData)
		m_DisplacementData.reset( new effDisplacementData(*i_CopyFrom.m_DisplacementData) );
	else 
		m_DisplacementData.reset();

	m_bHasRendermanOverride = i_CopyFrom.m_bHasRendermanOverride;
	if (i_CopyFrom.m_RendermanOverrideData)
		m_RendermanOverrideData.reset( new effRendermanOverrideData(*i_CopyFrom.m_RendermanOverrideData) );
	else 
		m_RendermanOverrideData.reset();

	return *this;
}

//--------------------------------------------------------------------
//	Material Name
//--------------------------------------------------------------------
void mdlMaterialInfo::SetMaterialName(const std::string& i_Name)
{
	m_MaterialName = i_Name;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mdlMaterialInfo::AddMaterialLayer(const shared_ptr<effShaderParams>& i_ShaderParams,
									   effUVTransform& i_UVTransform)
{
	sMaterialLayer layer = {i_ShaderParams, i_UVTransform};
	m_MaterialLayers.push_back(layer);
}

//--------------------------------------------------------------------
//	Shader Info
//--------------------------------------------------------------------
shared_ptr<effShaderParams> mdlMaterialInfo::GetShaderParams(int i_MaterialLayer) const
{
	return m_MaterialLayers[i_MaterialLayer].m_ShaderParams;
}
fsLocator mdlMaterialInfo::GetShader(int i_MaterialLayer) const
{
	if (m_MaterialLayers[i_MaterialLayer].m_ShaderParams)
		return m_MaterialLayers[i_MaterialLayer].m_ShaderParams->GetShaderName();
	else
		return fsLocator();
}
void mdlMaterialInfo::SetShaderParams(shared_ptr<effShaderParams> i_Params, int i_MaterialLayer)
{
	m_MaterialLayers[i_MaterialLayer].m_ShaderParams = i_Params;
}

//--------------------------------------------------------------------
//	UV Transform Info
//--------------------------------------------------------------------
const effUVTransform& mdlMaterialInfo::GetUVTransform(int i_MaterialLayer) const
{
	return m_MaterialLayers[i_MaterialLayer].m_UVTransform;
}
effUVTransform& mdlMaterialInfo::UVTransform(int i_MaterialLayer)
{
	return m_MaterialLayers[i_MaterialLayer].m_UVTransform;
}

//--------------------------------------------------------------------
//	Glow Shader Info
//--------------------------------------------------------------------
bool mdlMaterialInfo::GetHasGlow() const
{
	// Return true if we have glow data and it is enabled
	return (m_bHasGlow && m_GlowData);
}
void mdlMaterialInfo::SetHasGlow(bool i_bHasGlow)
{
	m_bHasGlow = i_bHasGlow;

	// Create the glow parameters if needed, but keep old values
	// if turning glow off (values remain same when glow is turned back on).
	if (i_bHasGlow && !m_GlowData)
		m_GlowData.reset(new effGlowData());
}
const effGlowData& mdlMaterialInfo::GetGlowParams() const
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_GlowData, "Glow Data is NULL, call SetHasGlow(true) first");
	return (*m_GlowData);
}
effGlowData& mdlMaterialInfo::GlowParams()
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_GlowData, "Glow Data is NULL, call SetHasGlow(true) first");
	return (*m_GlowData);
}

//--------------------------------------------------------------------
//	Normal map Shader Info
//--------------------------------------------------------------------
const effNormalsData& mdlMaterialInfo::GetNormalsParams() const
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_NormalsData, "Normals Data is NULL, call SetHasNormals(true) first");
	return (*m_NormalsData);
}
effNormalsData& mdlMaterialInfo::NormalsParams()
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_NormalsData, "Normals Data is NULL, call SetHasNormals(true) first");
	return (*m_NormalsData);
}

//--------------------------------------------------------------------
//	Texture Filter Shader Info
//--------------------------------------------------------------------
const effTextureFilterData& mdlMaterialInfo::GetTextureFilterParams() const
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_TextureFilterData, "TextureFilter Data is NULL, call SetHasTextureFilter(true) first");
	return (*m_TextureFilterData);
}
effTextureFilterData& mdlMaterialInfo::TextureFilterParams()
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_TextureFilterData, "Normals Data is NULL, call SetHasNormals(true) first");
	return (*m_TextureFilterData);
}

//--------------------------------------------------------------------
//	Outline Shader Info
//--------------------------------------------------------------------
bool mdlMaterialInfo::GetHasOutline() const
{
	// Return true if we have outline data and it is enabled
	return (m_bHasOutline && m_OutlineData);
}
void mdlMaterialInfo::SetHasOutline(bool i_bHasOutline)
{
	m_bHasOutline = i_bHasOutline;

	// Create the outline parameters if needed, but keep old values
	// if turning outline off (values remain same when outline is turned back on).
	if (i_bHasOutline && !m_OutlineData)
		m_OutlineData.reset(new effOutlineData());
}
const effOutlineData& mdlMaterialInfo::GetOutlineParams() const
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_OutlineData, "Outline Data is NULL, call SetHasOutline(true) first");
	return (*m_OutlineData);
}
effOutlineData& mdlMaterialInfo::OutlineParams()
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_OutlineData, "Outline Data is NULL, call SetHasOutline(true) first");
	return (*m_OutlineData);
}

bool mdlMaterialInfo::IsOutlineVisible() const
{
	return m_bOutlineVisible;
}

void mdlMaterialInfo::SetOutlineVisible( bool i_bVisible )
{
	m_bOutlineVisible = i_bVisible;
}


//--------------------------------------------------------------------
//	Material Library Info
//--------------------------------------------------------------------
void mdlMaterialInfo::SetLibraryFilename(const fsLocator& i_Locator)
{
	m_LibraryFilename = i_Locator;
}

//--------------------------------------------------------------------
//	Reflection Shader Info
//--------------------------------------------------------------------
bool mdlMaterialInfo::GetHasReflection() const
{
	// Return true if we have glow data and it is enabled
	return (m_bHasReflection && m_ReflectionData);
}
void mdlMaterialInfo::SetHasReflection(bool i_bHasReflection)
{
	m_bHasReflection = i_bHasReflection;

	// Create the glow parameters if needed, but keep old values
	// if turning glow off (values remain same when glow is turned back on).
	if (i_bHasReflection && !m_ReflectionData)
		m_ReflectionData.reset(new effReflectionMap());
}
const effReflectionMap& mdlMaterialInfo::GetReflectionParams() const
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_ReflectionData, "Reflection Data is NULL, call SetHasReflection(true) first");
	return (*m_ReflectionData);
}
effReflectionMap& mdlMaterialInfo::ReflectionParams()
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_ReflectionData, "Reflection Data is NULL, call SetHasReflection(true) first");
	return (*m_ReflectionData);
}

//--------------------------------------------------------------------
//	Displacement Shader Info
//--------------------------------------------------------------------
bool mdlMaterialInfo::GetHasDisplacement() const
{
	// Return true if we have displacement data and it is enabled
	return (m_bHasDisplacement && m_DisplacementData);
}
void mdlMaterialInfo::SetHasDisplacement(bool i_bHasDisplacement)
{
	m_bHasDisplacement = i_bHasDisplacement;

	// Create the displacement parameters if needed, but keep old values
	// if turning displacement off (values remain same when displacement is turned back on).
	if (i_bHasDisplacement && !m_DisplacementData)
		m_DisplacementData.reset(new effDisplacementData());
}
const effDisplacementData& mdlMaterialInfo::GetDisplacementParams() const
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_DisplacementData, "Displacement Data is NULL, call SetHasDisplacement(true) first");
	return (*m_DisplacementData);
}
effDisplacementData& mdlMaterialInfo::DisplacementParams()
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_DisplacementData, "Displacement Data is NULL, call SetHasDisplacement(true) first");
	return (*m_DisplacementData);
}

//--------------------------------------------------------------------
//	RendermanOverride Shader Info
//--------------------------------------------------------------------
bool mdlMaterialInfo::GetHasRendermanOverride() const
{
	// Return true if we have displacement data and it is enabled
	return (m_bHasRendermanOverride && m_RendermanOverrideData);
}
void mdlMaterialInfo::SetHasRendermanOverride(bool i_bHasRendermanOverride)
{
	m_bHasRendermanOverride = i_bHasRendermanOverride;

	// Create the displacement parameters if needed, but keep old values
	// if turning displacement off (values remain same when displacement is turned back on).
	if (i_bHasRendermanOverride && !m_RendermanOverrideData)
		m_RendermanOverrideData.reset(new effRendermanOverrideData());
}
const effRendermanOverrideData& mdlMaterialInfo::GetRendermanOverrideParams() const
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_RendermanOverrideData, "RendermanOverride Data is NULL, call SetHasRendermanOverride(true) first");
	return (*m_RendermanOverrideData);
}
effRendermanOverrideData& mdlMaterialInfo::RendermanOverrideParams()
{
	// This function could also be designed to create the data when needed...
	DBG_ASSERT(m_RendermanOverrideData, "RendermanOverride Data is NULL, call SetHasRendermanOverride(true) first");
	return (*m_RendermanOverrideData);
}

//--------------------------------------------------------------------
//	Set/Get if the material is active
//--------------------------------------------------------------------
void mdlMaterialInfo::SetActive(bool i_bIsActive)
{
	m_bIsActive = i_bIsActive;
	//if (m_bIsActive != i_bIsActive)
	//{
	//	m_bIsActive = i_bIsActive;
	//	
	//	if (i_bIsActive)
	//	{
	//		//reload material texture here
	//	}
	//	else
	//	{
	//		//release material texture here
	//	}
	//}
}

bool mdlMaterialInfo::GetActive()
{
	return m_bIsActive;
}

//--------------------------------------------------------------------
//	In/Decrease material reference count
//--------------------------------------------------------------------
void mdlMaterialInfo::IncreaseRefCount()
{
	m_RefCount++;
}

void mdlMaterialInfo::DecreaseRefCount()
{
	if (m_RefCount > 0)
		m_RefCount--;
}

//--------------------------------------------------------------------
//	Get material reference count
//--------------------------------------------------------------------
int mdlMaterialInfo::GetRefCount()
{
	return m_RefCount;
}