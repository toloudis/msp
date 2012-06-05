/*****************************************************************************
**	tmlnDriverVideo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Video/tmlnDriverVideo.hpp"

#include "Drivers/Video/tmlnDriverVideoInfo.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelFileName.hpp"
#include "Support/tmln/tmlnChannelVideo.hpp"

//#include "Core/fs/fsResourceTracker.hpp"
#include "Core/It/itStringUtil.hpp"
#include "core/Fs/fsFileUtil.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include "Tool/gui/guiMessageBox.hpp"

#include <algorithm>
#include <sstream>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnDriverVideo::tmlnDriverVideo(tmlnChannelVideo& i_ChannelVideo, 
								 chDefs::Name i_ChunkName)
:	m_ChannelVideo(i_ChannelVideo), 
	m_pPlayingVideo(NULL),
	m_ChunkName(i_ChunkName),
	//m_FirstTextureFileName("First Texture Filename",i_ChannelTexture.GetValue()),
	m_FrameRate("Frame Rate", g3dConstants::c_fDefaultFrameRate),
	m_bLooping("Looping", false),
	m_bReversing("Reversing", false),
	m_bDriverToVideoLength("Driver to Anim Length", true),
	//m_fVideoStartTime("Video Start Time", 0.0f),
	//m_fVideoEndTime("Video End Time", 0.0f),
	m_bPrevLoop(false)
{
	this->SetBlendType(tmlnDriver::e_NoBlending);

	// Since we are getting our initial value from the channel, we need
	// to actually do the call to load the texture name for this initial value.
	// After this, changes to the property will load the next textures.
	//set_textures(m_FirstTextureFileName.GetValue(), m_NumberOfFrames.GetValue());

	prtyFileChooserUIInfo* pPFCUII = new prtyFileChooserUIInfo(&(m_FileName), "Video", "filename");
	AddProperty( pPFCUII );
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyFloatEditUIInfo(&(m_FrameRate), "Video", "Frames per second to play");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bLooping), "Video", "Animation loops back to start");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bReversing), "Video", "Animation reverses direction when it loops");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_bDriverToVideoLength), "Video", "Resize driver length to length of animation at given frame rate");
	AddProperty( pPUII );
	//pPUII = new prtyCheckBoxUIInfo(&(m_fVideoStartTime), "Video", "Starting time for video");
	//AddProperty( pPUII );
	//pPUII = new prtyCheckBoxUIInfo(&(m_fVideoEndTime), "Video", "Ending time for video");
	//AddProperty( pPUII );

	//	property callbacks
	m_FileName.AddCallback(new prtyCallbackWrapper<tmlnDriverVideo>(this, &tmlnDriverVideo::VideoFileNameChanged));
	m_FrameRate.AddCallback(new prtyCallbackWrapper<tmlnDriverVideo>(this, &tmlnDriverVideo::PropertyChanged));
	m_bLooping.AddCallback(new prtyCallbackWrapper<tmlnDriverVideo>(this, &tmlnDriverVideo::PropertyChanged));
	m_bReversing.AddCallback(new prtyCallbackWrapper<tmlnDriverVideo>(this, &tmlnDriverVideo::ReversePropertyChanged));
	m_bDriverToVideoLength.AddCallback(new prtyCallbackWrapper<tmlnDriverVideo>(this, &tmlnDriverVideo::DriverToAnimLengthChanged));
	//m_fVideoStartTime.AddCallback(new prtyCallbackWrapper<tmlnDriverVideo>(this, &tmlnDriverVideo::PropertyChanged));
	//m_fVideoEndTime.AddCallback(new prtyCallbackWrapper<tmlnDriverVideo>(this, &tmlnDriverVideo::PropertyChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnDriverVideo::~tmlnDriverVideo()
{
//	std::for_each(m_Textures.begin(), m_Textures.end(), matTextureMgr::ReleaseTexture);
	//m_ChannelVideo.SetValue(CVideoData());
	
	if( m_pPlayingVideo )
	{
		mnmAviUtil::CloseAvi( m_pPlayingVideo );
		m_pPlayingVideo = NULL;
	}
}

//----------------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//----------------------------------------------------------------------------
std::string tmlnDriverVideo::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
/*
	if (m_FirstTextureFileName.GetValue().GetNumNames() > 0)
	{
		std::string str = "List : ";
		str += itStringUtil::GetStdString(m_FirstTextureFileName.GetValue().GetLastName());
		desc += str;
	}
*/
	return desc;
}

