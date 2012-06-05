/*****************************************************************************
**	cmraChannelCapture.hpp
**
**	 Channel to enable/disable dir Cameras
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_CHANNELCAPTURE_HPP
#error cmraChannelCapture.hpp multiply included
#endif
#define CMRA_CHANNELCAPTURE_HPP

#ifndef TMLN_CHANNELBOOLEAN_HPP
#include "Support/tmln/tmlnChannelBoolean.hpp"
#endif
#ifndef TMLN_TIMEINOUTDATA_HPP
#include "Support/tmln/tmlnTimeInOutData.hpp"
#endif


//============================================================================
//============================================================================
class cmraCameraObject;


//============================================================================
//============================================================================
class cmraChannelCapture : public tmlnChannelBoolean
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraChannelCapture(const char* i_Name);//, cmraCameraObject* i_pCamera);

	//--------------------------------------------------------------------
	//  Set new state for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetState(bool i_Val);

	//--------------------------------------------------------------------
	//  Get current state of object
	//--------------------------------------------------------------------
	virtual bool  GetState() const;

	//--------------------------------------------------------------------
	//	Get a list of all the cameras and their in/out times
	//--------------------------------------------------------------------
	void GetInOutTimes(tmlnTimeInOutDataList& o_List);

private:
	//cmraCameraObject* m_pCamera;
};
