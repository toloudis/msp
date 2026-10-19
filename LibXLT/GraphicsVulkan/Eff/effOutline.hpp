/*****************************************************************************
**  effOutline.hpp
**
**      effOutline is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_OUTLINE_HPP
#error effOutline.hpp multiply included
#endif
#define GVK_EFF_OUTLINE_HPP

#ifndef GVK_EFF_PLAINSHADERVK_HPP
#include "GraphicsVulkan/Eff/effPlainShaderVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effOutline : public effPlainShaderVk
{
public:
	effOutline(std::unique_ptr<fxEffectVk> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectVk::Resource m_hTexture;
	fxEffectVk::Constant m_hDepthScale;
	fxEffectVk::Constant m_hMinAngle;
	fxEffectVk::Constant m_hMaxAngle;
	fxEffectVk::Constant m_hThickness;
	fxEffectVk::Constant m_hColor;
	fxEffectVk::Constant m_hViewSize;
	fxEffectVk::Constant m_hMinWidth;
	fxEffectVk::Constant m_hMaxWidth;
};
}
