/****************************************************************************\
**  effShaderArray.hpp
**
**	GraphicsVulkan port of GraphicsDX11/Eff/effShaderArray.hpp (namespace
**	gvk): the shader manager, registering every built-in effect under the
**	names and data types GraphicsDX11 uses.
**
**      effShaderArray manages an array of shaders that chooses the 
**	appropriate shader based on the combination of texture layers.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_SHADERARRAY_HPP
#error effShaderArray.hpp multiply included
#endif
#define GVK_EFF_SHADERARRAY_HPP

#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif


#include <string>
#include <list>

class fsLocator;
class matMaterial;

class matShaderEffect;

namespace gvk
{

//class effShaderBaseVk;


class effShaderArray : public matShaderMgrImpl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~effShaderArray();

	//------------------------------------------------------------------------
	// Load effects from given directory
	//------------------------------------------------------------------------
	virtual void RegisterEffects(const fsLocator &i_ShaderDir,
			std::map<std::string, matShaderInfo>& io_ShaderMap);
	virtual void RegisterSelectableShaders(std::vector<matShaderInfo>& o_Materials,
			std::vector<matShaderInfo>& o_PostEffects);
	
	virtual void UnloadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap);
	virtual matShaderEffect* LoadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap);

	// GraphicsDX11's GetDefaultEffect, GetDefaultLightingEffect and
	// GetMapNormalsToScreen (shaders loaded from compiled .o files, for
	// shdw's rasterizer utilities) come with the shdw port that needs them.
};
}
