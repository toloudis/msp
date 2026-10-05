/*****************************************************************************
**  effRamp.hpp
**
**      effRamp is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_RAMP_HPP
#error effRamp.hpp multiply included
#endif
#define EFF_RAMP_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effRamp : public effShaderBaseDX11
{
public:
	//====================================================================
	//====================================================================
	effRamp(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

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
