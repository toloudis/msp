/*****************************************************************************
**	tmlnDriverAnimationFull.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Animation/tmlnDriverAnimationFull.hpp"

#include "Drivers/Animation/tmlnDriverAnimationFullInfo.hpp"
#include "Drivers/Animation/tmlnDriverAnimationFullParser.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"		// Unfortunate include out to Features library
#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/fsys/fsysFileList.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnChannelAnimationFull.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"

#include "Core/env/envSharedAssetMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entAnimKeys.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <assert.h>
#include <sstream>


//============================================================================
//	Shared Animation Manager
//============================================================================
namespace
{
	const bool c_UseSharedAssetMgr = true;

	class SharedAnimMgr : public envSharedAssetMgr<entAnimKeys, fsLocator>
	{
		//--------------------------------------------------------------------
		// Load Animation from filename
		//--------------------------------------------------------------------
		virtual entAnimKeys* LoadAsset(const fsLocator& i_Locator)
		{
			return entImport::LoadAnimKeys( i_Locator );
		}
	};
	SharedAnimMgr l_SharedAnimationMgr;
}


//============================================================================
//============================================================================
bool tmlnDriverAnimationFull::sm_bSkipAnimations = false;
bool tmlnDriverAnimationFull::sm_bDelayAnimations = false;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimationFull::tmlnDriverAnimationFull(tmlnChannelAnimationFull &i_Channel, 
												 chDefs::Name i_ChunkName)
:	m_Channel(i_Channel),
	m_ChunkName(i_ChunkName),
	m_pAnimation(NULL),
	m_pAnimKeys(NULL),
	m_bDriverToAnimLength("Driver to Anim Length",true),
	m_bUseAnimStart("Use Anim Start",false),
	m_AnimFilename("Animation File Name"),
	m_AnimName("Anim Name"),
	m_FrameRate("Frame Rate",g3dConstants::c_fDefaultFrameRate),
	m_StartFrame("Start Frame",-1),			// -1.0 is default / not set
	m_EndFrame("End Frame",-1),				// -1.0 is default / not set
	m_bLooping("Looping",false),
	m_AnimStartTime("Anim Start Time", 0)
{
	const int lc_ANIMCONTROL_WIDTH = 500;
	m_pAnimFilenameUIInfo = new prtyComboBoxUIInfo(&(m_AnimFilename), "Asset", "Animation filename");
	m_pAnimFilenameUIInfo->SetWidth( lc_ANIMCONTROL_WIDTH );
	AddProperty( m_pAnimFilenameUIInfo );
	prtyPropertyUIInfo* pPUII;
	prtyTextBoxUIInfo* pTBUII;
	pTBUII = new prtyTextBoxUIInfo(&(m_AnimName), "Asset", "Display name for animation");
	const int lc_ANIMTEXT_WIDTH = 150;
	pTBUII->SetMaxWidth( lc_ANIMTEXT_WIDTH );
	AddProperty( pTBUII );
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
	m_AnimFilename.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::AnimFileNameChanged));
	m_AnimName.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::AnimNameChanged));
	m_bDriverToAnimLength.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::DriverToAnimLengthChanged));
	m_bUseAnimStart.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::UseAnimStartChanged));
	m_FrameRate.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::FrameRateChanged));
	m_StartFrame.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::LengthChanged));
	m_EndFrame.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::LengthChanged));
	m_bLooping.AddCallback(new prtyCallbackWrapper<tmlnDriverAnimationFull>(this, &tmlnDriverAnimationFull::AnimationPropertiesChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimationFull::~tmlnDriverAnimationFull()
{
//	DBG_LOG0("tmlnDriverAnimationFull: destroying animation");
	free_animation();
}

//--------------------------------------------------------------------
// Force a load of the animation (for pre-loading)
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::load_animation(const std::string& i_AnimFile)
{
	fsLocator fileloc;
	fsFileUtil::ANSIFilenameToLocator( i_AnimFile, fileloc );

	this->m_AnimFilename.SetValue(fileloc.GetLastName());
	load_animation();
}

//----------------------------------------------------------------------------
// Set all drivers to skip the loading of all animations in order to
//	speed loading when they aren't needed.
//----------------------------------------------------------------------------
//static 
void tmlnDriverAnimationFull::SetSkipAnimations(bool i_bSkip)
{
	sm_bSkipAnimations = i_bSkip;
}

//----------------------------------------------------------------------------
// Set all drivers to wait until the first call to Operate before loading
//	the driver data.
//----------------------------------------------------------------------------
//static 
void tmlnDriverAnimationFull::SetDelayAnimations(bool i_bDelay)
{
	sm_bDelayAnimations = i_bDelay;
}

//--------------------------------------------------------------------
//	This should be called to set the list of animation files
//	possible for this driver.
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::SetAnimationList(const fsysFileList& i_FileList,
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
//  Update the object that is being driven
//--------------------------------------------------------------------
void  tmlnDriverAnimationFull::Operate(float i_Time)
{
	// Check for delayed animation load
	if (sm_bDelayAnimations && !sm_bSkipAnimations)
	{
		if ((!m_pAnimation) && (m_AnimFilename.GetValue().GetLength() > 0))
		{
			load_animation();
		}
	}

	//	make sure the pointer exists
	//
	if (m_pAnimation == NULL)
	{
		return;	// don't continue
	}

	// Check this often in order to tell when someone is trying to 
	// alter the end time and is breaking the constraint
	//update_driver_length_from_anim_length();

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		float blend_time = 0.0f;
		if (get_blend_time(i_Time, blend_time))
		{
			bool bSmoothBlend = (this->GetBlendType() == tmlnDriver::e_SmoothBlend);
			float easeIn = this->GetEaseInWeight();
			float easeOut = 1.0f;
			if (bSmoothBlend)
			{
				tmlnDriver *pPrevDriver = m_Channel.GetPreviousDriver(i_Time);
				if (pPrevDriver)
					easeOut = pPrevDriver->GetEaseOutWeight();
			}
			
			m_Channel.BlendAnimation( m_pAnimation, this->GetBeginTime(), blend_time,
				bSmoothBlend, easeIn, easeOut);
		}
		// otherwise, don't do anything, let last state continue
	}
	else
	{
		m_Channel.SetAnimation( m_pAnimation, this->GetBeginTime() );
	}

	// This forces a call to Animate() at the given simulation time
	// in order to update the matrices for attachments
	m_Channel.Animate(i_Time);
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverAnimationFull::GetDriverInfo() const
{
	tmlnDriverAnimationFullInfo *pInfo = new tmlnDriverAnimationFullInfo(m_ChunkName);
	this->GetBaseDriverInfo(*pInfo);

	int anim_index = 0;

	pInfo->m_Info.m_AnimFilename	= m_AnimFilename.GetValue();
	pInfo->m_Info.m_AnimName		= m_AnimName.GetValue();
	pInfo->m_Info.m_bDriverToAnimLength = m_bDriverToAnimLength.GetValue();
	pInfo->m_Info.m_bUseAnimStart	= m_bUseAnimStart.GetValue();

	pInfo->m_Info.m_FrameRate		= m_FrameRate.GetValue();
	pInfo->m_Info.m_bLooping		= m_bLooping.GetValue();
	pInfo->m_Info.m_StartFrame		= m_StartFrame.GetValue();
	pInfo->m_Info.m_EndFrame		= m_EndFrame.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::SetDriverInfo(const tmlnDriverAnimationFullInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	// Turn off the driver length flag in order to prevent 
	// multiple updates to the driver length as the anim
	// info is set.
	this->m_bDriverToAnimLength = false;
	this->m_bUseAnimStart = false;

	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// this will load and create the animation as needed
	this->m_AnimFilename.SetValue(i_Info.m_Info.m_AnimFilename, i_Undoable );
	
	this->m_AnimName.SetValue(i_Info.m_Info.m_AnimName, i_Undoable );
	this->m_FrameRate.SetValue(i_Info.m_Info.m_FrameRate, i_Undoable );
	this->m_bLooping.SetValue(i_Info.m_Info.m_bLooping, i_Undoable );
	this->m_StartFrame.SetValue(i_Info.m_Info.m_StartFrame, i_Undoable );
	this->m_EndFrame.SetValue(i_Info.m_Info.m_EndFrame, i_Undoable );

	// Set this data last so that the begin and end time are correct.
	// the earlier sets will alter end time because of the 
	// m_bDriverToAnimLength flag. 
	this->m_bDriverToAnimLength.SetValue(i_Info.m_Info.m_bDriverToAnimLength, i_Undoable );
	this->m_bUseAnimStart.SetValue(i_Info.m_Info.m_bUseAnimStart, i_Undoable );
}

//--------------------------------------------------------------------
// Find out how long the animation keyframe data is in terms
// of frames. (This does not do any computation with the
// current frame rate).
//--------------------------------------------------------------------
float tmlnDriverAnimationFull::GetAnimationNumFrames() const
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
float tmlnDriverAnimationFull::GetAnimationLength() const
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
void tmlnDriverAnimationFull::GetResourceList( fsResourceTrackerData& io_List )
{
	if (m_AnimFilename.GetValue().GetLength() > 0)
	{
		io_List.Add( this->m_AnimFilename.GetValue() );
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverAnimationFull::GetClipFillColor() const
{
	return maFloatRGBA( 1.0f, 0.5412f, 0.7569f, 1.0f );
}

//--------------------------------------------------------------------
//	PerformSplit - Split up the details of this driver between itself
//	and the driver passed in.
//--------------------------------------------------------------------
//virtual 
void tmlnDriverAnimationFull::PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime )
{
	float percent = tmlnDriver::CalculatePercent( i_fTime );

	//	cast the 2nd driver and check it is the same type
	//
	tmlnDriverAnimationFull* pSecondDriver = dynamic_cast<tmlnDriverAnimationFull*>(io_pDriverAtEnd);
	DBG_ASSERT0( pSecondDriver != 0, "Cannot split with 2 different type of drivers" );

	this->m_bDriverToAnimLength.SetValue(false);
	pSecondDriver->m_bDriverToAnimLength.SetValue(false);
	this->m_bUseAnimStart.SetValue(false);
	pSecondDriver->m_bUseAnimStart.SetValue(false);

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
void tmlnDriverAnimationFull::check_driver_name()
{
	if (m_AnimName.GetValue().size() > 0)
	{
		this->SetName(m_AnimName.GetValue().c_str());
	}
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
void tmlnDriverAnimationFull::snap_anim_start()
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
void tmlnDriverAnimationFull::update_driver_length_from_anim_length()
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

		//DBG_LOG1( "driver animfull duration  %6.2f", m_Driver.GetDuration() );
	}
}

//--------------------------------------------------------------------
// return true if blending and if true, return amount blending time
//--------------------------------------------------------------------
bool tmlnDriverAnimationFull::get_blend_time(float i_Time, float &o_BlendTime)
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
// Do actual load of animation data.
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::load_animation()
{
	if (m_pAnimation)
	{
		// Clear out animation in channel so that it doesn't use the animation
		// we are about to delete anymore.
		m_Channel.Reset();

		free_animation();
	}

	if (!sm_bSkipAnimations)
	{
		// This should assert if animation file is not found.
		fsLocator anim_loc = this->GetAnimationLocator( m_AnimFilename.GetValue() );

		//	check for 0
		if (anim_loc.GetNumNames() == 0)
		{
			//	continue without loading any animation
			//
			//DBG_ERROR0("Animation path and filename is empty for animation driver");
			return;

			//std::string msg = "Animation filename is empty";
			//DBG_ERROR1("%s", msg.c_str() );
			//guiMessageBox::Show(msg.c_str(), "Error");
			//assert(false);
		}


		// Load the animation (either ourselves, or through shared animation manager
		if (c_UseSharedAssetMgr)
			m_pAnimKeys = l_SharedAnimationMgr.Load( anim_loc );
		else
			m_pAnimKeys = entImport::LoadAnimKeys( anim_loc );
		
		// Get start time exported with animation data
		if (m_pAnimKeys)
			this->m_AnimStartTime = m_pAnimKeys->GetStartTime();

		//
		if (m_pAnimKeys)
		{
			m_pAnimation = entImport::CreateAnimation( *m_pAnimKeys );

			DBG_ASSERT0( m_pAnimation != 0, "CreateAnimation failed" );

			if (!m_Channel.CheckAnimation(m_pAnimation))
			{
				// Model is incompatible, delete the new animation 
				// and notify the user
				free_animation();
				
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(anim_loc, filename);
				std::ostringstream message;
				message << "Animation " << filename << " is incompatible with model.";
				guiMessageBox::Show(message.str().c_str(), "Incompatible animation", guiMessageBox::e_OKOnly);
			}
			else
			{
				// Animation is compatible

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
		}
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
// Do delete of animation data
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::free_animation()
{
	if (m_pAnimation)
		delete m_pAnimation;
	m_pAnimation = NULL;

	if (m_pAnimKeys)
	{
		if (c_UseSharedAssetMgr)
			l_SharedAnimationMgr.Release(m_pAnimKeys);
		else
			delete m_pAnimKeys;
	}
	m_pAnimKeys = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::AnimFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{			
	if (m_pAnimation)
	{
		// Clear out animation in channel so that it doesn't use the animation
		// we are about to delete anymore.
		m_Channel.Reset();

		free_animation();
	}

	if (!sm_bDelayAnimations)
	{
		load_animation();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::AnimNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	check_driver_name();

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::FrameRateChanged(prtyProperty *i_pProperty, bool i_bDirty)
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
void tmlnDriverAnimationFull::AnimationPropertiesChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	if (m_pAnimation)
	{
		m_pAnimation->SetLooping( m_bLooping.GetValue() );
		update_driver_length_from_anim_length();
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	update_driver_length_from_anim_length();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::UseAnimStartChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	snap_anim_start();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAnimationFull::LengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	if (m_pAnimation)
	{
		//	TODO - figure out a way to UNCHECK the "driver to anim length" flag
		//	if the user resizes the driver

		//	set the values
		m_pAnimation->SetStartFrame( m_StartFrame.GetValue() );
		m_pAnimation->SetEndFrame( m_EndFrame.GetValue() );

		//	
		update_driver_length_from_anim_length();
	}

	this->MarkDirty();
}
