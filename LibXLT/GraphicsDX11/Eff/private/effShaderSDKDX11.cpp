/*****************************************************************************
**  effShaderSDKDX11.cpp
**
**      effShaderSDKDX11 contains the windows implementation of the
**	effShaderSDK.
**
** 
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/eff/effShaderSDKDX11.hpp"

// Wrapper code which contains header and footer HLSL strings
#include "GraphicsDX11/eff/private/Wrappers/ShaderStrings.hpp"

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "Core/Dbg/dbgMsg.hpp"

namespace
{
	//============================================================================
	// DX11 class for maintaining the data of a compiled shader.
	// When the caller releases this class, the compiled data should be freed.
	//============================================================================
	class effShaderSDKDX11Data : public effShaderSDKData
	{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		effShaderSDKDX11Data(ID3DBlob* i_pShaderCodeBlob)
			: m_pShaderCodeBlob(i_pShaderCodeBlob) {}	
		
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~effShaderSDKDX11Data() 
		{
			if (m_pShaderCodeBlob)
				m_pShaderCodeBlob->Release();
		}

		//------------------------------------------------------------------------
		// GetData() - returns pointer to internal data. 
		//	Ownership remains with this class.
		//------------------------------------------------------------------------
		virtual void GetData(BYTE ** o_CompiledShader, 
							 int * o_CompiledShaderSize)
		{
			if (m_pShaderCodeBlob)
			{
				*o_CompiledShader = (BYTE*)m_pShaderCodeBlob->GetBufferPointer();
				*o_CompiledShaderSize = (int)m_pShaderCodeBlob->GetBufferSize();
			}
		}
	private:
		ID3DBlob* m_pShaderCodeBlob;
	};


	//------------------------------------------------------------------------
	// Decryption of swizzled HLSL code (for protection of our trade secrets)
	//------------------------------------------------------------------------
	#define PLACES	1

	//------------------------------------------------------------------------
	// decrypt()
	//------------------------------------------------------------------------
	char decrypt(char i_c, int i_numPlaces)
	{
		if ( (i_c >= 31) && (i_c <= '~') )
		{
			return i_c + i_numPlaces;
		}
		return i_c;
	}

	//------------------------------------------------------------------------
	// decryptArray()
	//------------------------------------------------------------------------
	void decryptArray( std::string* i_encrypted , int i_size, std::string& o_decrypted )
	{
		for ( int i = 0 ; i < i_size ; i++ )
		{
			const char * encrypted = i_encrypted[i].c_str();
			std::string decrypted = "";
			for ( unsigned int j = 0 ; j < i_encrypted[i].size() ; j++ )
			{
				char encrypted_chr = encrypted[j];
				char decrypted_chr = decrypt( encrypted_chr , PLACES );
				decrypted.push_back(decrypted_chr);
			}
			o_decrypted.append(decrypted);
			o_decrypted.append("\n"); // "\r\n" ?
		}
	}

	//------------------------------------------------------------------------
	//	parseErrors()
	//------------------------------------------------------------------------
	std::vector<std::string> parseErrors( const char * i_errors , int i_headerSize, std::string i_CustomShaderName )
	{
		std::vector<std::string> errors;

		std::string allErrors = i_errors;
		std::string singleError;

		// Split error strings and put them into individual vector elements
		for ( int i = 0 ; i < allErrors.length() ; i++ )
		{
			char currChar = allErrors.at(i);
			singleError.push_back( currChar );
			if ( currChar == '\n' )
			{
				errors.push_back(singleError);
				singleError.clear();
			}
		}

		// Change the error line number and input filename/path
		for ( int i = 0 ; i < errors.size() ; i++ )
		{
			int locTwo = errors[i].find( "):" , 0 );
			if ( locTwo == -1 ) continue;

			int locOne = errors[i].rfind( "(" , locTwo );
			if ( locOne == -1 ) continue;

			std::string numbers = errors[i].substr( locOne+1 , locTwo-locOne-1 );

			int locComma = numbers.find( "," , 0 );
			if ( locComma == -1 ) continue;

			std::string lineNumber = numbers.substr(0,locComma);

			int newRawNumber = atoi( lineNumber.c_str() ) - i_headerSize;

			char newLineNumber [ 16 ];
			_itoa( newRawNumber, newLineNumber, 10 );

			errors[i].replace(locOne+1, locComma, newLineNumber);
			errors[i].replace(0, locOne, i_CustomShaderName);
		}
		
		return errors;
	}
}	// end of namespace

//------------------------------------------------------------------------
//	CompileCustomShader()
//------------------------------------------------------------------------
bool effShaderSDKDX11::CompileCustomShader(const char* i_CustomShaderSrc, 
										  shared_ptr<effShaderSDKData>& o_CompiledShaderData,
										  std::vector<std::string>& o_Errors,
										  int i_headerSize,
										  const std::string& i_CustomShaderName)
{
	ID3DBlob* shaderCode = NULL;
	ID3DBlob* errors = NULL;

	// Compile effect 
	HRESULT hr;
	hr = D3DX11CompileFromMemory((LPCSTR)i_CustomShaderSrc, strlen(i_CustomShaderSrc), i_CustomShaderName.c_str(), 
		NULL, NULL, "", "fx_5_0", 
#ifdef _DEBUG
		D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_DEBUG, 
#else
		D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 
#endif
		0, 0, &shaderCode, &errors, 0);

    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			o_Errors = parseErrors ( reinterpret_cast<const char*>(errors->GetBufferPointer()),
				i_headerSize , i_CustomShaderName );
			errors->Release();
        } 
		return false;
    } 
 
	// Return a data class implementation that will free the ID3DBlob in its destructor.
	// This passes ownership of the blob to the caller.
	o_CompiledShaderData.reset( new effShaderSDKDX11Data(shaderCode) );
	
	if (errors) errors->Release();

	return true;
}

//------------------------------------------------------------------------
// CompileCustomShadeFunction()
// In this case, the string given shuld be a section of HLSL code that does not
// contain fragment and pixel shader function. This code should have a function 
// with the signature "float4 sgpu_shader_main(State state, Light_iterator light)"
// that will be called from the header and footer code.
//------------------------------------------------------------------------
bool effShaderSDKDX11::CompileCustomShadeFunction(const char* i_CustomShaderSrc, 
								 shared_ptr<effShaderSDKData>& o_CompiledShaderData,
								 std::vector<std::string>& o_Errors,
								 const std::string& i_CustomShaderName)
{
	// Encrypted strings g_Header and g_Footer are included in "ShaderStrings.hpp"
	std::string header, footer;
	decryptArray( g_Header, sizeof(g_Header)/sizeof(std::string), header );
	decryptArray( g_Footer, sizeof(g_Footer)/sizeof(std::string), footer );

	// Header size should be number of lines within g_Header
	int i_headerSize = sizeof(g_Header)/sizeof(std::string);

	std::string total_shader = header + i_CustomShaderSrc + footer;
	return CompileCustomShader(total_shader.c_str(), o_CompiledShaderData, 
								o_Errors, i_headerSize, i_CustomShaderName);
}
