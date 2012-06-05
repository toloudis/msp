/*****************************************************************************
**	cmraDriverMayaScript.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverMayaScript.hpp"

#include "Systems/Cameras/GUI/cmraAnimList.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScriptInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScriptParser.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/fsys/fsysFileList.hpp"
#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/api3d/api3dScale.hpp"
#include "Tool/cam3d/cam3dAnimKeys.hpp"
#include "Tool/cam3d/cam3dImport.hpp"
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	const float c_InitialCutThreshold = 1.0f;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraDriverMayaScript::cmraDriverMayaScript(tmlnChannelPosition &i_ChannelPosition, 
										   tmlnChannelPosition &i_ChannelTarget, 
										   tmlnChannelFloat &i_ChannelFov, 
										   tmlnChannelFloat &i_ChannelTilt)
:	m_ChannelPosition(i_ChannelPosition),
	m_ChannelTarget(i_ChannelTarget),
	m_ChannelFov(i_ChannelFov),
	m_ChannelTilt(i_ChannelTilt),
	m_pAnimKeys(NULL),
	m_NumFrames(0),
	m_bDisableResize(false),
	m_AnimFilename("Animation File Name"),
	m_FrameRate("Frame Rate", g3dConstants::c_fDefaultFrameRate),
	m_StartFrame("Start Frame", -1),
	m_EndFrame("End Frame", -1),
	m_bLooping("Looping", false),
	m_bDriverToAnimLength("Driver to Anim Length", true),
	m_bUseAnimStart("Use Anim Start",false),
	m_Offset("Offset", maPoint3d(0,0,0)),
	m_CutThreshold("Cut Threshold", c_InitialCutThreshold),
	m_AnimStartTime("Anim Start Time", 0)
{
	this->SetBlendType(tmlnDriver::e_Previous); //GetDefaultBlendType());

	fsysFileList file_list;
	cmraAnimList::BuildFileList(file_list);
	const int lc_ANIMCONTROL_WIDTH = 500;
	m_pAnimFilenameUIInfo = new prtyComboBoxUIInfo(&(m_AnimFilename), "Asset", "Animation filename");
	m_pAnimFilenameUIInfo->SetWidth( lc_ANIMCONTROL_WIDTH );
	for (int i=0; i<file_list.Size(); i++)
	{
		itString filename = file_list.GetFilename(i);
		m_pAnimFilenameUIInfo->AddItem( itStringUtil::GetStdString( filename ) );
	}
	AddProperty( m_pAnimFilenameUIInfo );

	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyCheckBoxUIInfo(&(m_bDriverToAnimLength), "Asset", "Resize driver length to length of animation at given frame rate");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bUseAnimStart), "Asset", "Set begin time to time exported with animation");
	AddProperty( pPUII );
	pPUII = new tmlnTimeEditUIInfo(&(m_AnimStartTime), "Asset", "Start time of animation loaded from importer");
	pPUII->SetReadOnly(true); // display only, not saved to file
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUIInfo(&(m_Offset), "Asset", "Positional offset to animation");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_FrameRate), "Playback", "Frames per second to play");
	AddProperty( pPUII );
	m_pStartFrameUIInfo = new prtyRangedFloatUIInfo(&(m_StartFrame), "Playback", "Start frame of animation segment, -1 for start of file");
	m_pStartFrameUIInfo->SetMinimum(-1.0);
	m_pStartFrameUIInfo->SetMaximum(0.0);
	m_pStartFrameUIInfo->SetReadOnly(true);
	AddProperty( m_pStartFrameUIInfo );
	m_pEndFrameUIInfo = new prtyRangedFloatUIInfo(&(m_EndFrame), "Playback", "End frame of animation segment, -1 for whole animation");
	m_pEndFrameUIInfo->SetMinimum(-1.0);
	m_pEndFrameUIInfo->SetMaximum(0.0);
	m_pEndFrameUIInfo->SetReadOnly(true);
	AddProperty( m_pEndFrameUIInfo );
	pPUII = new prtyCheckBoxUIInfo(&(m_bLooping), "Playback", "Animation loops back to start");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_CutThreshold), "Playback", "Amount of motion between frames above which to treat as a cut in the camera animation");
	AddProperty( pPUII );

	//	property callbacks	
	m_AnimFilename.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::AnimFileNameChanged));
	m_bDriverToAnimLength.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::DriverToAnimLengthChanged));
	m_bUseAnimStart.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::UseAnimStartChanged));
	m_FrameRate.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::AnimationPropertiesChanged));
	m_StartFrame.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::LengthChanged));
	m_EndFrame.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::LengthChanged));
	m_bLooping.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::AnimationPropertiesChanged));
	m_Offset.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::PropertyChanged));
	m_CutThreshold.AddCallback(new prtyCallbackWrapper<cmraDriverMayaScript>(this, &cmraDriverMayaScript::PropertyChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraDriverMayaScript::~cmraDriverMayaScript()
{
	delete m_pAnimKeys;
}

//----------------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//----------------------------------------------------------------------------
std::string cmraDriverMayaScript::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	return desc;
}

//----------------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//----------------------------------------------------------------------------
tmlnDriverInfo*  cmraDriverMayaScript::GetDriverInfo() const
{
	cmraDriverMayaScriptInfo *pInfo = new cmraDriverMayaScriptInfo(cmraDriverMayaScriptParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_AnimFilename = this->m_AnimFilename.GetValue();
	pInfo->m_FrameRate = this->m_FrameRate.GetValue();
	pInfo->m_StartFrame = this->m_StartFrame.GetValue();
	pInfo->m_EndFrame = this->m_EndFrame.GetValue();
	pInfo->m_bLooping = this->m_bLooping.GetValue();
	pInfo->m_bDriverToAnimLength = this->m_bDriverToAnimLength.GetValue();
	pInfo->m_Offset = this->m_Offset.GetValue();
	pInfo->m_CutThreshold = this->m_CutThreshold.GetValue();

	return pInfo;
}

//----------------------------------------------------------------------------
// Set internal variables from data structure
//----------------------------------------------------------------------------
void cmraDriverMayaScript::SetDriverInfo(	const cmraDriverMayaScriptInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	// Turn off the driver length flag in order to prevent 
	// multiple updates to the driver length as the anim
	// info is set.
	m_bDisableResize = true;

	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// this will load and create the animation as needed
	this->m_AnimFilename.SetValue(i_Info.m_AnimFilename, i_Undoable );
	
	this->m_FrameRate.SetValue(i_Info.m_FrameRate, i_Undoable );
	this->m_bLooping.SetValue(i_Info.m_bLooping, i_Undoable );
	this->m_StartFrame.SetValue(i_Info.m_StartFrame, i_Undoable );
	this->m_EndFrame.SetValue(i_Info.m_EndFrame, i_Undoable );
	this->m_Offset.SetValue( i_Info.m_Offset, i_Undoable );
	this->m_CutThreshold.SetValue( i_Info.m_CutThreshold, i_Undoable );

	// There are issues here about how to handle automatic resizing
	// after loading a file.  If the animation data file has changed, 
	// should the driver resize automatically on load, or is it
	// important to respect the begin and end value already written
	// to a file?
	// Because of problems with splitting in older files in version 2.9,
	// this driver is going to say that the begin and end times are the
	// important things and keep the resize disabled while doing SetData()
	//
	this->m_bDriverToAnimLength.SetValue(i_Info.m_bDriverToAnimLength, i_Undoable );

	// restore the disable resize flag
	m_bDisableResize = false;

}


//----------------------------------------------------------------------------
//  Update position of things that are being driven
//----------------------------------------------------------------------------
void  cmraDriverMayaScript::Operate(float i_Time)
{
	// Execute camera script
	if (m_pAnimKeys)
	{
		// calculate the frame
		// step method
		//
		float frame = compute_frame(i_Time);
		//float frame = (float)((int)compute_frame(i_Time));

		// Test for camera cut. 
		// - now using member variable to define the 
		//const float c_LargeMoveThreshold = 1.0f;
		//float scaled_threshold = api3dScale::Scale(c_LargeMoveThreshold);
		if (m_pAnimKeys->TestForCut(frame, m_CutThreshold.GetValue()))
		{
			// Clamp down to previous frame, making a "step"
			int stepped_frame = (int) frame;
			frame = (float) stepped_frame;
		}

		maPoint3d pos = m_pAnimKeys->GetPosition(frame);
		maVector3d view = m_pAnimKeys->GetViewVector(frame);
		this->OperateChannel( i_Time, m_ChannelPosition, pos + m_Offset.GetValue() );
		this->OperateChannel( i_Time, m_ChannelTarget, pos + view + m_Offset.GetValue() );

		//DBG_WARNING9("Frame %7.3f - %4d - %6.1f  pos(%6.3f,%6.3f,%6.3f)  view(%6.3f,%6.3f,%6.3f)", frame_orig, frame_int, frame, pos.GetX(), pos.GetY(), pos.GetZ(), view.GetX(), view.GetY(), view.GetZ() );

		//	tilt 
		if (m_pAnimKeys->HasTiltAnimation())
		{
			float tilt = m_pAnimKeys->GetTilt(frame);
			this->OperateChannel( i_Time, m_ChannelTilt, tilt );
		}

		//	field of view
		if (m_pAnimKeys->HasFieldOfViewAnimation())
		{
			float fov = m_pAnimKeys->GetFieldOfView(frame);
			this->OperateChannel( i_Time, m_ChannelFov, fov );
		}
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverMayaScript::GetClipFillColor() const
{
	return maFloatRGBA( 0.5412f, 0.6392f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------
// Find out how long the animation keyframe data is in terms
// of frames. (This does not do any computation with the
// current frame rate).
//--------------------------------------------------------------------
float cmraDriverMayaScript::GetAnimationNumFrames() const
{
	return (float)m_NumFrames;
}

//--------------------------------------------------------------------
// Returns how long the animation is in seconds given the 
// frame rate and other data about this animation.
//--------------------------------------------------------------------
float cmraDriverMayaScript::GetAnimationLength() const
{
	float start_frame = (m_StartFrame.GetValue() < 0) ? 0 : m_StartFrame.GetValue();
	float end_frame = (m_EndFrame.GetValue() < 0) ? m_NumFrames : m_EndFrame.GetValue();
	float anim_len = end_frame - start_frame;

	if (m_FrameRate.GetValue() == 0)
		return 0;
	else
		return (anim_len / m_FrameRate.GetValue());
}



//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void cmraDriverMayaScript::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add( this->m_AnimFilename.GetValue() );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::SetFrameRate( float i_Val )
{
	this->m_FrameRate.SetValue( i_Val );
	this->MarkDirty();
}

//--------------------------------------------------------------------
// -1.0 is default, meaning use the beginning of the clip
//--------------------------------------------------------------------
void cmraDriverMayaScript::SetStartFrame( float i_Val )
{
	this->m_StartFrame.SetValue( i_Val );
	this->MarkDirty();
}

//--------------------------------------------------------------------
// -1.0 is default, meaning use the end of the clip
//--------------------------------------------------------------------
void cmraDriverMayaScript::SetEndFrame( float i_Val )
{
	this->m_EndFrame.SetValue( i_Val );
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::SetLooping( bool i_bSetFlag )
{
	this->m_bLooping.SetValue( i_bSetFlag );
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::SetDriverToAnimLengthFlag( bool i_bSetFlag )
{
	this->m_bDriverToAnimLength.SetValue( i_bSetFlag );
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::SetOffset(const maPoint3d& i_Offset )
{
	this->m_Offset.SetValue( i_Offset );
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	PerformSplit - Split up the details of this driver between itself
//	and the driver passed in.
//--------------------------------------------------------------------
//virtual 
void cmraDriverMayaScript::PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime )
{
	float percent = tmlnDriver::CalculatePercent( i_fTime );

	//	cast the 2nd driver and check it is the same type
	//
	cmraDriverMayaScript* pSecondDriver = dynamic_cast<cmraDriverMayaScript*>(io_pDriverAtEnd);
	DBG_ASSERT0( pSecondDriver != 0, "Cannot split with 2 different type of drivers" );

	this->m_bDriverToAnimLength.SetValue(false);
	pSecondDriver->m_bDriverToAnimLength.SetValue(false);

	//	split the custom parts of this driver
	//
	float startframe, endframe;
	startframe	= this->m_StartFrame.GetValue();
	if (startframe == -1)
	{
		startframe = 0;		// is this always true?
	}
	endframe	= this->m_EndFrame.GetValue();
	if (endframe == -1)
	{
		endframe = this->GetAnimationNumFrames();
	}

	float midframe = startframe + (endframe - startframe) * percent;
	pSecondDriver->m_StartFrame.SetValue( midframe );
	this->m_EndFrame.SetValue( midframe );

	//	set the driver times accordingly
	tmlnDriver::PerformSplit(io_pDriverAtEnd, i_fTime);
}


//----------------------------------------------------------------------------
//	Perform the operate on a single channel
//----------------------------------------------------------------------------
void cmraDriverMayaScript::OperateChannel( float i_Time, tmlnChannelPosition& i_Channel, maPoint3d& i_Goal )
{
	// Check and handle blend into driver
	//
	if (IsBefore(i_Time))
	{
		float prevtime = i_Channel.GetPreviousTime(i_Time);
		float percent = this->GetBlendAlpha( prevtime, i_Time );
		//maFunctions::Clamp( percent, 0.0f, 100.0f );

		maPoint3d pos = i_Goal*percent + i_Channel.GetPosition()*(1.0f - percent);	// linear blend
		i_Channel.SetPosition(pos);
	}
	else
	{
		// within driver range
		//
		i_Channel.SetPosition( i_Goal );
	}
}
void cmraDriverMayaScript::OperateChannel( float i_Time, tmlnChannelFloat& i_Channel, float i_Goal )
{
	if (IsBefore(i_Time))
	{
		float prevtime = i_Channel.GetPreviousTime(i_Time);
		float percent = this->GetBlendAlpha( prevtime, i_Time );
		float val = i_Goal*percent + i_Channel.GetValue()*(1.0f - percent);	// linear blend
		i_Channel.SetValue(val);
	}
	else
	{
		// within driver range
		i_Channel.SetValue( i_Goal );
	}
}

//--------------------------------------------------------------------
// Resize duration of driver to match length of animation if requested
//--------------------------------------------------------------------
void cmraDriverMayaScript::update_driver_length_from_anim_length()
{
	if (m_bDisableResize)
		return;

	// if this checkbox is checked, resize the driver length.
	//
	if (   ( m_bDriverToAnimLength.GetValue() )
		&& ( m_AnimFilename.GetValue().GetLength() > 0 ) )
	{
		float anim_duration = this->GetAnimationLength();
		this->SetEndTime( this->GetBeginTime() + anim_duration );

		// update gui
		//chnlDialogUtil::UpdateChannels();
		chnlDialogUtil::UpdateDriver(this);

		//DBG_LOG1( "driver animfull duration  %6.2f", m_Driver.GetDuration() );
	}
}


//--------------------------------------------------------------------
// Check value of animation start properties and move 
//	begin time if needed.
//--------------------------------------------------------------------
void cmraDriverMayaScript::snap_anim_start()
{
	//
	if ( m_bUseAnimStart.GetValue() )
	{
		this->SetBeginTime( this->m_AnimStartTime.GetValue() );

		// update gui
		chnlDialogUtil::UpdateDriver(this);
	}
}

//--------------------------------------------------------------------
// Compute frame of animation to use based on frame rate, 
//	start/end frame, etc.
//--------------------------------------------------------------------
float cmraDriverMayaScript::compute_frame(float i_Time)
{
	float elapsed = i_Time - this->GetBeginTime();
	float start_frame = (m_StartFrame.GetValue() < 0) ? 0.0f : m_StartFrame.GetValue();
	float cur_frame = elapsed * m_FrameRate.GetValue() + start_frame;

	float end_frame = (m_EndFrame.GetValue() < 0) ? m_NumFrames : m_EndFrame.GetValue();
	if (cur_frame > end_frame)
	{
		if (m_bLooping.GetValue())
		{
			float anim_len = end_frame - start_frame;
			if (anim_len <= 0)
				return end_frame;

			int num_cycles = int ((cur_frame - start_frame) / anim_len);
			float loop_frame = (cur_frame - (num_cycles * anim_len));
			return loop_frame;
		}
		else
		{
			// stopped at last frame
			return end_frame;
		}
	}

	return cur_frame;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::AnimFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{			
	if (m_pAnimKeys)
	{
		delete m_pAnimKeys;
		m_pAnimKeys = NULL;
		m_NumFrames = 0;
	}

	if (m_AnimFilename.GetValue().GetLength() > 0)
	{
		fsLocator anim_loc;
		fsysFileList file_list;
		cmraAnimList::BuildFileList(file_list);
		file_list.GetFilePath(m_AnimFilename.GetValue(), anim_loc);
		anim_loc.Push(m_AnimFilename.GetValue());

		float anim_fps = g3dConstants::c_fDefaultFrameRate;
		float begin_frame = 0;
		m_pAnimKeys = cam3dImport::LoadAnimation( anim_loc, anim_fps, begin_frame );

		// Get start time exported with animation data
		this->m_AnimStartTime.SetValue( (anim_fps > 0) ? (begin_frame/anim_fps) : 0 );

		std::string str = itStringUtil::GetStdString(m_AnimFilename.GetValue());
		this->SetName(str.c_str());

		if (m_pAnimKeys)
		{	
			m_NumFrames = m_pAnimKeys->GetNumFrames();
			m_FrameRate.SetValue( anim_fps );

			snap_anim_start();
			update_driver_length_from_anim_length();

			// Update our property UI Infos
			m_pStartFrameUIInfo->SetMaximum((float)m_NumFrames);
			m_pStartFrameUIInfo->SetNumTicks((short)(m_NumFrames + 1));
			m_pStartFrameUIInfo->SetReadOnly(false);
			m_pStartFrameUIInfo->UpdateControl();

			m_pEndFrameUIInfo->SetMaximum((float)m_NumFrames);
			m_pEndFrameUIInfo->SetNumTicks((short)(m_NumFrames + 1));
			m_pEndFrameUIInfo->SetReadOnly(false);
			m_pEndFrameUIInfo->UpdateControl();
		}
	}
	
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::AnimationPropertiesChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	update_driver_length_from_anim_length();
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	update_driver_length_from_anim_length();
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::UseAnimStartChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	snap_anim_start();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::LengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	//	TODO - figure out a way to UNCHECK the "driver to anim length" flag
	//	if the user resizes the driver

	update_driver_length_from_anim_length();

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverMayaScript::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
