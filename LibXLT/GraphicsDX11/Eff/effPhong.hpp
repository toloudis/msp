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
	effPhong(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	ID3DX11EffectScalarVariable* m_hHasDiffuse;
	ID3DX11EffectShaderResourceVariable* m_hDiffuseMap;
	ID3DX11EffectScalarVariable* m_hHasSpecular;
	ID3DX11EffectShaderResourceVariable* m_hSpecularMap;
	ID3DX11EffectScalarVariable* m_hHasCube;
	ID3DX11EffectShaderResourceVariable* m_hCubeMap;
	ID3DX11EffectScalarVariable* m_hHasNormal;
	ID3DX11EffectShaderResourceVariable* m_hNormalMap;
	ID3DX11EffectScalarVariable* m_hHasGloss;
	ID3DX11EffectShaderResourceVariable* m_hGlossMap;
	ID3DX11EffectScalarVariable* m_hHasReflectFactor;
	ID3DX11EffectShaderResourceVariable* m_hReflectFactorMap;
	ID3DX11EffectScalarVariable* m_hHasTransparencyMap;
	ID3DX11EffectShaderResourceVariable* m_hTransparencyMap;

	ID3DX11EffectVectorVariable* m_MaterialEmissiveHandle;
	ID3DX11EffectVectorVariable* m_MaterialAmbientHandle;
	ID3DX11EffectVectorVariable* m_MaterialDiffuseHandle;
	ID3DX11EffectVectorVariable* m_MaterialSpecularHandle;
	
	ID3DX11EffectScalarVariable* m_MaterialSpecularPowerHandle;
	ID3DX11EffectScalarVariable* m_MaterialBumpMapScaleHandle;
	ID3DX11EffectScalarVariable* m_MaterialReflectivityHandle;
	ID3DX11EffectScalarVariable* m_MaterialTransparencyHandle;

};
