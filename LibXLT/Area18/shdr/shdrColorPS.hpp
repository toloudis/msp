/****************************************************************************\
**  shdrColorPS.hpp
**
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef SHDR_COLORPS_HPP
#error shdrColorPS.hpp multiply included
#endif
#define SHDR_COLORPS_HPP

#ifndef SHDR_PS_HPP
#include "Area18/shdr/shdrPS.hpp"
#endif

class oglContext;
class maFloatRGBA;

class shdrColorPS : public shdrPS
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	shdrColorPS(oglContext* i_pDevice);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~shdrColorPS();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BindConstants(shdrParams* iData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetColor(const maFloatRGBA& i_Color);


private:
	struct cb 
	{
		float g_Color[4];
	};
	cb m_Data;
	GLuint mCB;
};
