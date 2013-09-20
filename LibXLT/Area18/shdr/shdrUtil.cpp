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
#include "Core/fs/fsLocator.hpp"
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
	
	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	GLuint CompileShaderFromFiles( const std::vector<const fsLocator*>& i_Locator, GLenum i_ShaderType )
	{
		fsLocator locator;
		std::vector<const char*> shaderStrings;

		for (size_t i = 0; i < i_Locator.size(); ++i) {

			// 1st try: use locator as full path.
			locator = *(i_Locator[i]);
			if (!fsFileUtil::FileExists(locator))
			{
				// 2nd try: use ShaderPath
				locator = l_ShaderPath;
				locator.Push(*(i_Locator[i]));
				if (!fsFileUtil::FileExists(locator))
				{
					// 3rd try: use app path + "/Shaders"
					locator = gfPaths::GetPath(gfPaths::e_ExePath);
					locator.Push("Shaders");
					locator.Push(*(i_Locator[i]));
					if (!fsFileUtil::FileExists(locator))
					{
						DBG_ERROR("Could not find shader " << *(i_Locator[i]));
						return E_FAIL;
					}
				}
			}
	
			itString filename;
			fsFileUtil::LocatorToUnicodeString(locator, filename);

			BYTE* b = readFileToByteArray((WCHAR*)filename.GetString());
			shaderStrings.push_back(reinterpret_cast<const char*>(b));
		}
		GLuint retval = CompileShaderFromStrings(shaderStrings, i_ShaderType);

		for (int i = 0; i < shaderStrings.size(); ++i) {
			delete [] shaderStrings[i];
		}

		return retval;
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
		std::vector<const char*> src;
		src.push_back(reinterpret_cast<const char*>(pFileData));
		GLuint shaderID = CompileShaderFromStrings(src, i_ShaderType);

		delete []pFileData;

		return shaderID;
	}

	GLuint CompileShaderFromStrings(std::vector<const char*>& i_Src, GLenum i_ShaderType)
	{
		GLuint programID = glCreateShaderProgramv( i_ShaderType, i_Src.size(), &i_Src[0]);
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