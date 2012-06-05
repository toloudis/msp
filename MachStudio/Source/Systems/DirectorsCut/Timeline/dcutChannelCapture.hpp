/*****************************************************************************
**	dcutChannelCapture.hpp
**
**	 Channel to enable/disable dir Cameras
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_CHANNELCAPTURE_HPP
#error dcutChannelCapture.hpp multiply included
#endif
#define DCUT_CHANNELCAPTURE_HPP

#ifndef TMLN_CHANNELBOOLEAN_HPP
#include "Support/tmln/tmlnChannelBoolean.hpp"
#endif
#ifndef TMLN_TIMEINOUTDATA_HPP
#include "Support/tmln/tmlnTimeInOutData.hpp"
#endif


//============================================================================
//============================================================================
class dcutDirectorsCutObject;


//============================================================================
//============================================================================
class dcutChannelCapture : public tmlnChannelBoolean
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dcutChannelCapture(const char* i_Name);//, dcutDirectorsCutObject* i_pCamera);

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
	//dcutDirectorsCutObject* m_pCamera;
};
