/*****************************************************************************
**  effParticle.hpp
**
**      effParticle is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_PARTICLE_HPP
#error effParticle.hpp multiply included
#endif
#define EFF_PARTICLE_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effParticle : public effShaderBaseDX11
{
public:
	effParticle(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectVariable* m_hDiffuseMap;
	fxEffectVariable* m_MaterialAmbientHandle;
	fxEffectVariable* m_MaterialEmissiveHandle;
	fxEffectVariable* m_MaterialSpecularPowerHandle;
};
