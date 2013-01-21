#include "shdrLambertPS.h"

#include "Area18/ogl/oglContext.h"

#include "Core/ma/maFloatRGBA.hpp"
#include "Core/ma/maVector4d.hpp"
#include "Graphics/G3d/g3dPointLight.hpp"

shdrLambertPS::shdrLambertPS(oglContext* i_pDevice)
:	shdrPS(i_pDevice, shdrPS::shdrFile, "LambertPS.glsl")
{
	cbLambert initLambert;
	cbLight initLight;

	glGenBuffers(1, &mCB);
	glBindBuffer(GL_UNIFORM_BUFFER, mCB);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cbLambert), &initLambert, GL_STREAM_DRAW);

	GLuint cbIndex = glGetUniformBlockIndex(m_CompiledShader, "cb0");
	// bind it to the 0th location
	glUniformBlockBinding( m_CompiledShader, cbIndex, 0 );

	glGenSamplers(1, &mDiffuseSampler);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mDiffuseSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenBuffers(1, &mCBLight);
	glBindBuffer(GL_UNIFORM_BUFFER, mCBLight);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cbLight), &initLight, GL_STREAM_DRAW);

	cbIndex = glGetUniformBlockIndex(m_CompiledShader, "cb0");
	// bind it to the 0th location
	glUniformBlockBinding( m_CompiledShader, cbIndex, 0 );

	CHECKGLERROR();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrLambertPS::~shdrLambertPS()
{
	glDeleteSamplers(1, &mDiffuseSampler);
	glDeleteBuffers(1, &mCB);
	glDeleteBuffers(1, &mCBLight);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrLambertParams::SetColor(const maFloatRGBA& i_Color)
{
	memcpy(mLambert.g_Color, i_Color.GetPtr(), 4*sizeof(float)); 
}
void shdrLambertParams::SetEyePos(const maVector4d& i_E)
{
	memcpy(mLambert.g_EyePos, i_E.GetPtr(), 4*sizeof(float)); 
}
void shdrLambertParams::SetTexture(oglTexture2dHandle iTex)
{
	mDiffuseTexture = iTex;
}
void shdrLambertParams::SetLight(g3dPointLight* iLight)
{
	memcpy(mLight.g_Pos, iLight->GetPosition().GetPtr(), 3*sizeof(float)); 
	mLight.g_Pos[3] = 1;
	memcpy(mLight.g_Diffuse, iLight->GetScaledIntensity().GetPtr(), 4*sizeof(float)); 
	memcpy(mLight.g_Specular, iLight->GetScaledIntensity().GetPtr(), 4*sizeof(float)); 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrLambertPS::BindConstants(shdrParams* iParams)
{
	shdrLambertParams* params = (shdrLambertParams*)iParams;

	glBindBuffer(GL_UNIFORM_BUFFER, mCB);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cbLambert), &params->mLambert);
	GLuint bufferIndex = 0;
	glBindBufferRange(GL_UNIFORM_BUFFER, bufferIndex, mCB, 0, sizeof(cbLambert));

	glBindBuffer(GL_UNIFORM_BUFFER, mCBLight);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cbLight), &params->mLight);
	bufferIndex++;
	glBindBufferRange(GL_UNIFORM_BUFFER, bufferIndex, mCBLight, 0, sizeof(cbLight));

	GLuint texture_unit = 0;
	glBindSampler(texture_unit, mDiffuseSampler);
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, params->mDiffuseTexture->GetResource());

	CHECKGLERROR();
}
