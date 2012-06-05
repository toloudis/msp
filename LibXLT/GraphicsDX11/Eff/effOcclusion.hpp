/*****************************************************************************
**  effOcclusion.hpp
**
**      effOcclusion is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_OCCLUSION_HPP
#error effOcclusion.hpp multiply included
#endif
#define EFF_OCCLUSION_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effOcclusion : public effShaderBaseDX11
{
public:
	effOcclusion(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;
	virtual void SetupGlowPass(const effGlowData& i_GlowData);

protected:
};
