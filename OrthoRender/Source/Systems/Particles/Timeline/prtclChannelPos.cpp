/*****************************************************************************
**	prtclChannelPos.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Timeline/prtclChannelPos.hpp"

#include "Core/prty/prtyPoint3d.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclChannelPos::prtclChannelPos(const char* i_Name, prtyPoint3d& i_Property)
:	tmlnChannelPosition(i_Name),
	m_Property(i_Property)
{
}

//--------------------------------------------------------------------
//  Set new position for object, needs to be implemented in
// derived class
//--------------------------------------------------------------------
void  prtclChannelPos::SetPosition(const maPoint3d &i_Pos)
{
	m_Property.SetValue(i_Pos);
}

//--------------------------------------------------------------------
//  Get position of object, for blending with current value
//--------------------------------------------------------------------
maPoint3d  prtclChannelPos::GetPosition() const
{
	return m_Property.GetValue();
}
