/*****************************************************************************
**	dirltChannelColor.hpp
**
**	 Channel represents color of dir lights
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef DIRLT_CHANNELCOLOR_HPP
#error dirltChannelColor.hpp multiply included
#endif
#define DIRLT_CHANNELCOLOR_HPP


#ifndef TMLN_CHANNELCOLOR_HPP
#include "tmlnChannelColor.hpp"
#endif


//============================================================================
//============================================================================
class dirltDirLightObject;


//============================================================================
//============================================================================
class dirltChannelColor : public tmlnChannelColor
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dirltChannelColor(const char* i_Name, dirltDirLightObject* i_pLight);

	//--------------------------------------------------------------------
	//  Set new color for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	//  Get color of object, for blending with current value
	//--------------------------------------------------------------------
	virtual maFloatRGBA  GetColor() const;

private:
	dirltDirLightObject* m_pLight;
};
