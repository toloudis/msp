/*****************************************************************************
**  effShaderSDKDX11.hpp
**
**      effShaderSDKDX11 contains the windows implementation of the
**	effShaderSDK.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_SHADERSDKDX11_HPP
#error effShaderSDKDX11.hpp multiply included
#endif
#define EFF_SHADERSDKDX11_HPP

#ifndef EFF_SHADERSDK_HPP
#include "Graphics/eff/effShaderSDK.hpp"
#endif

#include <string>

//============================================================================
//============================================================================
class fsLocator;

//============================================================================
//============================================================================
class effShaderSDKDX11 : public effShaderSDKImpl
{
public:

	//------------------------------------------------------------------------
	// CompileCustomShader()
	//------------------------------------------------------------------------
	bool CompileCustomShader(const char* i_CustomShaderSrc,
							 shared_ptr<effShaderSDKData>& o_CompiledShaderData,
							 std::vector<std::string>& o_Errors,
							 int i_headerSize,
							 const std::string& i_CustomShaderName);

	//------------------------------------------------------------------------
	// CompileCustomShadeFunction()
	// In this case, the string given shuld be a section of HLSL code that does not
	// contain fragment and pixel shader function. This code should have a function 
	// with the signature "float4 sgpu_shader_main(State state, Light_iterator light)"
	// that will be called from the header and footer code.
	//------------------------------------------------------------------------
	bool CompileCustomShadeFunction(const char* i_CustomShaderSrc, 
									 shared_ptr<effShaderSDKData>& o_CompiledShaderData,
									 std::vector<std::string>& o_Errors,
									 const std::string& i_CustomShaderName);
};
