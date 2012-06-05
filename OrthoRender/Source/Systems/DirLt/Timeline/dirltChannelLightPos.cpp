/*****************************************************************************
**	dirltChannelLightPos.cpp
**
**	 Channel for altering position of dir lights
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "dirltChannelLightPos.hpp"

#include "dbgLog.hpp"
#include "dirltDirLightObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dirltChannelLightPos::dirltChannelLightPos(const char* i_Name, dirltDirLightObject* i_pLight)
: tmlnChannelPosition(i_Name), m_pLight(i_pLight)
{

}

//--------------------------------------------------------------------
//  Set new position for object, needs to be implemented in
// derived class
//--------------------------------------------------------------------
void  dirltChannelLightPos::SetPosition(const maPoint3d &i_Pos)
{
//	m_pLight->SetPosition(i_Pos);

	//DBG_LOG3( "light(%6.3f,%6.3f,%6.3f)", i_Pos.GetX(), i_Pos.GetY(), i_Pos.GetZ() );
}

//--------------------------------------------------------------------
//  Get position of object, for blending with current value
//--------------------------------------------------------------------
maPoint3d  dirltChannelLightPos::GetPosition() const
{
	return maPoint3d(0,0,0);
	//return m_pLight->GetPosition();
}
