/*****************************************************************************
**  effBake.hpp
**
**      effBake is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_BAKE_HPP
#error effBake.hpp multiply included
#endif
#define GVK_EFF_BAKE_HPP

#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif

class matMaterial;

namespace gvk
{


class effBake : public effShaderBaseVk
{
public:
	effBake(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

	virtual maFloatRGBA GetDiffuse() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetAmbient() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetSpecular() const {return maFloatRGBA(0,0,0,0);}


protected:
	fxEffectVariable* m_hHasEmissive;
	fxEffectVariable* m_hEmissiveMap;
	fxEffectVariable* m_hHasNormal;
	fxEffectVariable* m_hNormalMap;
	
	fxEffectVariable* m_MaterialEmissiveHandle;

};
}
