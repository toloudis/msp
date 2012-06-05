/*****************************************************************************
**	prtclDriverAnimation.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Timeline/prtclDriverAnimation.hpp"

#include "Systems/Particles/Timeline/prtclChannelAnimation.hpp"
#include "Systems/Particles/Timeline/prtclDriverAnimationInfo.hpp"
#include "Systems/Particles/Timeline/prtclDriverAnimationParser.hpp"
//#include "prtclDriverAnimationForm.h"
#include "Systems/Particles/GUI/prtclAnimList.hpp"

// Unfortunate include out to Features library
#include "Features/Channels/chnlDialogUtil.hpp"

#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/prt/prtImport.hpp"
#include "Graphics/prt/prtVertexAnimation.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclDriverAnimation::prtclDriverAnimation(prtclChannelAnimation &i_Channel, 
												 chDefs::Name i_ChunkName)
:	m_Channel(i_Channel),
	m_ChunkName(i_ChunkName),
	m_pAnimation(NULL),
	m_ParticleFrames(NULL),
	m_bDriverToAnimLength("Driver to Anim Length",true),
	m_bUseAnimStart("Use Anim Start",false),
	m_AnimFilename("Anim FileName"),
	m_AnimName("Anim Name"),
	m_FrameRate("Frame Rate",g3dConstants::c_fDefaultFrameRate),
	m_StartFrame("Start Frame",-1),			// -1.0 is default / not set
	m_EndFrame("End Frame",-1),				// -1.0 is default / not set
	m_bLooping("Looping",false),
	m_AnimStartTime("Anim Start Time", 0)
{
	m_pAnimFilenameUIInfo = new prtyComboBoxUIInfo(&(m_AnimFilename), "Asset", "Animation filename");
	AddProperty( m_pAnimFilenameUIInfo );
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_AnimName), "Asset", "Display name for animation");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bDriverToAnimLength), "Asset", "Resize driver length to length of animation at given frame rate");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bUseAnimStart), "Asset", "Set begin time to time exported with animation");
	AddProperty( pPUII );
	pPUII = new tmlnTimeEditUIInfo(&(m_AnimStartTime), "Asset", "Start time of animation loaded from importer");
	pPUII->SetReadOnly(true); // display only, not saved to file
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

	// Register callbacks to update dirty bit when properties change
	m_AnimFilename.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::AnimFileNameChanged));
	m_AnimName.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::AnimNameChanged));
	m_bDriverToAnimLength.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::DriverToAnimLengthChanged));
	m_bUseAnimStart.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::UseAnimStartChanged));
	m_FrameRate.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::FrameRateChanged));
	m_StartFrame.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::AnimationPropertiesChanged));
	m_EndFrame.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::AnimationPropertiesChanged));
	m_bLooping.AddCallback(new prtyCallbackWrapper<prtclDriverAnimation>(this, &prtclDriverAnimation::AnimationPropertiesChanged));

	// Set up the
	fsysFileList fileList;
	prtclAnimList::BuildFileList(fileList);
	const bool bUpdateControl = false; // couldn't have created the control yet
	this->SetAnimationList(fileList, bUpdateControl);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclDriverAnimation::~prtclDriverAnimation()
{
//	DBG_LOG("prtclDriverAnimation: destroying animation");
	m_ParticleKeys.clear();
	envSTLHelpers::DeleteContainer(m_ParticleFrames);
	delete m_pAnimation;
	m_pAnimation = NULL;
}

//--------------------------------------------------------------------
//	This should be called to set the list of animation files
//	possible for this driver.
//--------------------------------------------------------------------
void prtclDriverAnimation::SetAnimationList(const fsysFileList& i_FileList,
											   bool i_bUpdateControl)
{
	m_pAnimFilenameUIInfo->ClearItems();
	for (int i=0; i<i_FileList.Size(); i++)
	{
		itString filename = i_FileList.GetFilename(i);
		m_pAnimFilenameUIInfo->AddItem( itStringUtil::GetStdString( filename ) );
	}
	if (i_bUpdateControl)
		m_pAnimFilenameUIInfo->UpdateControl();
}

//--------------------------------------------------------------------
//	Find the full locator for the given animation filename.
//--------------------------------------------------------------------
fsLocator prtclDriverAnimation::GetAnimationLocator( const itString &i_AnimFilename ) const
{
	fsLocator anim_loc;
	fsysFileList file_list;
	prtclAnimList::BuildFileList(file_list);
	if (file_list.FindFilePath(i_AnimFilename, anim_loc))
		anim_loc.Push(i_AnimFilename);
	return anim_loc;
}


//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void  prtclDriverAnimation::Operate(float i_Time)
{
	// No blending, just a set when within the particle driver's range
	if (IsWithin(i_Time))
	{
		m_Channel.SetAnimation( m_pAnimation, this->GetBeginTime() );
	}
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  prtclDriverAnimation::GetDriverInfo() const
{
	prtclDriverAnimationInfo *pInfo = new prtclDriverAnimationInfo(m_ChunkName);
	this->GetBaseDriverInfo(*pInfo);

	int anim_index = 0;

	pInfo->m_Info.m_AnimFilename	= m_AnimFilename.GetValue();
	pInfo->m_Info.m_AnimName		= m_AnimName.GetValue();
	pInfo->m_Info.m_bDriverToAnimLength = m_bDriverToAnimLength.GetValue();

	pInfo->m_Info.m_FrameRate		= m_FrameRate.GetValue();
	pInfo->m_Info.m_bLooping		= m_bLooping.GetValue();
	pInfo->m_Info.m_StartFrame		= m_StartFrame.GetValue();
	pInfo->m_Info.m_EndFrame		= m_EndFrame.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void prtclDriverAnimation::SetDriverInfo(const prtclDriverAnimationInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// this will load and create the animation as needed
	this->m_AnimFilename.SetValue(i_Info.m_Info.m_AnimFilename, i_Undoable );
	
	this->m_AnimName.SetValue(i_Info.m_Info.m_AnimName, i_Undoable );
	this->m_bDriverToAnimLength.SetValue(i_Info.m_Info.m_bDriverToAnimLength, i_Undoable );
	this->m_FrameRate.SetValue(i_Info.m_Info.m_FrameRate, i_Undoable );
	this->m_bLooping.SetValue(i_Info.m_Info.m_bLooping, i_Undoable );
	this->m_StartFrame.SetValue(i_Info.m_Info.m_StartFrame, i_Undoable );
	this->m_EndFrame.SetValue(i_Info.m_Info.m_EndFrame, i_Undoable );
}

//--------------------------------------------------------------------
// Find out how long the animation keyframe data is in terms
// of frames. (This does not do any computation with the
// current frame rate).
//--------------------------------------------------------------------
float prtclDriverAnimation::GetAnimationNumFrames() const
{
	if (m_pAnimation)
	{
		return m_pAnimation->GetNumFrames();
	}
	return 0;
}

//--------------------------------------------------------------------
// Returns how long the animation is in seconds given the 
// frame rate and other data about this animation.
//--------------------------------------------------------------------
float prtclDriverAnimation::GetAnimationLength() const
{
	if (m_pAnimation)
	{
		return m_pAnimation->GetLength();
	}
	return 0;
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void prtclDriverAnimation::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add( this->m_AnimFilename.GetValue() );
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA prtclDriverAnimation::GetClipFillColor() const
{
	return maFloatRGBA( 1.0f, 0.5412f, 0.7569f, 1.0f );
}

//--------------------------------------------------------------------
//	PerformSplit - Split up the details of this driver between itself
//	and the driver passed in.
//--------------------------------------------------------------------
//virtual 
void prtclDriverAnimation::PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime )
{
	float percent = tmlnDriver::CalculatePercent( i_fTime );

	//	cast the 2nd driver and check it is the same type
	//
	prtclDriverAnimation* pSecondDriver = dynamic_cast<prtclDriverAnimation*>(io_pDriverAtEnd);
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
		endframe = m_pAnimation->GetNumFrames();
	}

	float midframe = startframe + (endframe - startframe) * percent;
	pSecondDriver->m_StartFrame.SetValue( midframe );
	this->m_EndFrame.SetValue( midframe );

	//	set the driver times accordingly
	tmlnDriver::PerformSplit(io_pDriverAtEnd, i_fTime);
}

//--------------------------------------------------------------------
// Set name of driver from filename or anim name
//--------------------------------------------------------------------
void prtclDriverAnimation::check_driver_name()
{
	if (m_AnimName.GetValue().size() > 0)
		this->SetName(m_AnimName.GetValue().c_str());
	else if (m_AnimFilename.GetValue().GetLength() > 0)
	{
		std::string str = itStringUtil::GetStdString(m_AnimFilename.GetValue());
		this->SetName(str.c_str());
	}
}

//--------------------------------------------------------------------
// Check value of animation start properties and move 
//	begin time if needed.
//--------------------------------------------------------------------
void prtclDriverAnimation::snap_anim_start()
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
// Resize duration of driver to match length of animation if requested
//--------------------------------------------------------------------
void prtclDriverAnimation::update_driver_length_from_anim_length()
{
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

		//DBG_LOG1( "driver anim duration  %6.2f", m_Driver.GetDuration() );
	}
}

//--------------------------------------------------------------------
// return true if blending and if true, return amount blending time
//--------------------------------------------------------------------
bool prtclDriverAnimation::get_blend_time(float i_Time, float &o_BlendTime)
{
	switch (this->GetBlendType())
	{
		default:
		case tmlnDriver::e_NoBlending:
			return false;
		case tmlnDriver::e_Overwrite:
		{
			o_BlendTime = 0.0f;
			return true;
		}
		case tmlnDriver::e_BlendTime:
		{
			o_BlendTime = this->GetBlendTime();
			return (this->GetBeginTime() - i_Time <= o_BlendTime);
		}
		case tmlnDriver::e_Previous:
		case tmlnDriver::e_SmoothBlend:
		{
			float begin_time = this->GetBeginTime();
			float prev_time = m_Channel.GetPreviousTime(begin_time);
			o_BlendTime = begin_time - prev_time;
			return (begin_time - i_Time <= o_BlendTime);
		}
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDriverAnimation::AnimFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{			
	// Clear out animation in channel so that it doesn't use the animation
	// we are about to delete anymore.
	m_Channel.Reset();

	m_ParticleKeys.clear();
	envSTLHelpers::DeleteContainer(m_ParticleFrames);
	delete m_pAnimation;
	m_pAnimation = NULL;

	// This should assert if animation file is not found.
	fsLocator anim_loc = this->GetAnimationLocator( m_AnimFilename.GetValue() );

	// Load in animation data
	float anim_fps = g3dConstants::c_fDefaultFrameRate;
	float begin_frame = 0;
	prtImport::LoadVertexAnimation( anim_loc,
						m_ParticleKeys,
						m_ParticleFrames,
						anim_fps,
						begin_frame);

	// Get start time exported with animation data
	this->m_AnimStartTime.SetValue( (anim_fps > 0) ? (begin_frame/anim_fps) : 0 );

	if (!m_ParticleKeys.empty())
	{
		// Create animation and give it to particle generator
		m_pAnimation = new prtVertexAnimation(m_ParticleKeys[0], anim_fps);

		DBG_ASSERT0( m_pAnimation != 0, "CreateAnimation failed" );

		//std::string tempstr2;
		//fsFileUtil::LocatorToANSIFilename(anim_loc, tempstr2);
		//DBG_LOG3( "--Loaded and Created Animation %d-%d - %s", i, j, tempstr2.c_str() );

		// Instead of setting the animation's frame rate here, 
		// set our frame rate property from the animation's frame rate
		// which was written when the animation was exported. 
		// If this is coming from SetData(), then the existing
		// frame rate will overwrite it, but if this is a new animation
		// then we will use the desired frame rate automagically.
		m_FrameRate.SetValue( m_pAnimation->GetFrameRate() );

		// Set the values we have already into the new animation
		//m_pAnimation->SetFrameRate( m_FrameRate.GetValue() );
		m_pAnimation->SetLooping( m_bLooping.GetValue());
		m_pAnimation->SetStartFrame( m_StartFrame.GetValue() );
		m_pAnimation->SetEndFrame( m_EndFrame.GetValue() );

		check_driver_name();
		snap_anim_start();
		update_driver_length_from_anim_length();

		// Update our property UI Infos
		float num_frames = this->GetAnimationNumFrames();
		m_pStartFrameUIInfo->SetMaximum(num_frames);
		m_pStartFrameUIInfo->SetNumTicks((short)(num_frames + 1));
		m_pStartFrameUIInfo->SetReadOnly(false);
		m_pStartFrameUIInfo->UpdateControl();

		m_pEndFrameUIInfo->SetMaximum(num_frames);
		m_pEndFrameUIInfo->SetNumTicks((short)(num_frames + 1));
		m_pEndFrameUIInfo->SetReadOnly(false);
		m_pEndFrameUIInfo->UpdateControl();
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDriverAnimation::AnimNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	check_driver_name();
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDriverAnimation::FrameRateChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	if (m_pAnimation)
	{
		m_pAnimation->SetFrameRate( m_FrameRate.GetValue() );
		update_driver_length_from_anim_length();
	}
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDriverAnimation::AnimationPropertiesChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	if (m_pAnimation)
	{
		m_pAnimation->SetLooping( m_bLooping.GetValue() );
		m_pAnimation->SetStartFrame( m_StartFrame.GetValue() );
		m_pAnimation->SetEndFrame( m_EndFrame.GetValue() );
		update_driver_length_from_anim_length();
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDriverAnimation::DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	update_driver_length_from_anim_length();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDriverAnimation::UseAnimStartChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	snap_anim_start();
}
