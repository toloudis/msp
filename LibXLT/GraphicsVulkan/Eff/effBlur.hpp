/*****************************************************************************
**  effBlur.hpp
**
**      effBlur is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_BLUR_HPP
#error effBlur.hpp multiply included
#endif
#define GVK_EFF_BLUR_HPP

#ifndef GVK_EFF_PLAINSHADERVK_HPP
#include "GraphicsVulkan/Eff/effPlainShaderVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effBlur : public effPlainShaderVk
{
public:
	effBlur(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupParams(const effShaderData* i_Data) const;

protected:
	fxEffectVk::Resource m_hSceneTexture;
	fxEffectVk::Resource m_hDownsampledTexture;
	fxEffectVk::Resource m_hHorizontalBlurTexture;

	fxEffectVk::Constant m_hSrcSize;
	fxEffectVk::Constant m_hDownsampledSize;
};
}
