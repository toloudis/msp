/****************************************************************************\
**  effShaderArray.hpp
**
**      effShaderArray manages an array of shaders that chooses the 
**	appropriate shader based on the combination of texture layers.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_SHADERARRAY_HPP
#error effShaderArray.hpp multiply included
#endif
#define EFF_SHADERARRAY_HPP

#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#include <string>
#include <list>

//class effShaderBaseDX11;
class fsLocator;
class matMaterial;
class matShaderEffect;

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

	//------------------------------------------------------------------------
	// there is one single default effect.
	//------------------------------------------------------------------------
//	static effShaderBaseDX11* GetDefaultEffect();
	static void GetDefaultEffect(void** o_pShaderBytecode, unsigned long* o_bytecodeLength,
									  ID3D11VertexShader** o_pVS, ID3D11PixelShader** o_pPS );

	static void GetDefaultLightingEffect(ID3D11VertexShader** o_pVS, ID3D11PixelShader** o_pPS);


	// postproc fullscreen quad shaders:
	static ID3D11PixelShader* GetMapNormalsToScreen();

private:
	//------------------------------------------------------------------------
	// Load registered (internally known) shaders
	//------------------------------------------------------------------------
	void LoadAllShaders(std::map<std::string, matShaderInfo>& io_ShaderMap);

};
