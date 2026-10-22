/*****************************************************************************
**  effParticle.cpp
**
**      effParticle is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_PARTICLE_HPP
#include "GraphicsVulkan/Eff/effParticle.hpp"
#endif

#ifndef EFF_OCCLUSIONDATA_HPP
#include "Graphics/eff/effOcclusionData.hpp"
#endif
#ifndef EFF_PARTICLEDATA_HPP
#include "Graphics/eff/effParticleData.hpp"
#endif

#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif

namespace gvk
{

effParticle::effParticle(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseVk(i_Directory, std::move(i_pEffect), i_name)
{
	MapParameter("diffuseMap", m_hDiffuseMap );
	MapParameter("g_emissive", m_MaterialEmissiveHandle);
	MapParameter("g_ambient", m_MaterialAmbientHandle);
	MapParameter("g_shininess", m_MaterialSpecularPowerHandle);
}

void effParticle::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effParticleData* pData = dynamic_cast<const effParticleData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effParticle");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	m_hDiffuseMap->SetResource(g3dVkTextureUtil::GetImage(pData->m_TextureDiffuse));

    m_MaterialEmissiveHandle->SetFloatVector( pData->m_ColorEmissive.Ptr() );
    m_MaterialAmbientHandle->SetFloatVector( pData->m_ColorAmbient.Ptr() );
	
	m_MaterialSpecularPowerHandle->SetFloat(pData->m_SpecularPower );
}

effShaderData* effParticle::CreateData(const matMaterial* i_Mat)
{
	effParticleData* pData = new effParticleData;
	return pData;
}

}
