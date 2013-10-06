/****************************************************************************\
**	shdrVS.cpp
**
**		see .hpp
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/shdr/shdrPipeline.hpp"

#include "Area18/ogl/oglDevice.hpp"
#include "Area18/shdr/shdrShader.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/ma/maMatrix4x4.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrPipeline::shdrPipeline()
{
	glGenProgramPipelines(1, &m_PipelineID);
	glBindProgramPipeline(m_PipelineID);
	CHECKGLERROR();
}

shdrPipeline::shdrPipeline(const shdrPipeline& that)
{
	glGenProgramPipelines(1, &m_PipelineID);
	glBindProgramPipeline(m_PipelineID);
	this->AttachVS(that.vs());
	this->AttachPS(that.ps());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrPipeline::~shdrPipeline()
{
	glDeleteProgramPipelines(1, &m_PipelineID);
}

void shdrPipeline::AttachVS(shdrShaderPtr i_pVS)
{
	glUseProgramStages(m_PipelineID, GL_VERTEX_SHADER_BIT, i_pVS->GetShader());
	CHECKGLERROR();
	m_pVS = i_pVS;
}
void shdrPipeline::AttachPS(shdrShaderPtr i_pPS)
{
	glUseProgramStages(m_PipelineID, GL_FRAGMENT_SHADER_BIT, i_pPS->GetShader());
	CHECKGLERROR();
	m_pPS = i_pPS;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrPipeline::Bind(oglDevice* i_pDevice)
{
	glValidateProgramPipeline(m_PipelineID);
	CHECKGLERROR();
	glBindProgramPipeline(m_PipelineID);
	CHECKGLERROR();
	// set shader constants here too?
}

void shdrPipeline::Unbind()
{
	glBindProgramPipeline(0);
}

void shdrPipeline::activateVs() const
{
	glActiveShaderProgram(m_PipelineID, m_pVS->GetShader());
	CHECKGLERROR();
}
void shdrPipeline::activatePs() const
{
	glActiveShaderProgram(m_PipelineID, m_pPS->GetShader());
	CHECKGLERROR();
}

