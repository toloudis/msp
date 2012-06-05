/****************************************************************************\
**  effShaderUtilWin.hpp
**
**      effShaderUtilWin sets up effect shaders from files
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effShaderUtilWin.hpp"

#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/it/itStringUtil.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

namespace effShaderUtilWin
{


	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
	{
		HRESULT hr = S_OK;

		// find the file
		//WCHAR str[MAX_PATH];
		//( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

		// open the file
		HANDLE hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( INVALID_HANDLE_VALUE == hFile )
			return E_FAIL;

		// Get the file size
		LARGE_INTEGER FileSize;
		GetFileSizeEx( hFile, &FileSize );

		// create enough space for the file data
		BYTE* pFileData = new BYTE[ FileSize.LowPart ];
		if( !pFileData )
			return E_OUTOFMEMORY;

		// read the data in
		DWORD BytesRead;
		if( !ReadFile( hFile, pFileData, FileSize.LowPart, &BytesRead, NULL ) )
			return E_FAIL; 

		CloseHandle( hFile );

		// Compile the shader
		char pFilePathName[MAX_PATH];        
		WideCharToMultiByte(CP_ACP, 0, szFileName, -1, pFilePathName, MAX_PATH, NULL, NULL);
		ID3DBlob* pErrorBlob = NULL;
		hr = D3DCompile( pFileData, FileSize.LowPart, pFilePathName, NULL, NULL, szEntryPoint, szShaderModel, D3D10_SHADER_ENABLE_STRICTNESS, 0, ppBlobOut, &pErrorBlob );

		delete []pFileData;

		if( FAILED(hr) )
		{
			if (pErrorBlob)
			{
				OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
				pErrorBlob->Release();
			}
			return hr;
		}
		if (pErrorBlob)
			pErrorBlob->Release();

		return S_OK;
	}
	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	HRESULT LoadShaderFromFile( const WCHAR* szFileName, SIZE_T* pSizeOut, void** ppBlobOut )
	{
		HRESULT hr = S_OK;

		// find the file
		//WCHAR str[MAX_PATH];
		//( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

		// open the file
		HANDLE hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( INVALID_HANDLE_VALUE == hFile )
			return E_FAIL;

		// Get the file size
		LARGE_INTEGER FileSize;
		GetFileSizeEx( hFile, &FileSize );

		// create enough space for the file data
		BYTE* pFileData = new BYTE[ FileSize.LowPart ];
		if( !pFileData )
			return E_OUTOFMEMORY;

		// read the data in
		DWORD BytesRead;
		if( !ReadFile( hFile, pFileData, FileSize.LowPart, &BytesRead, NULL ) )
			return E_FAIL; 

		CloseHandle( hFile );

		*pSizeOut = FileSize.LowPart;
		*ppBlobOut = pFileData;
		return S_OK;
	}

	void LoadShaderDX11(const fsLocator& i_Locator, 
		const std::string& i_Entrypoint,
		const std::string& i_Profile,
		ID3DBlob** o_Shader)
	{
		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_Locator, filename);
		//DBG_LOG1("Reading effect from file: %s", filename.c_str());

		ID3DBlob* shader = NULL;
		ID3DBlob* errors = NULL;

		HRESULT op_result = D3DX11CompileFromFile(
		  filename.GetString(),//LPCTSTR pSrcFile,
		  NULL,//CONST D3D11_SHADER_MACRO *pDefines,
		  NULL,//LPD3D10INCLUDE pInclude,
		  i_Entrypoint.c_str(),//LPCSTR pFunctionName,
		  i_Profile.c_str(),//LPCSTR pProfile,
		  0,//UINT Flags1,
		  0,//UINT Flags2,
		  NULL,//ID3DX11ThreadPump *pPump,
		  &shader,//ID3DBlob **ppShader,
		  &errors,//ID3DBlob **ppErrorMsgs,
		  NULL//HRESULT *pHResult
		);

		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			if (errors != NULL)
			{
				const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
				DBG_WARNING(err_msg);
			}
		}

		if( errors )
			errors->Release();

		*o_Shader = shader;

	}

ID3DX11Effect* LoadEffectData(void* i_Eff, int i_nBytes, std::string i_Name)
{
    // Create effect 
	ID3DX11Effect* effect = NULL;
	HRESULT hr = D3DX11CreateEffectFromMemory(i_Eff, i_nBytes,
		0, g2dDX11Global::g_pDevice, &effect);

    if(FAILED(hr)) 
    { 
		g2dDX11Global::PrintDXError(hr);
        return NULL; 
    } 

	return effect;
}


// **** DX11 Version ****
ID3DX11Effect* LoadEffectDX11(const fsLocator& i_Locator)
{
	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

	std::string sFileName;
	fsFileUtil::LocatorToANSIFilename(i_Locator, sFileName);

	//DBG_LOG1("Reading effect from file: %s", filename.c_str());

	ID3DBlob* shaderCode = NULL;
	ID3DBlob* errors = NULL;

	void* pFileData = NULL;
	SIZE_T fileSize = 0;
	HRESULT hr = LoadShaderFromFile( filename.GetString(), &fileSize, &pFileData );

// assuming precompiled shader!
    // Create effect 
	ID3DX11Effect* effect = NULL;
	effect = LoadEffectData(pFileData, fileSize, sFileName);

	// free up the file contents now
	delete [] pFileData;

#if 0

	// Compile effect 
	hr = D3DX11CompileFromMemory((LPCSTR)pFileData, fileSize, sFileName.c_str(), 
		NULL, NULL, "", "fx_5_0", 
#ifdef _DEBUG
		D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_DEBUG, 
#else
		D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 
#endif
		0, 0, &shaderCode, &errors, 0);

	// free up the file contents now
	delete [] pFileData;

    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_WARNING(err_msg);
        } 
        return NULL; 
    } 
 
    // Create effect 
	ID3DX11Effect* effect = NULL;
	effect = LoadEffectData(shaderCode->GetBufferPointer(), (int)shaderCode->GetBufferSize(), sFileName);
#endif

	return effect;
}

//------------------------------------------------------------------------
// Initialize system
//------------------------------------------------------------------------
void Initialize()
{
}

//------------------------------------------------------------------------
// Clean up system
//------------------------------------------------------------------------
void DeInitialize()
{
}
//------------------------------------------------------------------------
// Create effect from file
//------------------------------------------------------------------------
matShaderEffect* CompileEffect(const fsLocator &i_Locator)
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::string name = itStringUtil::GetStdString(i_Locator.GetLastName());
	ID3DX11Effect* effect = LoadEffectDX11(i_Locator);
	fsLocator directory = i_Locator;
	directory.Pop();
	effShaderBaseDX11* pEffect = new effShaderBaseDX11(directory, effect, name);

	fsResourceTracker::MarkEnd(i_Locator);
	return pEffect;
}

matShaderEffect* CompileGenericEffect(const fsLocator &i_Locator)
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::string name = itStringUtil::GetStdString(i_Locator.GetLastName());
	ID3DX11Effect* effect = LoadEffectDX11(i_Locator);
	fsLocator directory = i_Locator;
	directory.Pop();
	effShaderBaseDX11* pEffect = new effShaderBaseDX11(directory, effect, name);

	fsResourceTracker::MarkEnd(i_Locator);
	return pEffect;
}

ID3DX11Effect* CompileStringEffect(const char* eff)
{
	ID3DBlob* shaderCode = NULL;
	ID3DBlob* errors = NULL;

	std::string s("unknown shader");

	// Compile effect 
	HRESULT hr;
	hr = D3DX11CompileFromMemory((LPCSTR)eff, strlen(eff), s.c_str(), 
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
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_WARNING(err_msg);
        } 
        return NULL; 
    } 
 
    // Create effect 
	ID3DX11Effect* effect = NULL;
	effect = LoadEffectData(shaderCode->GetBufferPointer(), (int)shaderCode->GetBufferSize(), s);

	return effect;
}

}	// end of namespace