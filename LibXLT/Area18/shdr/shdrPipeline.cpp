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
:	m_pVS(NULL),
	m_pPS(NULL)
{
	glGenProgramPipelines(1, &m_PipelineID);
	glBindProgramPipeline(m_PipelineID);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shdrPipeline::~shdrPipeline()
{
	glDeleteProgramPipelines(1, &m_PipelineID);
}

void shdrPipeline::AttachVS(shdrShader* i_pVS)
{
	glUseProgramStages(m_PipelineID, GL_VERTEX_SHADER_BIT, i_pVS->GetShader());
	m_pVS = i_pVS;
}
void shdrPipeline::AttachPS(shdrShader* i_pPS)
{
	glUseProgramStages(m_PipelineID, GL_FRAGMENT_SHADER_BIT, i_pPS->GetShader());
	m_pPS = i_pPS;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void shdrPipeline::Bind(oglDevice* i_pDevice)
{
	glBindProgramPipeline(m_PipelineID);
	// set shader constants here too?
}

