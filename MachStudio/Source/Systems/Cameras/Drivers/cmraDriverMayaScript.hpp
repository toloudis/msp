/*****************************************************************************
**	cmraDriverMayaScript.hpp
**
**		Derived driver class for the camera key
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_DRIVERMAYASCRIPT_HPP
#error cmraDriverMayaScript.hpp multiply included
#endif
#define CMRA_DRIVERMAYASCRIPT_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif 



//============================================================================
//============================================================================
class tmlnChannel;
class tmlnChannelFloat;
class tmlnChannelPosition;
class cmraDriverMayaScriptInfo;
class cam3dAnimKeys;
class prtyComboBoxUIInfo;
class prtyRangedFloatUIInfo;

//============================================================================
//============================================================================
class cmraDriverMayaScript : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverMayaScript( tmlnChannelPosition& i_ChannelPosition, 
						  tmlnChannelPosition& i_ChannelTarget, 
						  tmlnChannelFloat& i_ChannelFov, 
						  tmlnChannelFloat& i_ChannelTilt, 
						  tmlnChannelFloat& i_ChannelFocalLength, 
						  tmlnChannelFloat& i_ChannelHorizontalAperture );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverMayaScript();

	//--------------------------------------------------------------------
	// Only attach to stereo parameters if we have aniamtion 
	// data for them.
	//--------------------------------------------------------------------
	void AttachToStereoParams( tmlnChannelFloat* i_pChannelStereoFD, 
							   tmlnChannelFloat* i_pChannelStereoIOD );

	
	//--------------------------------------------------------------------
	// Return true if the animation data that has been loaded has
	// stereo animation also. It means this driver needs to be attached
	// to the stereo channels also.
	//--------------------------------------------------------------------
	bool HasStereoAnimation() const;

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
	void SetDriverInfo(	const cmraDriverMayaScriptInfo& i_Info );

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
	// Returns how long the animation is in seconds given the 
	// frame rate and other data about this animation.
	//--------------------------------------------------------------------
	float GetAnimationLength() const;

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	void GetResourceList( fsResourceTrackerData& io_List );

	//====================================================================
	// Get/Set data
	//====================================================================

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const fsLocator&	GetAnimFilename() const;

	//--------------------------------------------------------------------
	// Sets frame rate of driver to match frame rate of animation file.
	//--------------------------------------------------------------------
	void SetFrameRateFromAnimData();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline float GetFrameRate() const;
	void SetFrameRate( float i_Val );

	//--------------------------------------------------------------------
	// -1.0 is default, meaning use the beginning of the clip
	//--------------------------------------------------------------------
	inline float GetStartFrame() const;
	void SetStartFrame( float i_Val );

	//--------------------------------------------------------------------
	// -1.0 is default, meaning use the end of the clip
	//--------------------------------------------------------------------
	inline float GetEndFrame() const;
	void SetEndFrame( float i_Val );

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
	inline bool IsUseAnimStart() const;
	void SetUseAnimStartFlag( bool i_bSetFlag );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const maPoint3d& GetOffset() const;
	void SetOffset(const maPoint3d& i_Offset );

	//--------------------------------------------------------------------
	//	PerformSplit - Split up the details of this driver between itself
	//	and the driver passed in.
	//--------------------------------------------------------------------
	virtual void PerformSplit( tmlnDriver* io_pDriverAtEnd, const maTime& i_fTime );

private:
	//--------------------------------------------------------------------
	//	Perform the operate on a single channel
	//--------------------------------------------------------------------
	void OperateChannel( const maTime& i_Time, tmlnChannelPosition& i_Channel, maPoint3d& i_Goal );
	void OperateChannel( const maTime& i_Time, tmlnChannelFloat& i_Channel, float i_Goal );

	//--------------------------------------------------------------------
	// Resize duration of driver to match length of animation if requested
	//--------------------------------------------------------------------
	void update_driver_length_from_anim_length();

	//--------------------------------------------------------------------
	// Check value of animation start properties and move 
	//	begin time if needed.
	//--------------------------------------------------------------------
	void snap_anim_start();

	//--------------------------------------------------------------------
	// Compute frame of animation to use based on frame rate, 
	//	start/end frame, etc.
	//--------------------------------------------------------------------
	float compute_frame(const maTime& i_Time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AnimFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void AnimationPropertiesChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void UseAnimStartChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void LengthChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void ChannelsChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	// Adjust the channel so that this driver is included in the
	// channel list only if the boolean is set to true
	//--------------------------------------------------------------------
	void confirm_channel(tmlnChannel& i_Channel, bool i_bAttach);

private:
	tmlnChannelPosition&	m_ChannelPosition;
	tmlnChannelPosition&	m_ChannelTarget;
	tmlnChannelFloat&		m_ChannelFov;
	tmlnChannelFloat&		m_ChannelTilt;
	tmlnChannelFloat&		m_ChannelFocalLength;
	tmlnChannelFloat&		m_ChannelHorizontalAperture;
	
	tmlnChannelFloat*		m_pChannelStereoFD;		// can be NULL
	tmlnChannelFloat*		m_pChannelStereoIOD;	// can be NULL

	cam3dAnimKeys*	m_pAnimKeys;
	int m_NumFrames;			// computed, not written to file
	float m_OriginalFrameRate;  // frame rate of animation in file
	bool m_bDisableResize;		// while doing a "SetData()", disable m_bDriverToAnimLength resizing

	// Need to keep track of these UI Infos because we need to
	// adjust them after the contructor
	//prtyComboBoxUIInfo *m_pAnimFilenameUIInfo;
	prtyRangedFloatUIInfo *m_pStartFrameUIInfo;
	prtyRangedFloatUIInfo *m_pEndFrameUIInfo;

	prtyFilePath	m_AnimFilename;
	prtyFloat		m_FrameRate;
	prtyFloat		m_StartFrame;			// -1.0 is default / not set
	prtyFloat		m_EndFrame;			// -1.0 is default / not set
	prtyBoolean		m_bLooping;
	prtyBoolean		m_bDriverToAnimLength;
	prtyBoolean		m_bUseAnimStart;
	prtyPoint3d		m_Offset;			// offset all positions by this amount
	prtyDistance	m_CutThreshold;

	// This property is not saved to file, just used for display
	prtyFloat		m_AnimStartTime;

	// Control over which channels are altered through a list
	// of booleans for each channel that might be altered
	prtyBoolean		m_bAlterPosition;
	prtyBoolean		m_bAlterTarget;
	prtyBoolean		m_bAlterFov;
	prtyBoolean		m_bAlterTilt;
	prtyBoolean		m_bAlterFocalLength;
	prtyBoolean		m_bAlterHorizontalAperture;
	prtyBoolean		m_bAlterStereoFD;
	prtyBoolean		m_bAlterStereoIOD;
	
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const fsLocator&	cmraDriverMayaScript::GetAnimFilename() const
{
	return m_AnimFilename.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline float cmraDriverMayaScript::GetFrameRate() const
{
	return m_FrameRate.GetValue();
}
//--------------------------------------------------------------------
// -1.0 is default, meaning use the beginning of the clip
//--------------------------------------------------------------------
inline float cmraDriverMayaScript::GetStartFrame() const
{
	return m_StartFrame.GetValue();
}

//--------------------------------------------------------------------
// -1.0 is default, meaning use the end of the clip
//--------------------------------------------------------------------
inline float cmraDriverMayaScript::GetEndFrame() const
{
	return m_EndFrame.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool cmraDriverMayaScript::GetLooping() const
{
	return m_bLooping.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool cmraDriverMayaScript::IsDriverToAnimLength() const
{
	return m_bDriverToAnimLength.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool cmraDriverMayaScript::IsUseAnimStart() const
{
	return m_bUseAnimStart.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const maPoint3d& cmraDriverMayaScript::GetOffset() const
{
	return m_Offset.GetValue();
}

