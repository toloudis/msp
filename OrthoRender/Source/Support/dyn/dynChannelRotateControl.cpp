/*****************************************************************************
**	dynChannelRotateControl.cpp
**
**	 Channel for rotating around an axis of a control node
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Support/dyn/dynChannelRotateControl.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/sc/scRotateControl.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynChannelRotateControl::dynChannelRotateControl(const char* i_Name,
			scRotateControl* i_pControl, float i_MinValue, float i_MaxValue)
: tmlnChannelRangedFloat(i_Name, i_MinValue, i_MaxValue),
	m_pControl(i_pControl), m_Axis(e_X)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynChannelRotateControl::~dynChannelRotateControl()
{
	// don't delete rotate control, it is owned by the scObject it
	// is attached to
}

//--------------------------------------------------------------------
// Axis controls which axis to rotate around
//--------------------------------------------------------------------
void dynChannelRotateControl::SetAxis(Axis i_Axis)
{
	m_Axis = i_Axis;
}
dynChannelRotateControl::Axis dynChannelRotateControl::GetAxis() const
{
	return m_Axis;
}

//--------------------------------------------------------------------
//  Set new value for channel, needs to be implemented in
// derived class
//--------------------------------------------------------------------
void  dynChannelRotateControl::SetValue(float i_Val)
{
	m_Value = i_Val;

	float radians = maConstants::c_fAngleToRad * i_Val;

	switch (m_Axis)
	{
	default:
	case e_X:
		m_pControl->SetRotation(maRotation(radians,0,0));
		break;
	case e_Y:
		m_pControl->SetRotation(maRotation(0,radians,0));
		break;
	case e_Z:
		m_pControl->SetRotation(maRotation(0,0,radians));
		break;
	}
}

//--------------------------------------------------------------------
//  Get current value of channel
//--------------------------------------------------------------------
float  dynChannelRotateControl::GetValue() const
{
	return m_Value;
}
