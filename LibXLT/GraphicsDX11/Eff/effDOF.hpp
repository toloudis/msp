/*****************************************************************************
**  effDOF.hpp
**
**      effDOF is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_DOF_HPP
#error effDOF.hpp multiply included
#endif
#define EFF_DOF_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effDOF : public effShaderBaseDX11 
{
public:
	effDOF(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupParams(const effShaderData* i_Data) const;

protected:
	ID3DX11EffectShaderResourceVariable* m_hSharpTexture;
	ID3DX11EffectShaderResourceVariable* m_hBlurryTexture;
	ID3DX11EffectVectorVariable* m_hSharpResolutionData; 
	ID3DX11EffectVectorVariable* m_hBlurryResolutionData;
	ID3DX11EffectScalarVariable* m_hMaxCoC;
};
