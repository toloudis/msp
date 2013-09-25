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

	shdrPipeline(const shdrPipeline&);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~shdrPipeline();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Bind(oglDevice* i_pDevice);
	virtual void Unbind();

	void AttachVS(shdrShader* i_pVS);
	void AttachPS(shdrShader* i_pPS);

	shdrShader* vs() const {return m_pVS;}
	shdrShader* ps() const {return m_pPS;}

	void activateVs() const;
	void activatePs() const;
protected:
	GLuint m_PipelineID;

	// TODO: use shared_ptr here!!!
	shdrShader* m_pVS;
	shdrShader* m_pPS;
};
