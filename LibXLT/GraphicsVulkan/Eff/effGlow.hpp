/*****************************************************************************
**  effGlow.hpp
**
**      effGlow is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_GLOW_HPP
#error effGlow.hpp multiply included
#endif
#define GVK_EFF_GLOW_HPP

#ifndef GVK_EFF_PLAINSHADERVK_HPP
#include "GraphicsVulkan/Eff/effPlainShaderVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effGlow : public effPlainShaderVk
{
public:
	effGlow(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectVk::Resource m_hTexture;
	fxEffectVk::Constant m_hGlowAmount;
	fxEffectVk::Constant m_hGlowScale;
	fxEffectVk::Constant m_hGlowSize;
	fxEffectVk::Constant m_hConstantGlow;
	fxEffectVk::Constant m_hSrcSizeInfo;
};
}