//----------------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//----------------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverVideo::GetDriverInfo() const
{
	tmlnDriverVideoInfo *pInfo = new tmlnDriverVideoInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_FileName = this->m_FileName.GetValue();
	pInfo->m_FrameRate = this->m_FrameRate.GetValue();
	pInfo->m_bLooping = this->m_bLooping.GetValue();
	pInfo->m_bReversing = this->m_bReversing.GetValue();
	pInfo->m_bDriverToAnimLength = this->m_bDriverToVideoLength.GetValue();

	return pInfo;
}

//----------------------------------------------------------------------------
// Set internal variables from data structure
//----------------------------------------------------------------------------
void tmlnDriverVideo::SetDriverInfo( const tmlnDriverVideoInfo& i_Info )
{
	// Turn off the driver length flag in order to prevent 
	// multiple updates to the driver length as the anim
	// info is set.
	this->m_bDriverToVideoLength = false;

	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// this will load the video
	this->m_FileName.SetValue(i_Info.m_FileName);

	// Store local info
	this->m_FrameRate.SetValue(i_Info.m_FrameRate);
	this->m_bLooping.SetValue(i_Info.m_bLooping);
	this->m_bReversing.SetValue(i_Info.m_bReversing);

	// Set this data last so that the begin and end time are correct.
	// the earlier sets will alter end time because of the 
	// m_bDriverToAnimLength flag. 
	this->m_bDriverToVideoLength.SetValue(i_Info.m_bDriverToAnimLength);

	this->MarkDirty();
}


