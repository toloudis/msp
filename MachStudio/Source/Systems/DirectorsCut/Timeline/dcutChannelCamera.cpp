/*****************************************************************************
**	dcutChannelCamera.cpp
**
**	 Channel to enable/disable dir lights
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Timeline/dcutChannelCamera.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutChannelCamera::dcutChannelCamera(const char* i_Name, dcutDirectorsCutObject* i_pObject)
:	tmlnChannel(i_Name), 
	m_pObject(i_pObject)
{

}

//--------------------------------------------------------------------
//  Set camera to use by index
//--------------------------------------------------------------------
void  dcutChannelCamera::SetCameraIndex(int i_Index)
{
	m_pObject->SetCameraIndex(i_Index);
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void dcutChannelCamera::Reset()
{
	this->SetCameraIndex(-1);
}



