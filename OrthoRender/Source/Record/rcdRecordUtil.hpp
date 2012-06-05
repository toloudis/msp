/*****************************************************************************
**	rcdRecordUtil.hpp
**
**	Util to control recording mode
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef RCD_RECORDUTIL_HPP
#error rcdRecordUtil.hpp multiply included
#endif
#define RCD_RECORDUTIL_HPP

//#ifndef MODE_MODE_HPP
//#include "Support/mode/modeMode.hpp"
//#endif


namespace rcdRecordUtil
{

	//--------------------------------------------------------------------
	// SetMode
	//--------------------------------------------------------------------
	void SetMode(int /*modeModeID*/ i_RecordMode);

	//--------------------------------------------------------------------
	// StartRecording - go into recording mode and move time from
	//	begin to end time
	//--------------------------------------------------------------------
	void StartRecording(float i_BeginTime, float i_EndTime);

	//--------------------------------------------------------------------
	// StopRecording
	//--------------------------------------------------------------------
	void StopRecording();

}	// end of namespace
