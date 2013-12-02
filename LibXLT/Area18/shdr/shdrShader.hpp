#pragma once

#include "Area18/ogl/oglTypes.hpp"
#include "Core/Env/envBoost.hpp"

#include "Area18/tinyxml2.h"
using namespace tinyxml2;

#include <vector>

class oglContext;

class shdrParams {};

class shdrShader;
typedef boost::shared_ptr<shdrShader> shdrShaderPtr;

class shdrShader
{
public:
	shdrShader(void);
	shdrShader(oglContext* i_pDevice, GLenum shaderType, std::vector<std::string>& i_ShaderStrings);
	shdrShader(oglContext* i_pDevice, GLenum shaderType, std::string i_ShaderString);
	shdrShader(GLuint iProgram);

	virtual ~shdrShader(void);

	GLuint GetShader() {return m_CompiledShader;}

	void LoadMetadata(const fsLocator& file);
	const XMLDocument* Metadata() {return mMetaData;}
protected:
	GLuint m_CompiledShader;
	oglContext* m_pDevice;

	void GetShaderData(GLuint i_CompiledShader);

	int FindConstantBuffer(const std::string& i_Name);

	XMLDocument* mMetaData;
};
