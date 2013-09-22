/****************************************************************************\
**  shdrVS.hpp
**
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#pragma once

#include "Area18/ogl/oglTypes.hpp"

class oglDevice;
class shdrShader;

class shdrPipeline
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shdrPipeline();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~shdrPipeline();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Bind(oglDevice* i_pDevice);

	void AttachVS(shdrShader* i_pVS);
	void AttachPS(shdrShader* i_pPS);

	shdrShader* vs() {return m_pVS;}
	shdrShader* ps() {return m_pPS;}
protected:
	GLuint m_PipelineID;

	shdrShader* m_pVS;
	shdrShader* m_pPS;
};
