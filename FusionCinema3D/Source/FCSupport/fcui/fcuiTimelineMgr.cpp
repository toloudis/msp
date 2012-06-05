/*****************************************************************************
**	fcuiTimelineMgr.cpp
**
**		See .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"

#include "Features/Playback/plbkModePlayback.hpp"
#include "Features/Playback/plbkPackage.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fcuiTimelineMgr::fcuiTimelineMgr()
:   m_bPlaying(false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fcuiTimelineMgr::~fcuiTimelineMgr()
{}

//----------------------------------------------------------------------------
/// Return the current instance of the timeline mgr singleton
//----------------------------------------------------------------------------
fcuiTimelineMgr* fcuiTimelineMgr::Instance = 0;

//----------------------------------------------------------------------------
/// Set the start time of the timeline
//----------------------------------------------------------------------------
void fcuiTimelineMgr::SetEndTime(const maTime& i_EndTime)
{
	m_EndTime = i_EndTime;
}

//----------------------------------------------------------------------------
/// Get the time value of the end of the timeline
//----------------------------------------------------------------------------
const maTime& fcuiTimelineMgr::GetEndTime() const
{
	return m_EndTime;
}

//----------------------------------------------------------------------------
/// Return whether or not we are playing the timeline
//----------------------------------------------------------------------------
bool fcuiTimelineMgr::IsPlaying()
{
	plbkModePlayback* pMode = GetPlaybackMode();
	return pMode->IsPlaying();
}	

//----------------------------------------------------------------------------
/// Go to the beginning
//----------------------------------------------------------------------------
void fcuiTimelineMgr::Begin()
{
	plbkModePlayback* pMode = GetPlaybackMode();
	pMode->SetLooping(false);
	//pMode->GoBegin();
	pMode->GoBegin();
}
//----------------------------------------------------------------------------
///Go to the end
//----------------------------------------------------------------------------
void fcuiTimelineMgr::End()
{
	plbkModePlayback* pMode = GetPlaybackMode();
	pMode->SetLooping(false);
	//pMode->GoBegin();
	pMode->GoEnd();
}

//----------------------------------------------------------------------------
/// Currently plays the timeline from the beginning
//----------------------------------------------------------------------------
void fcuiTimelineMgr::Play()
{
	plbkModePlayback* pMode = GetPlaybackMode();
	pMode->SetLooping(false);
	//pMode->GoBegin();
	pMode->Play();
}

//----------------------------------------------------------------------------
/// Ends playback mode
//----------------------------------------------------------------------------
void fcuiTimelineMgr::Pause()
{
	plbkModePlayback* pMode = GetPlaybackMode();
	pMode->Pause();
	pMode->ExitMode();
}

//----------------------------------------------------------------------------
/// Does a fast reverse
//----------------------------------------------------------------------------
void fcuiTimelineMgr::Rewind()
{
	plbkModePlayback* pMode = GetPlaybackMode();
	pMode->FastRev();
}

//----------------------------------------------------------------------------
/// fast forward
//----------------------------------------------------------------------------
void fcuiTimelineMgr::FastFwd()
{
	plbkModePlayback* pMode = GetPlaybackMode();
	pMode->FastFwd();
}

//----------------------------------------------------------------------------
/// Increment the end time of the timeline
//----------------------------------------------------------------------------
void fcuiTimelineMgr::IncrementTime(float i_IncrementAmount)
{
	maTime curTime = tmlnTimeLine::GetValue();
	//curTime += (i_IncrementAmount / tmlnTimeLine::GetFPS());	
	curTime += maTime::FromFrame(i_IncrementAmount, tmlnTimeLine::GetFPS());	//TIME
	if(curTime < tmlnTimeLine::GetMaximum())
		tmlnTimeLine::SetValue(curTime);
}

//------------------------------------------------------------------------
/// Decrement the end time of the timeline
//------------------------------------------------------------------------
void fcuiTimelineMgr::DecrementTime(float i_DecrementAmount)
{
	maTime curTime = tmlnTimeLine::GetValue();
	//curTime -= (i_DecrementAmount / tmlnTimeLine::GetFPS());	
	curTime -= maTime::FromFrame(i_DecrementAmount, tmlnTimeLine::GetFPS());	//TIME
	if(curTime > tmlnTimeLine::GetMinimum())
		tmlnTimeLine::SetValue(curTime);
}

//----------------------------------------------------------------------------
/// Get the current playback time of the timeline
//----------------------------------------------------------------------------
const maTime& fcuiTimelineMgr::GetCurrentTime() const
{
	return m_CurrentTime;
}

//----------------------------------------------------------------------------
/// Set the current playback time of the timeline
//----------------------------------------------------------------------------
void fcuiTimelineMgr::SetCurrentTime(const maTime& i_NewTime)
{
	m_CurrentTime = i_NewTime;
}

//----------------------------------------------------------------------------
/// Set the playback mode as the current mode on the mode stack
//----------------------------------------------------------------------------
void fcuiTimelineMgr::LoadPlaybackMode()
{
	if( !modeModeMgr::IsInStack( plbkPackage::GetModePlaybackID() ))
		modeModeMgr::Push( plbkPackage::GetModePlaybackID() );

	modeModeMgr::SetCurrentMode( plbkPackage::GetModePlaybackID() );
}

//----------------------------------------------------------------------------
/// Return the pointer to the playback mode
//----------------------------------------------------------------------------
plbkModePlayback* fcuiTimelineMgr::GetPlaybackMode()
{
	modeMode* pMode = modeModeMgr::GetMode( plbkPackage::GetModePlaybackID() );
	DBG_ASSERT( pMode != 0, "No playback mode" );
	plbkModePlayback *pActualMode = dynamic_cast<plbkModePlayback*>(pMode);
	DBG_ASSERT( pActualMode != 0, "mode ID is not the mode" );

	LoadPlaybackMode();
	
	return pActualMode;
}
