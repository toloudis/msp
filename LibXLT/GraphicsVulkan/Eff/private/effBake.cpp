/*****************************************************************************
**  effBake.cpp
**
**      effBake is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_BAKE_HPP
#include "GraphicsVulkan/Eff/effBake.hpp"
#endif

#ifndef EFF_BAKEDATA_HPP
#include "Graphics/eff/effBakeData.hpp"
#endif

#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif

namespace gvk
{

effBake::effBake(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseVk(i_Directory, std::move(i_pEffect), i_name)
{
	MapParameter("hasEmissiveMap", m_hHasEmissive);
	MapParameter("emissiveMap", m_hEmissiveMap);
	MapParameter("hasNormalMap", m_hHasNormal );
	MapParameter("normalMap", m_hNormalMap );
	MapParameter("g_emissive", m_MaterialEmissiveHandle);
}

void effBake::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effBakeData* pData = dynamic_cast<const effBakeData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effBake");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	if (g3dSingleLightRendering::GetDoBaking())
	{
		float comb_emissive[4] = {0,0,0,0};
		m_MaterialEmissiveHandle->SetFloatVector(comb_emissive);
		m_hHasEmissive->AsScalar()->SetBool( (pData->m_TextureEmissive != NULL) ? true : false);
		m_hEmissiveMap->AsShaderResource()->SetResource( g3dVkTextureUtil::GetImage(pData->m_TextureEmissive));
	}
	else
	{
		m_MaterialEmissiveHandle->SetFloatVector( pData->m_ColorEmissive.Ptr() );
		m_hHasEmissive->AsScalar()->SetBool( (pData->m_TextureEmissive != NULL) ? true : false);
		m_hEmissiveMap->AsShaderResource()->SetResource( g3dVkTextureUtil::GetImage(pData->m_TextureEmissive));
	}

	m_hHasNormal->AsScalar()->SetBool((pData->m_TextureNormalMap != NULL) ? true : false);
	m_hNormalMap->AsShaderResource()->SetResource( g3dVkTextureUtil::GetImage(pData->m_TextureNormalMap));
	
	effShaderBaseVk::SetupMaterial( i_Material, i_MaterialLayerIndex );
}

effShaderData* effBake::CreateData(const matMaterial* i_Mat)
{
	effBakeData* pData = new effBakeData;
	return pData;
}

}
