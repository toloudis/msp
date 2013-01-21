/****************************************************************************\
**	shdrVS.cpp
**
**		see .hpp
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/shdr/shdrVS.hpp"

#include "Area18/ogl/oglDevice.hpp"
#include "Area18/shdr/shdrUtil.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/ma/maMatrix4x4.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrVS::shdrVS()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrVS::shdrVS(oglContext* i_pDevice, eShaderSource i_Source, std::string i_ShaderString)
{
	GLuint id = 0;
	if (i_Source == shdrFile)
	{
		fsLocator locator(itString(i_ShaderString.c_str()));
		id = shdrUtil::CompileShaderFromFile(locator, GL_VERTEX_SHADER);
	}
	else
	{
		id = shdrUtil::CompileShaderFromString(i_ShaderString.c_str(), GL_VERTEX_SHADER);
	}
	if (id > 0)
	{
		GetShaderData(id);
		m_pDevice = i_pDevice;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrVS::~shdrVS()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrVS::BindShader()
{
}
