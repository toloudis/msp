/*****************************************************************************
**  effPhong.hpp
**
**      effPhong is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_PHONG_HPP
#error effPhong.hpp multiply included
#endif
#define EFF_PHONG_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effPhong : public effShaderBaseDX11
{
public:
	effPhong(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectVariable* m_hHasDiffuse;
	fxEffectVariable* m_hDiffuseMap;
	fxEffectVariable* m_hHasSpecular;
	fxEffectVariable* m_hSpecularMap;
	fxEffectVariable* m_hHasCube;
	fxEffectVariable* m_hCubeMap;
	fxEffectVariable* m_hHasNormal;
	fxEffectVariable* m_hNormalMap;
	fxEffectVariable* m_hHasGloss;
	fxEffectVariable* m_hGlossMap;
	fxEffectVariable* m_hHasReflectFactor;
	fxEffectVariable* m_hReflectFactorMap;
	fxEffectVariable* m_hHasTransparencyMap;
	fxEffectVariable* m_hTransparencyMap;

	fxEffectVariable* m_MaterialEmissiveHandle;
	fxEffectVariable* m_MaterialAmbientHandle;
	fxEffectVariable* m_MaterialDiffuseHandle;
	fxEffectVariable* m_MaterialSpecularHandle;
	
	fxEffectVariable* m_MaterialSpecularPowerHandle;
	fxEffectVariable* m_MaterialBumpMapScaleHandle;
	fxEffectVariable* m_MaterialReflectivityHandle;
	fxEffectVariable* m_MaterialTransparencyHandle;

};
