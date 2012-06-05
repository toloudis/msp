/*****************************************************************************
**  effBake.hpp
**
**      effBake is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_BAKE_HPP
#error effBake.hpp multiply included
#endif
#define EFF_BAKE_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effBake : public effShaderBaseDX11
{
public:
	effBake(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

	virtual maFloatRGBA GetDiffuse() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetAmbient() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetSpecular() const {return maFloatRGBA(0,0,0,0);}


protected:
	ID3DX11EffectScalarVariable* m_hHasEmissive;
	ID3DX11EffectShaderResourceVariable* m_hEmissiveMap;
	ID3DX11EffectScalarVariable* m_hHasNormal;
	ID3DX11EffectShaderResourceVariable* m_hNormalMap;
	
	ID3DX11EffectVectorVariable* m_MaterialEmissiveHandle;

};
