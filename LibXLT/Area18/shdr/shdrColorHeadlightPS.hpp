#pragma once

#include "Area18/ogl/oglTexture2d.h"

#include "Area18/shdr/shdrPS.hpp"

class oglContext;
class maFloatRGBA;
class maVector4d;

class shdrColorHeadlightPS : public shdrPS
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shdrColorHeadlightPS(oglContext* i_pDevice);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~shdrColorHeadlightPS();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BindConstants(shdrParams* iData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetColor(const maFloatRGBA& i_Color);
	void SetEyePos(const maVector4d& i_E);
	void SetTexture(oglTexture2dHandle iTex);

private:
	struct cb 
	{
		float g_Color[4];
		float g_EyePos[4];
		bool g_HasDiffuseTexture;
	};
	cb m_Data;
	GLuint mCB;
	oglTexture2dHandle mDiffuseTexture;
	GLuint mDiffuseSampler;
};
