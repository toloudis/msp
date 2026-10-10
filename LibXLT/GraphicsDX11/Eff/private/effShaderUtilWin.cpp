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

        HRESULT op_result = D3DCompileFromFile(
            filename.GetString(),//in      LPCWSTR pFileName,
            NULL,//in_opt  const D3D_SHADER_MACRO pDefines,
            NULL,//in_opt  ID3DInclude pInclude,
            i_Entrypoint.c_str(),//in      LPCSTR pEntrypoint,
            i_Profile.c_str(),//in      LPCSTR pTarget,
            0,//in      UINT Flags1,
            0,//in      UINT Flags2,
            &shader,//out     ID3DBlob ppCode,
            &errors//out_opt ID3DBlob ppErrorMsgs
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

}	// end of namespace