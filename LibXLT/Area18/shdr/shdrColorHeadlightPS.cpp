/****************************************************************************\
**	rasterizerUtil.cpp
**
**		see .hpp
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/shdr/shdrColorHeadlightPS.hpp"

#include "Area18/ogl/oglContext.h"

#include "Core/ma/maFloatRGBA.hpp"
#include "Core/ma/maVector4d.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrColorHeadlightPS::shdrColorHeadlightPS(oglContext* i_pDevice)
:	shdrPS(i_pDevice, shdrPS::shdrFile, "ColorHeadlightPS.glsl")
{
	glGenBuffers(1, &mCB);
	glBindBuffer(GL_UNIFORM_BUFFER, mCB);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cb), &m_Data, GL_STREAM_DRAW);

	GLuint cbIndex = glGetUniformBlockIndex(m_CompiledShader, "cb0");
	// bind it to the 0th location
	glUniformBlockBinding( m_CompiledShader, cbIndex, 0 );

	glGenSamplers(1, &mDiffuseSampler);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	CHECKGLERROR();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrColorHeadlightPS::~shdrColorHeadlightPS()
{
	glDeleteSamplers(1, &mDiffuseSampler);
	glDeleteBuffers(1, &mCB);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrColorHeadlightPS::SetColor(const maFloatRGBA& i_Color)
{
	memcpy(m_Data.g_Color, i_Color.GetPtr(), 4*sizeof(float)); 
}
void shdrColorHeadlightPS::SetEyePos(const maVector4d& i_E)
{
	memcpy(m_Data.g_EyePos, i_E.GetPtr(), 4*sizeof(float)); 
}
void shdrColorHeadlightPS::SetTexture(oglTexture2dHandle iTex)
{
	mDiffuseTexture = iTex;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrColorHeadlightPS::BindConstants(shdrParams* iData)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mCB);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cb), &m_Data);
	GLuint bufferIndex = 0;
	glBindBufferRange(GL_UNIFORM_BUFFER, bufferIndex, mCB, 0, sizeof(cb));

	GLuint texture_unit = 0;
	glBindSampler(texture_unit, mDiffuseSampler);
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, mDiffuseTexture->GetResource());

	CHECKGLERROR();
}
