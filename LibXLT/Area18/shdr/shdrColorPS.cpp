/****************************************************************************\
**	rasterizerUtil.cpp
**
**		see .hpp
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/shdr/shdrColorPS.hpp"

#include "Area18/ogl/oglContext.h"

#include "Core/ma/maFloatRGBA.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrColorPS::shdrColorPS(oglContext* i_pDevice)
:	shdrPS(i_pDevice, shdrPS::shdrFile, "ColorPS.glsl")
{
	glGenBuffers(1, &mCB);
	glBindBuffer(GL_UNIFORM_BUFFER, mCB);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cb), &m_Data, GL_STREAM_DRAW);

	GLuint cbIndex = glGetUniformBlockIndex(m_CompiledShader, "cb0");
	// bind it to the 0th location
	glUniformBlockBinding( m_CompiledShader, cbIndex, 0 );

	CHECKGLERROR();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrColorPS::~shdrColorPS()
{
	glDeleteBuffers(1, &mCB);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrColorPS::SetColor(const maFloatRGBA& i_Color)
{
	memcpy(m_Data.g_Color, i_Color.GetPtr(), 4*sizeof(float)); 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrColorPS::BindConstants(shdrParams* iData)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mCB);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cb), &m_Data);
	GLuint bufferIndex = 0;
	glBindBufferRange(GL_UNIFORM_BUFFER, bufferIndex, mCB, 0, sizeof(cb));

	CHECKGLERROR();
}
