/*****************************************************************************
**	dcutChannelCamera.hpp
**
**	 Channel to script the cuts from one camera to the other
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_CHANNELCAMERA_HPP
#error dcutChannelCamera.hpp multiply included
#endif
#define DCUT_CHANNELCAMERA_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif


//============================================================================
//============================================================================
class dcutDirectorsCutObject;


//============================================================================
//============================================================================
class dcutChannelCamera : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dcutChannelCamera(const char* i_Name, dcutDirectorsCutObject* i_pObject);

	//--------------------------------------------------------------------
	//  Set camera to use by index
	//--------------------------------------------------------------------
	void  SetCameraIndex(int i_Index);

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();


private:
	dcutDirectorsCutObject* m_pObject;
};
