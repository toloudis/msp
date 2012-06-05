/****************************************************************************\
**	g3dExceptionX.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dExceptionX.hpp"


//============================================================================
//	g3dShaderLoadX is thrown when a shader can not be loaded.
//============================================================================

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g3dShaderLoadX::g3dShaderLoadX(std::string i_ShaderName)
:	m_ShaderName(i_ShaderName)
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g3dShaderLoadX::GetErrorMessage() const
{
	return "Failed to load shader: " + m_ShaderName;
}

//------------------------------------------------------------------------
// GetNodeName
//------------------------------------------------------------------------
std::string g3dShaderLoadX::GetShaderName() const
{
	return m_ShaderName;
}

