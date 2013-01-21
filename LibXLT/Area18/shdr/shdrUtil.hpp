/****************************************************************************\
**  shdrUtil.hpp
**
**      shdrUtil sets up effect shaders from files
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#pragma once

#include "Area18/ogl/oglTypes.hpp"

#include <string>

class fsLocator;
class matShaderEffect;

namespace shdrUtil
{
	//------------------------------------------------------------------------
	// Initialize system
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	// Clean up system
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fsLocator GetShaderPath();

	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	GLuint CompileShaderFromFile( const fsLocator& i_Locator, 
		GLenum i_ShaderType );
	GLuint CompileShaderFromFile( WCHAR* szFileName, 
		GLenum i_ShaderType );

	//--------------------------------------------------------------------------------------
	// Helper function to compile an hlsl shader from file, 
	// its binary compiled code is returned
	//--------------------------------------------------------------------------------------
	GLuint LoadShaderFromFile( const WCHAR* szFileName );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	GLuint CompileShaderFromString(const char* i_Src, GLenum i_ShaderType );
}
