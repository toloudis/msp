/*****************************************************************************
**  effShaderSDK.hpp
**
**      Shader SDK generic and implementation interfaces
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_SHADERSDK_HPP
#error effShaderSDK.hpp multiply included
#endif
#define EFF_SHADERSDK_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


#include <string>
#include <vector>
#include <windows.h>


//============================================================================
//	forward references
//============================================================================
class fsLocator;


//============================================================================
// Abstract class for maintaining the data of a compiled shader.
// When the caller releases this class, the compiled data should be freed.
//============================================================================
class effShaderSDKData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~effShaderSDKData() = 0 {}

	//------------------------------------------------------------------------
	// GetData() - returns pointer to internal data. 
	//	Ownership remains with this class.
	//------------------------------------------------------------------------
	virtual void GetData(BYTE ** o_CompiledShader, 
						 int * o_CompiledShaderSize) = 0;
};

//============================================================================
//============================================================================
class effShaderSDKImpl
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~effShaderSDKImpl() {};

	//------------------------------------------------------------------------
	// CompileCustomShader()
	//------------------------------------------------------------------------
	virtual bool CompileCustomShader(const char* i_CustomShaderSrc, 
									 shared_ptr<effShaderSDKData>& o_CompiledShaderData,
									 std::vector<std::string>& o_Errors,
									 int i_headerSize,
									 const std::string& i_CustomShaderName) { return false; };

	//------------------------------------------------------------------------
	// CompileCustomShadeFunction()
	// In this case, the string given shuld be a section of HLSL code that does not
	// contain fragment and pixel shader function. This code should have a function 
	// with the signature "float4 sgpu_shader_main(State state, Light_iterator light)"
	// that will be called from the header and footer code.
	//------------------------------------------------------------------------
	virtual bool CompileCustomShadeFunction(const char* i_CustomShaderSrc, 
											 shared_ptr<effShaderSDKData>& o_CompiledShaderData,
											 std::vector<std::string>& o_Errors,
											 const std::string& i_CustomShaderName) { return false; };
};


//============================================================================
//============================================================================
namespace effShaderSDK
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetImplementation(effShaderSDKImpl *i_pImpl);

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
}

