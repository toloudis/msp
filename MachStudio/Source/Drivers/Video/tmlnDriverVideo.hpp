/*****************************************************************************
**	tmlnDriverVideo.hpp
**
**		Derived driver class for animating a set of Texture filenames in
**	numeric order.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERVIDEO_HPP
#error tmlnDriverVideo.hpp multiply included
#endif
#define TMLN_DRIVERVIDEO_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_VIDEO_HPP
#include "Core/prty/prtyVideo.hpp"
#endif

#ifndef MNM_AVIUTIL_HPP
#include "Support/mnm/mnmAviUtil.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class tmlnChannelFilePath;
class tmlnChannelVideo;
class tmlnDriverVideoInfo;
class matTexture;


//============================================================================
//============================================================================
class tmlnDriverVideo : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverVideo( tmlnChannelVideo& i_ChannelVideo, chDefs::Name i_ChunkName );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverVideo();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	// Set internal variables from data structure
	//--------------------------------------------------------------------
	void SetDriverInfo(	const tmlnDriverVideoInfo& i_Info );

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	// Returns how long the animation keyframe data is in terms
	// of frames. (This does not do any computation with the
	// current frame rate).
	//--------------------------------------------------------------------
	float GetAnimationNumFrames() const;

	//--------------------------------------------------------------------
	//	Returns how long the animation is in seconds given the 
	//	frame rate and other data about this animation.
	//--------------------------------------------------------------------
	float GetAnimationLength() const;

	//--------------------------------------------------------------------
	//	get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	void GetResourceList( fsResourceTrackerData& io_List );


	//====================================================================
	// Get/Set data
	//====================================================================

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline float GetFrameRate() const;
	void SetFrameRate( float i_Val );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline bool GetLooping() const;
	void SetLooping( bool i_bSetFlag );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline bool IsDriverToAnimLength() const;
	void SetDriverToAnimLengthFlag( bool i_bSetFlag );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void GetVideoDimensions( int& o_Width, int& o_Height );

private:
	//--------------------------------------------------------------------
	// Compute frame of animation to use based on frame rate, 
	//	start/end frame, etc.
	//--------------------------------------------------------------------
	int compute_frame(const maTime& i_Time);

	//--------------------------------------------------------------------
	// Set Textures as list of texture names in numbered order.
	// i_FirstTextureFileName defines the pattern, digits before the
	// file extension are used to specify the numbering.
	//--------------------------------------------------------------------
//	void set_textures( const fsLocator& i_FirstTextureFileName,
//					   int i_NumFrames );

	//--------------------------------------------------------------------
	// Resize duration of driver to match length of animation if requested
	//--------------------------------------------------------------------
	void update_driver_length_from_anim_length();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void VideoFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void ReversePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	maTime calculate_endtime();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	maTime calculate_duration();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool load_video(const tmlnDriverVideoInfo& i_Info);

private:
	//tmlnChannelFilePath&	m_ChannelVideo;
	tmlnChannelVideo&		m_ChannelVideo;
	chDefs::Name			m_ChunkName;
		
	prtyFilePath			m_FileName;
	prtyFloat				m_FrameRate;
	prtyBoolean				m_bLooping;
	prtyBoolean				m_bReversing;
	prtyBoolean				m_bDriverToVideoLength;
	bool					m_bPrevLoop;
	//prtyFloat				m_fVideoStartTime;
	//prtyFloat				m_fVideoEndTime;

	mnmAviUtil::HAVI		m_pPlayingVideo;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
/*
inline const fsLocator&	tmlnDriverVideo::GetFirstTextureFileName() const
{
	return m_FirstTextureFileName.GetValue();
}
*/
//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline float tmlnDriverVideo::GetFrameRate() const
{
	return m_FrameRate.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool tmlnDriverVideo::GetLooping() const
{
	return m_bLooping.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool tmlnDriverVideo::IsDriverToAnimLength() const
{
	return m_bDriverToVideoLength.GetValue();
}

