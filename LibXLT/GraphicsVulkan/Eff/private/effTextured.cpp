/*****************************************************************************
**  effTextured.cpp
**
**      effTextured is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_TEXTURED_HPP
#include "GraphicsVulkan/Eff/effTextured.hpp"
#endif

#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/eff/effTexturedData.hpp"
#endif

#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

namespace gvk
{

//====================================================================
//====================================================================
effTextured::effTextured(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseVk(i_Directory, std::move(i_pEffect), i_name)
{
	MapParameter("hasDiffuseMap", m_hHasTexture );
	MapParameter("diffuseMap", m_hTexture );
	MapParameter("g_diffuse", m_hColor );
}

//====================================================================
// Returns false because this effect doesn't need single light passes.
//====================================================================
bool effTextured::DoesLighting() const
{
	return false;
}

//====================================================================
//====================================================================
void effTextured::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effTexturedData* pData = dynamic_cast<const effTexturedData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effTextured");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	if (m_hHasTexture)
	{
		m_hHasTexture->SetBool(pData->m_Texture?true:false);
	}
	m_hTexture->SetResource(g3dVkTextureUtil::GetImage(pData->m_Texture));
	if (m_hColor)
	{
		m_hColor->SetFloatVector( pData->m_Color.Ptr() );
	}
		
}

//====================================================================
//====================================================================
effShaderData* effTextured::CreateData(const matMaterial* i_Mat)
{
	effTexturedData* pData = new effTexturedData;
	return pData;
}

}
