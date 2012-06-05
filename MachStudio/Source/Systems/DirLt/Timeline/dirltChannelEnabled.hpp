/*****************************************************************************
**	dirltChannelEnabled.hpp
**
**	 Channel to enable/disable dir lights
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef DIRLT_CHANNELENABLED_HPP
#error dirltChannelEnabled.hpp multiply included
#endif
#define DIRLT_CHANNELENABLED_HPP


#ifndef TMLN_CHANNELBOOLEAN_HPP
#include "tmlnChannelBoolean.hpp"
#endif

//============================================================================
//============================================================================
class dirltDirLightObject;


//============================================================================
//============================================================================
class dirltChannelEnabled : public tmlnChannelBoolean
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dirltChannelEnabled(const char* i_Name, dirltDirLightObject* i_pLight);

	//--------------------------------------------------------------------
	//  Set new state for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetState(bool i_Val);

	//--------------------------------------------------------------------
	//  Get current state of object
	//--------------------------------------------------------------------
	virtual bool  GetState() const;

private:
	dirltDirLightObject* m_pLight;
};
