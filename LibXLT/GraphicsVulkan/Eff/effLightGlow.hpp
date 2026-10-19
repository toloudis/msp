/*****************************************************************************
**  effLightGlow.hpp
**
**      effLightGlow is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_LIGHTGLOW_HPP
#error effLightGlow.hpp multiply included
#endif
#define GVK_EFF_LIGHTGLOW_HPP

#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effLightGlow : public effShaderBaseVk 
{
public:
	effLightGlow(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

	//====================================================================
	// Returns false because this effect doesn't need single light passes.
	//====================================================================
	virtual bool DoesLighting() const;

protected:
	fxEffectVariable* m_hHasDiffuseMap;
	fxEffectVariable* m_hDiffuseMap;

	fxEffectVariable* m_hEdgeFuzzCutoff;
	fxEffectVariable* m_hDistFalloffStart;
	fxEffectVariable* m_hDistFalloffEnd;
	fxEffectVariable* m_hGlowAlpha;
	fxEffectVariable* m_hUseShadow;
};
}