//----------------------------------------------------------------------------
//  Update position of things that are being driven
//----------------------------------------------------------------------------
void  tmlnDriverVideo::Operate(const maTime& i_Time)
{
	// If the blend settings say no blend, and time is less than our begin time,
	// leave the old texture in place.
	if (IsBefore(i_Time) && this->GetBlendType() == tmlnDriver::e_NoBlending)
	{
		return;
	}

	if ( m_FileName.GetValue().GetNumNames() != 0 )
	{
		CVideoData Data;
		Data.m_Filename = m_FileName.GetValue();
		Data.m_Frame = compute_frame( i_Time );
		GetVideoDimensions( Data.m_Width, Data.m_Height );
		Data.m_pVideo = (void*)m_pPlayingVideo;
		m_ChannelVideo.SetValue(Data);
//		m_ChannelVideo.Animate( i_Time );
	}
	else
	{
		// empty means clear the texture (this is a possible animation state)
		m_ChannelVideo.SetValue(CVideoData());
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverVideo::GetClipFillColor() const
{
	return maFloatRGBA( 0.5412f, 0.6392f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------
//	Returns how long the animation is in seconds given the 
//	frame rate and other data about this animation.
//--------------------------------------------------------------------
float tmlnDriverVideo::GetAnimationLength() const
{
//	if (m_FrameRate.GetValue() == 0)
		return 0;
//	else
//		return (m_NumberOfFrames.GetValue() / m_FrameRate.GetValue());
}

//--------------------------------------------------------------------
//	get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void tmlnDriverVideo::GetResourceList( fsResourceTrackerData& io_List )
{
	//	if there is a file...
	if (m_FileName.GetValue().GetNumNames() > 0)
	{
		io_List.Add( m_FileName.GetValue() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverVideo::SetFrameRate( float i_Val )
{
	m_FrameRate.SetValue(i_Val);

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverVideo::SetLooping( bool i_bSetFlag )
{
	m_bPrevLoop = m_bLooping.GetValue();
	m_bLooping.SetValue(i_bSetFlag);

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverVideo::SetDriverToAnimLengthFlag( bool i_bSetFlag )
{
	m_bDriverToVideoLength.SetValue(i_bSetFlag);

	this->MarkDirty();
}

//--------------------------------------------------------------------
// Compute frame of animation to use based on frame rate, 
//	start/end frame, etc.
//--------------------------------------------------------------------
int tmlnDriverVideo::compute_frame(const maTime& i_Time)
{
	int num_frames = (int)mnmAviUtil::GetAviLength(m_pPlayingVideo);
	if (num_frames == 0)
		return 0;

	// should this round off, not clamp down?
	//TIME - float frame rate given to AsFrame()
	int frame = (i_Time - GetBeginTime()).AsFrame(m_FrameRate.GetValue());
	if (frame <= 0)
	{
		frame = 0;
	}
	else if (frame >= num_frames)
	{
		if ( m_bReversing.GetValue() )
		{
			m_bLooping.SetValue(true);
			int rev_total_length = (2 * num_frames) - 2; // (1-2-3-2)-(1-2-3-2) is reversing loop
			frame = frame % rev_total_length;
			if (frame >= num_frames)
				frame = rev_total_length - frame;
		}
		else if ( m_bLooping.GetValue() )
		{
			frame = frame % num_frames;
		}
		else
		{
			frame = num_frames - 1;
		}
	}
	return frame;
}

//--------------------------------------------------------------------
// Resize duration of driver to match length of animation if requested
//--------------------------------------------------------------------
void tmlnDriverVideo::update_driver_length_from_anim_length()
{
	if( m_FileName.GetValue().GetNumNames() > 0)
	{
		//	calculate the duration and set the end time
		//
		maTime duration = calculate_duration();

		SetEndTime( GetBeginTime() + duration );

		// update gui
		chnlDialogUtil::UpdateDriver(this);
	}
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverVideo::VideoFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	tmlnDriverVideoInfo *pInfo = dynamic_cast<tmlnDriverVideoInfo*>(GetDriverInfo());

	if ( pInfo == NULL ) return;

	//	if no video, don't continue
	if( pInfo->m_FileName.GetNumNames() == 0 ) return;

	itString ext;

	pInfo->m_FileName.GetLastName().GetExtension(ext);
	itStringUtil::ToLower(ext);

	if (ext == itString("avi"))
	{
		if( !load_video(*pInfo))
		{
			m_FileName = fsLocator(); //the file didn't load so clear the name
		}

		maTime duration = calculate_duration();

		SetEndTime( GetBeginTime() + duration );

		// update gui
		chnlDialogUtil::UpdateDriver(this);

		this->MarkDirty();
	}
	delete pInfo;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverVideo::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	update_driver_length_from_anim_length();
	this->MarkDirty();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverVideo::DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	update_driver_length_from_anim_length();
}

void tmlnDriverVideo::ReversePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if(m_bReversing.GetValue()) 
		SetLooping(true);
	else
		SetLooping(m_bPrevLoop);
	update_driver_length_from_anim_length();
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maTime tmlnDriverVideo::calculate_duration()
{
	//	if no video, leave the duration that is already there.
	//
	if ( m_pPlayingVideo == NULL )
	{
		return (GetEndTime() - GetBeginTime()); //0.0f;
	}

	//	check if flag set to set the driver to the sound length
	//
	maTime video_length;
	if (m_bDriverToVideoLength.GetValue())
	{
		float num_frames = mnmAviUtil::GetAviLength(m_pPlayingVideo);

		if (m_FrameRate.GetValue() == 0)
			return maTime::c_ZeroTime;
		else
			return maTime::FromFrame(num_frames, m_FrameRate.GetValue());
	}
	else
	{
		video_length = GetEndTime() - GetBeginTime();
	}

	//video_length -= m_fVideoStartTime.GetValue();
	//video_length -= m_fVideoEndTime.GetValue();
	//if (video_length < 0) video_length = 1;
	//if (video_length > mnmAviUtil::GetAviLength(m_pPlayingVideo)) 
	//	video_length = mnmAviUtil::GetAviLength(m_pPlayingVideo);


	return video_length;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maTime tmlnDriverVideo::calculate_endtime()
{
	maTime end_time = GetBeginTime() + calculate_duration();
	return end_time;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool tmlnDriverVideo::load_video( const tmlnDriverVideoInfo& i_Info )
{
	// Clean up before setting
	if (m_pPlayingVideo != 0)
	{
		mnmAviUtil::CloseAvi(m_pPlayingVideo);
		m_pPlayingVideo = NULL;
	}

	//	if no video, don't continue
	if( i_Info.m_FileName.GetNumNames() == 0 ) return false;

	// Create video

	// Load sound
	//
	itString filename;
	fsFileUtil::LocatorToUnicodeString( i_Info.m_FileName, filename );
	m_pPlayingVideo = mnmAviUtil::OpenAVI( filename.GetString() );
	if( !m_pPlayingVideo )
	{
		static char errBuf[128];
		errBuf[0] = 0;
		mnmAviUtil::FormatAviMessage( mnmAviUtil::LastError, errBuf, 128 );
		std::string msg = "Problem loading video, " + std::string( errBuf );
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Video Failure", guiMessageBox::e_OKOnly);
		return false;
	}
	return true;
}

void tmlnDriverVideo::GetVideoDimensions( int& o_Width, int& o_Height )
{
	if( m_pPlayingVideo )
	{
		mnmAviUtil::GetAviDimensions( m_pPlayingVideo, o_Width, o_Height );
	}
	else
	{
		o_Width = 0;
		o_Height = 0;
	}
}