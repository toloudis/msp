/****************************************************************************\
**  shdrDefaultVS.hpp
**
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef SHDR_DEFAULTVS_HPP
#error shdrDefaultVS.hpp multiply included
#endif
#define SHDR_DEFAULTVS_HPP

#ifndef SHDR_VS_HPP
#include "Area18/shdr/shdrVS.hpp"
#endif

class oglContext;
class maMatrix4x4;

class cbVS : public shdrParams
{
public:
	void SetWorldTransform(const maMatrix4x4& i_Transform);
	void SetUVTransform(const maMatrix4x4& i_Transform);
	void SetProjectionTransform(const maMatrix4x4& i_Transform);

	float g_uvTransform[16];
	float g_worldview[16];
	float g_projection[16];
};

class shdrDefaultVS : public shdrVS
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shdrDefaultVS(oglContext* i_pDevice);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~shdrDefaultVS();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BindConstants(shdrParams* iData);

private:
	GLint mModelView;
	GLint mProjection;
	GLuint mCB;
	static char* sourceCode;

};
