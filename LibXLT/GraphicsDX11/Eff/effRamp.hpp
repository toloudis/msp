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
	ID3DX11EffectScalarVariable* m_hHasGradient;
	ID3DX11EffectShaderResourceVariable* m_hGradientTexture;

	ID3DX11EffectScalarVariable* m_hHasNoise;
	ID3DX11EffectShaderResourceVariable* m_hNoiseTexture;

	ID3DX11EffectScalarVariable* m_hShape;
	ID3DX11EffectScalarVariable* m_hInterpolation;
	ID3DX11EffectScalarVariable* m_hUWave;
	ID3DX11EffectScalarVariable* m_hUWaveFreq;
	ID3DX11EffectScalarVariable* m_hVWave;
	ID3DX11EffectScalarVariable* m_hVWaveFreq;
	ID3DX11EffectScalarVariable* m_hNoise;
	ID3DX11EffectScalarVariable* m_hNoiseFreq;
};
