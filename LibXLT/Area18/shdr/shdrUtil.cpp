/****************************************************************************\
**  shdrUtil.hpp
**
**      shdrUtil sets up effect shaders from files
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Area18/shdr/shdrUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"

namespace shdrUtil
{
	fsLocator l_ShaderPath = itString("D:\\dev\\CompletelyDifferent\\LibXLT\\Area18\\shdr\\");

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fsLocator GetShaderPath()
	{
		return l_ShaderPath;
	}

	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	GLuint CompileShaderFromFile( const fsLocator& i_Locator, GLenum i_ShaderType )
	{
		fsLocator locator;

		// 1st try: use locator as full path.
		locator = i_Locator;
		if (!fsFileUtil::FileExists(locator))
		{
			// 2nd try: use ShaderPath
			locator = l_ShaderPath;
			locator.Push(i_Locator);
			if (!fsFileUtil::FileExists(locator))
			{
				// 3rd try: use app path + "/Shaders"
				locator = gfPaths::GetPath(gfPaths::e_ExePath);
				locator.Push("Shaders");
				locator.Push(i_Locator);
				if (!fsFileUtil::FileExists(locator))
				{
					DBG_ERROR("Could not find shader " << i_Locator);
					return E_FAIL;
				}
			}
		}
	
		itString filename;
		fsFileUtil::LocatorToUnicodeString(locator, filename);

		return CompileShaderFromFile((WCHAR*)filename.GetString(), i_ShaderType);
	}
	
	// caller must delete return value.
	BYTE* readFileToByteArray(WCHAR* szFileName)
	{
		// open the file
		HANDLE hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( INVALID_HANDLE_VALUE == hFile )
			return NULL;

		// Get the file size
		LARGE_INTEGER FileSize;
		GetFileSizeEx( hFile, &FileSize );

		// create enough space for the file data
		// add one for a null terminator
		BYTE* pFileData = new BYTE[ FileSize.LowPart + 1 ];
		if( !pFileData )
			return NULL;

		// read the data in
		DWORD BytesRead;
		if( !ReadFile( hFile, pFileData, FileSize.LowPart, &BytesRead, NULL ) ) {
			delete [] pFileData;
			return NULL; 
		}
		// need to null terminate as a string.
		pFileData[FileSize.LowPart] = '\0';

		CloseHandle( hFile );

		return pFileData;
	}

	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	GLuint CompileShaderFromFile( WCHAR* szFileName, GLenum i_ShaderType )
	{
		// find the file
		//WCHAR str[MAX_PATH];
		//( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

		BYTE* pFileData = readFileToByteArray(szFileName);

		// convert filename
		char pFilePathName[MAX_PATH];        
		WideCharToMultiByte(CP_ACP, 0, szFileName, -1, pFilePathName, MAX_PATH, NULL, NULL);

		// Compile the shader
		DBG_LOG("compiling shader " << std::string(pFilePathName));
		GLuint shaderID = CompileShaderFromString((const char*)pFileData, i_ShaderType);

		delete []pFileData;

		return shaderID;
	}

	GLuint CompileShaderFromString(const char* i_Src, GLenum i_ShaderType)
	{
		GLuint programID = glCreateShaderProgramv( i_ShaderType, 1, &i_Src);
		char infoLog[8192];
		glGetProgramInfoLog(programID, 8192, NULL, infoLog);
		DBG_LOG("Shader Compile Log:\n" << infoLog);
		return programID;

		// Compile the shader
//		GLuint shaderID = glCreateShader(i_ShaderType);
//		glShaderSource(shaderID, 1, &i_Src, NULL);
//		glCompileShader(shaderID);
//		int shaderStatus = 0;
//		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderStatus);
//		{
//			char infoLog[8192];
//			glGetShaderInfoLog(shaderID, 8192, NULL, infoLog);
//			DBG_LOG("Shader Compile Log:\n" << infoLog);
//		}
//
//		return shaderID;
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

}	// end of namespace