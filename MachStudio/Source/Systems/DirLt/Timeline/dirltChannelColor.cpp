/*****************************************************************************
**	dirltChannelColor.cpp
**
**	 Channel represents color of dir lights
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "dirltChannelColor.hpp"

#include "dbgLog.hpp"
#include "dirltDirLightObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dirltChannelColor::dirltChannelColor(const char* i_Name, dirltDirLightObject* i_pLight)
: tmlnChannelColor(i_Name), m_pLight(i_pLight)
{

}

//--------------------------------------------------------------------
//  Set new color for object, needs to be implemented in
// derived class
//--------------------------------------------------------------------
void  dirltChannelColor::SetColor(const maFloatRGBA &i_Color)
{
	m_pLight->SetIntensity(i_Color);
}

//--------------------------------------------------------------------
//  Get color of object, for blending with current value
//--------------------------------------------------------------------
maFloatRGBA  dirltChannelColor::GetColor() const
{
	return m_pLight->GetIntensity();
}

