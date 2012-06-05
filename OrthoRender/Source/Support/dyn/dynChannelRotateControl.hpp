/*****************************************************************************
**	dynChannelRotateControl.hpp
**
**	 Channel for rotating around an axis of a control node
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef DYN_CHANNELROTATECONTROL_HPP
#error dynChannelRotateControl.hpp multiply included
#endif
#define DYN_CHANNELROTATECONTROL_HPP


#ifndef TMLN_CHANNELRANGEDFLOAT_HPP
#include "Support/tmln/tmlnChannelRangedFloat.hpp"
#endif

class scRotateControl;

class dynChannelRotateControl : public tmlnChannelRangedFloat
{
public:
	enum Axis
	{
		e_X = 0,
		e_Y,
		e_Z
	};

	//--------------------------------------------------------------------
	// This channel does not own the control animation
	//--------------------------------------------------------------------
	dynChannelRotateControl(const char* i_Name, scRotateControl* i_pControl,
							float i_MinValue, float i_MaxValue);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dynChannelRotateControl();

	//--------------------------------------------------------------------
	// Axis controls which axis to rotate around
	//--------------------------------------------------------------------
	void SetAxis(Axis i_Axis);
	Axis GetAxis() const;

	//--------------------------------------------------------------------
	//  Set new value for channel, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetValue(float i_Val);

	//--------------------------------------------------------------------
	//  Get current value of channel
	//--------------------------------------------------------------------
	virtual float  GetValue() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline scRotateControl* GetControlAnimation();

private:
	scRotateControl* m_pControl;
	Axis m_Axis;
	float m_Value;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline scRotateControl* dynChannelRotateControl::GetControlAnimation()
{
	return m_pControl;
}
