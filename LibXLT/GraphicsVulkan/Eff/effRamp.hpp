/*****************************************************************************
**  effRamp.hpp
**
**      effRamp is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_RAMP_HPP
#error effRamp.hpp multiply included
#endif
#define GVK_EFF_RAMP_HPP

#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effRamp : public effShaderBaseVk
{
public:
	//====================================================================
	//====================================================================
	effRamp(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	//====================================================================
	//====================================================================
	virtual ~effRamp();

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
	fxEffectVariable* m_hHasGradient;
	fxEffectVariable* m_hGradientTexture;

	fxEffectVariable* m_hHasNoise;
	fxEffectVariable* m_hNoiseTexture;

	fxEffectVariable* m_hShape;
	fxEffectVariable* m_hInterpolation;
	fxEffectVariable* m_hUWave;
	fxEffectVariable* m_hUWaveFreq;
	fxEffectVariable* m_hVWave;
	fxEffectVariable* m_hVWaveFreq;
	fxEffectVariable* m_hNoise;
	fxEffectVariable* m_hNoiseFreq;
};
}
