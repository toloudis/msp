/*****************************************************************************
**	dirltChannelEnabled.cpp
**
**	 Channel to enable/disable dir lights
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "dirltChannelEnabled.hpp"

#include "dbgLog.hpp"
#include "dirltDirLightObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dirltChannelEnabled::dirltChannelEnabled(const char* i_Name, dirltDirLightObject* i_pLight)
: tmlnChannelBoolean(i_Name), m_pLight(i_pLight)
{

}
//--------------------------------------------------------------------
//  Set new state for object, needs to be implemented in
// derived class
//--------------------------------------------------------------------
void  dirltChannelEnabled::SetState(bool i_bVal)
{
	m_pLight->SetEnabled( i_bVal );
}

//--------------------------------------------------------------------
//  Get current state of object
//--------------------------------------------------------------------
bool  dirltChannelEnabled::GetState() const
{
	return m_pLight->GetEnabled();
}

