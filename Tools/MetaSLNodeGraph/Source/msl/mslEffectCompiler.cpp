/*****************************************************************************
**	mslEffectCompiler.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslEffectCompiler.hpp"
#undef DeleteFile
#undef CreateFile

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "GraphicsDX11/Eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/Eff/effShaderUtilWin.hpp"


#include <fstream>

namespace
{
	//----------------------------------------------------
	// WriteShaderBin()
	//----------------------------------------------------
	//bool WriteShaderBin( const char * i_out , BYTE * i_data , int i_size )
	//{
	//	std::ofstream outFile(i_out, std::ios::out | std::ios::binary);

	//	if(!outFile) return 0;
	//	
	//	outFile.write( (char*)i_data , i_size );
	//	outFile.close();

	//	return 1;
	//}
	void WriteShaderBin( const fsLocator& i_Locator , BYTE * i_data , envType::Int64 i_NumBytes )
	{
		if( fsFileUtil::FileExists(i_Locator) )
			fsFileUtil::DeleteFile(i_Locator);
		fsFileUtil::CreateFile(i_Locator);

		gfFileBin file(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		file.Write(i_NumBytes, i_data);
	}
}

//--------------------------------------------------------------------
// CompileEffect
//--------------------------------------------------------------------
matShaderEffect* mslEffectCompiler::CompileEffect(const std::string & i_HLSLShaderString,
												  const fsLocator& i_TextureDirectory,
												  shared_ptr<effShaderSDKData> &o_CompiledShaderData)
{
	// Try to compile shader
	std::vector<std::string> errors;

	// Compile stitched shader in memory.
	// In this case, the string we are giving is a section of HLSL code that does not
	// contain fragment and pixel shader hooks. This code has a function with the
	// signature "float4 sgpu_shader_main(State state, Light_iterator light)"
	// that will be called from the header and footer code.
	std::string shader_name("CustomShader");
	bool success = effShaderSDK::CompileCustomShadeFunction( i_HLSLShaderString.c_str(), 
									 o_CompiledShaderData, errors, shader_name );

	matShaderEffect* pReturnValue = NULL;
	if (success && o_CompiledShaderData)
	{
		// Get shader data as pointer. Ownership maintained by effShaderSDKData class above.
		int shaderSize = 0;
		BYTE * pCompiledShader = NULL;
		o_CompiledShaderData->GetData(&pCompiledShader, &shaderSize);

		// Compile as DX11 effect
		ID3DX11Effect* pDX11Effect =  effShaderUtilWin::LoadEffectData(pCompiledShader, shaderSize, shader_name);

		// Create shader from effect
		pReturnValue = new effShaderBaseDX11(i_TextureDirectory, pDX11Effect, shader_name);

	}
	else
	{
		// Log errors
		for (int i=0; i<errors.size(); i++)
			DBG_ERROR(errors[i]);
	}

	return pReturnValue;
}

