/*****************************************************************************
**  effMaskAlpha.hpp
**
**      effMaskAlpha is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_MASKALPHA_HPP
#error effMaskAlpha.hpp multiply included
#endif
#define GVK_EFF_MASKALPHA_HPP

#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effMaskAlpha : public effShaderBaseVk
{
public:
	//====================================================================
	//====================================================================
	effMaskAlpha(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	//====================================================================
	//====================================================================
	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	//====================================================================
	// Returns false because this effect doesn't need single light passes.
	//====================================================================
	virtual bool DoesLighting() const;

	//====================================================================
	//====================================================================
	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectVariable* m_hSolidColor;
	fxEffectVariable* m_hHasTransparency;
	fxEffectVariable* m_hTransparencyMap;

	fxEffectVariable* m_hUseDither;
	fxEffectVariable* m_hDitherAlphaBias;
	fxEffectVariable* m_hTransparency;
};
}
