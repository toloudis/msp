/*****************************************************************************
**  effParticle.cpp
**
**      effParticle is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effParticle.hpp"

#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effParticleData.hpp"

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"

effParticle::effParticle(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, std::move(i_pEffect), i_name)
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

	m_hDiffuseMap->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureDiffuse));

    m_MaterialEmissiveHandle->SetFloatVector( pData->m_ColorEmissive.Ptr() );
    m_MaterialAmbientHandle->SetFloatVector( pData->m_ColorAmbient.Ptr() );
	
	m_MaterialSpecularPowerHandle->SetFloat(pData->m_SpecularPower );
}

effShaderData* effParticle::CreateData(const matMaterial* i_Mat)
{
	effParticleData* pData = new effParticleData;
	return pData;
}


