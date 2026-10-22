/*****************************************************************************
**  effOcclusion.cpp
**
**      effOcclusion is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_OCCLUSION_HPP
#include "GraphicsVulkan/Eff/effOcclusion.hpp"
#endif

#ifndef EFF_OCCLUSIONDATA_HPP
#include "Graphics/eff/effOcclusionData.hpp"
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

namespace gvk
{

effOcclusion::effOcclusion(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseVk(i_Directory, std::move(i_pEffect), i_name)
{
}

void effOcclusion::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
//	const effOcclusionData* pData = dynamic_cast<const effOcclusionData*>(i_Material->GetEffectData());
//	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effOcclusion");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

}

void effOcclusion::SetupGlowPass(const effGlowData& i_GlowData)
{
}

effShaderData* effOcclusion::CreateData(const matMaterial* i_Mat)
{
	effOcclusionData* pData = new effOcclusionData;
	return pData;
}

}
