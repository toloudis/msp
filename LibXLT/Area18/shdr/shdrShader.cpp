#include "Area18/shdr/shdrShader.hpp"
#include "Area18/shdr/shdrUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Fs/fsLocator.hpp"

shdrShader::shdrShader(void)
{
}
shdrShader::shdrShader(oglContext* i_pDevice, GLenum shaderType, std::vector<std::string>& i_ShaderStrings)
{
	std::ostringstream src;
	for(size_t i = 0; i < i_ShaderStrings.size(); ++i) {
		src << i_ShaderStrings[i];
	}
	GLuint id = shdrUtil::CompileShaderFromString(src.str().c_str(), shaderType);
	if (id > 0)
	{
		GetShaderData(id);
		m_pDevice = i_pDevice;
	}
}
shdrShader::shdrShader(oglContext* i_pDevice, GLenum shaderType, std::string i_ShaderString)
{
	GLuint id = 0;
	fsLocator locator(itString(i_ShaderString.c_str()));
	id = shdrUtil::CompileShaderFromFile(locator, shaderType);
	if (id > 0)
	{
		GetShaderData(id);
		m_pDevice = i_pDevice;
	}
}

shdrShader::~shdrShader(void)
{
	glDeleteProgram(m_CompiledShader);
}

void shdrShader::GetShaderData(GLuint i_CompiledShader)
{
	m_CompiledShader = i_CompiledShader;
}

int shdrShader::FindConstantBuffer(const std::string& i_Name)
{
	return -1;
}

