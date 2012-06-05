/*****************************************************************************
**	mslEffectCompiler.hpp
**
**	 mslEffectCompiler takes a HLSL shader string and compiles it into 
**	a DirectX11 effect that can be used by a material.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_EFFECTCOMPILER_HPP
#error mslEffectCompiler.hpp multiply included
#endif
#define MSL_EFFECTCOMPILER_HPP

#ifndef EFF_SHADERSDK_HPP
#include "Graphics/Eff/effShaderSDK.hpp"
#endif 

class fsLocator;
class matShaderEffect;

#include <string>

//============================================================================
//============================================================================
namespace mslEffectCompiler 
{
	//--------------------------------------------------------------------
	// CompileEffect from string in memory (i_ShaderString).
	// i_TextureDirectory is used to locate built-in textures.
	// i_EffectFilename sets filename to export binary (optional).
	// i_MetaSLShaderString can be used to backup the original source
	// into the binary MFX file written (optional).
	//--------------------------------------------------------------------
	matShaderEffect*  CompileEffect(const std::string & i_HLSLShaderString,
									const fsLocator& i_TextureDirectory,
									shared_ptr<effShaderSDKData> &o_CompiledShaderData);

}
