/*****************************************************************************
**  effDOF.hpp
**
**      effDOF is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_DOF_HPP
#error effDOF.hpp multiply included
#endif
#define GVK_EFF_DOF_HPP

#ifndef GVK_EFF_PLAINSHADERVK_HPP
#include "GraphicsVulkan/Eff/effPlainShaderVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effDOF : public effPlainShaderVk
{
public:
	effDOF(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupParams(const effShaderData* i_Data) const;

protected:
	fxEffectVk::Resource m_hSharpTexture;
	fxEffectVk::Resource m_hBlurryTexture;
	fxEffectVk::Constant m_hSharpResolutionData;
	fxEffectVk::Constant m_hBlurryResolutionData;
	fxEffectVk::Constant m_hMaxCoC;
};
}
