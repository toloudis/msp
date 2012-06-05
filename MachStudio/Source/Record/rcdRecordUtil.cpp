/*****************************************************************************
**	rcdRecordUtil.cpp
**
**	Util to control recording mode
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Record/rcdRecordUtil.hpp"

#include "Support/mode/modeModeMgr.hpp"
#include "Record/rcdModeRecord.hpp"



namespace rcdRecordUtil
{

	namespace
	{

		modeModeID l_RecordMode = 0;

	}	// end of namespace


	//--------------------------------------------------------------------
	// SetMode
	//--------------------------------------------------------------------
	void SetMode(int /*modeModeID*/ i_RecordMode)
	{
		l_RecordMode = i_RecordMode;
	}

	//--------------------------------------------------------------------
	// StartRecording - go into recording mode and move time from
	//	begin to end time
	//--------------------------------------------------------------------
	void StartRecording(float i_BeginTime, float i_EndTime)
	{
		modeModeMgr::Push(l_RecordMode);

		modeMode* pMode = modeModeMgr::GetMode(l_RecordMode);
		rcdModeRecord* pRecMode = dynamic_cast<rcdModeRecord*>( pMode );
		pRecMode->Play(i_BeginTime, i_EndTime);
	}


	//--------------------------------------------------------------------
	// StopRecording
	//--------------------------------------------------------------------
	void StopRecording()
	{
		modeModeMgr::Pop();
	}

}	// end of namespace
