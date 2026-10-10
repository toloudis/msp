/*****************************************************************************
**  effOcclusion.cpp
**
**      effOcclusion is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effOcclusion.hpp"

#include "Graphics/eff/effOcclusionData.hpp"

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"

effOcclusion::effOcclusion(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, std::move(i_pEffect), i_name)
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


