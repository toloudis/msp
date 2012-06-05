/*****************************************************************************
**  effShaderSDK.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effShaderSDK.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace effShaderSDK
{
namespace
{
	effShaderSDKImpl *l_pImpl = NULL;
}

//------------------------------------------------------------------------
// SetImplementation()
//------------------------------------------------------------------------
void SetImplementation(effShaderSDKImpl *i_pImpl)
{
	l_pImpl = i_pImpl;
}


//------------------------------------------------------------------------
// CompileCustomShader()
//------------------------------------------------------------------------
bool CompileCustomShader(const char* i_CustomShaderSrc, 
						 shared_ptr<effShaderSDKData>& o_CompiledShaderData,
						 std::vector<std::string>& o_Errors,
						 int i_headerSize,
						 const std::string& i_CustomShaderName)
{
	DBG_ASSERT(l_pImpl, "No ShaderSDK implementation.");
	if (!l_pImpl)
		return false;
	return l_pImpl->CompileCustomShader( i_CustomShaderSrc, o_CompiledShaderData, o_Errors, i_headerSize, i_CustomShaderName);
}

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
								 const std::string& i_CustomShaderName)
{
	DBG_ASSERT(l_pImpl, "No ShaderSDK implementation.");
	if (!l_pImpl)
		return false;
	return l_pImpl->CompileCustomShadeFunction( i_CustomShaderSrc, o_CompiledShaderData, o_Errors, i_CustomShaderName);
}
}
