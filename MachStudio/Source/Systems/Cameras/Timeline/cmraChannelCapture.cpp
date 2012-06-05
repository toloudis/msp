/*****************************************************************************
**	cmraChannelCapture.cpp
**
**	 Channel to enable/disable dir lights
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Timeline/cmraChannelCapture.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCapture.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraChannelCapture::cmraChannelCapture(const char* i_Name)//, cmraCameraObject* i_pCamera)
:	tmlnChannelBoolean(i_Name)//, 
//	m_pCamera(i_pCamera)
{

}

//--------------------------------------------------------------------
//  Set new state for object, needs to be implemented in
// derived class
//--------------------------------------------------------------------
void  cmraChannelCapture::SetState(bool i_bVal)
{
}

//--------------------------------------------------------------------
//  Get current state of object
//--------------------------------------------------------------------
bool  cmraChannelCapture::GetState() const
{
	return true;
}

//--------------------------------------------------------------------
//	Get a list of all the cameras and their in/out times
//--------------------------------------------------------------------
void cmraChannelCapture::GetInOutTimes(tmlnTimeInOutDataList& o_List)
{
	int i, count;
	count = GetNumDrivers();
	for (i=0; i<count; ++i)
	{
		cmraDriverCapture* pDriver = dynamic_cast<cmraDriverCapture*>(&Driver(i));
		if (pDriver != 0)
		{
			//DBG_LOG3("in-out #%02d begin %6.3f end %6.3f", i, pDriver->GetBeginTime(), pDriver->GetEndTime() );

			o_List.AddTimeInOut( pDriver->GetBeginTime(),
								 pDriver->GetEndTime() );
		}
	}
}


