#pragma once

#include "Area18/ogl/oglTypes.hpp"

class oglShader
{
public:
	oglShader(GLenum shaderType);
	virtual ~oglShader(void);
	bool	Load(const std::string& fileName);
	bool	Load(const char* fileName);
	bool	Compile(void);
	void	PrintInfoLog();
	
	const char*		GetInfoLog();
	inline GLuint	GetID(){return m_ID;}

protected:
	char* oglShader::FileRead(const char* fileName) ;

protected:
	GLenum	m_Type; // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_ID;
	char*	m_code;
};
