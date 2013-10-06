/****************************************************************************\
**  shdrVS.hpp
**
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#pragma once

#include "Area18/ogl/oglTypes.hpp"
#include "Area18/shdr/shdrShader.hpp"

class oglDevice;

class shdrPipeline
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shdrPipeline();

	shdrPipeline(const shdrPipeline&);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~shdrPipeline();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Bind(oglDevice* i_pDevice);
	virtual void Unbind();

	void AttachVS(shdrShaderPtr i_pVS);
	void AttachPS(shdrShaderPtr i_pPS);

	shdrShaderPtr vs() const {return m_pVS;}
	shdrShaderPtr ps() const {return m_pPS;}

	void activateVs() const;
	void activatePs() const;
protected:
	GLuint m_PipelineID;

	// TODO: use shared_ptr here!!!
	shdrShaderPtr m_pVS;
	shdrShaderPtr m_pPS;
};
