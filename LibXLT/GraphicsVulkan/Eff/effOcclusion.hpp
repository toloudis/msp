/*****************************************************************************
**  effOcclusion.hpp
**
**      effOcclusion is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_OCCLUSION_HPP
#error effOcclusion.hpp multiply included
#endif
#define GVK_EFF_OCCLUSION_HPP

#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effOcclusion : public effShaderBaseVk
{
public:
	effOcclusion(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;
	virtual void SetupGlowPass(const effGlowData& i_GlowData);

protected:
};
}
