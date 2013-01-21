#include "GLShader.h"

#include "Core/dbg/dbgMsg.hpp"

oglShader::oglShader(GLenum shaderType)
{
	m_Type = shaderType;
	m_ID = glCreateShader(shaderType); 
	m_code = NULL;
}

oglShader::~oglShader(void)
{
	if(m_code) delete [] m_code;
	glDeleteShader(m_ID);
}

bool oglShader::Load(const std::string& fileName)
{
	return Load(fileName.c_str());
}

bool oglShader::Load(const char* fileName)
{
	DBG_LOG("Load shader " << fileName);

	m_code = FileRead(fileName);
	if(m_code) return true;
	else return false;
}

bool oglShader::Compile(void)
{
	if(m_code == NULL) return false;
	const char * code = m_code;

	glShaderSource(m_ID, 1, &code, NULL);
	glCompileShader(m_ID);

	int param;
	glGetShaderiv(m_ID, GL_COMPILE_STATUS, &param);
	if(param == GL_TRUE) return true;
	else
	{
		const char* log = GetInfoLog();
		if (log != NULL) {
			DBG_LOG(log);
		}
		else {
			DBG_LOG("");
		}
		return false;
	}
}

char* oglShader::FileRead(const char* fileName) 
{
	FILE *fp;
	char *content = NULL;

	int count=0;

	fp = fopen(fileName,("rt"));
	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);
		if (count > 0) 
		{
			content = new char [count+1];
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}

	return content;
}

const char* oglShader::GetInfoLog()
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog = NULL;

	glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = new char [infologLength];
        glGetShaderInfoLog(m_ID, infologLength, &charsWritten, infoLog);
    }
	return infoLog;
}

void oglShader::PrintInfoLog()
{
	const char* log = GetInfoLog();
	if (log != NULL) {
		DBG_LOG(log);
	}
	else {
		DBG_LOG("");
	}
}
