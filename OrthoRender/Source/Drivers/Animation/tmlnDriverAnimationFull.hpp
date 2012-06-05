/*****************************************************************************
**	tmlnDriverAnimationFull.hpp
**
**	Derived driver class for full object animation
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERANIMATIONFULL_HPP
#error tmlnDriverAnimationFull.hpp multiply included
#endif
#define TMLN_DRIVERANIMATIONFULL_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif

#ifndef API3D_OBJECTENTITY_HPP
#include "Tool/api3d/api3dObjectEntity.hpp"
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
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelAnimationFull;
class tmlnDriverAnimationFullInfo;
class tmlnDriverInfo;
class entAnimation;
class entAnimKeys;
class fsysFileList;
class prtyComboBoxUIInfo;
class prtyRangedFloatUIInfo;


//============================================================================
//============================================================================
class tmlnDriverAnimationFull : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAnimationFull(tmlnChannelAnimationFull &i_Channel, 
							chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverAnimationFull();

	//--------------------------------------------------------------------
	// Force a load of the animation (for pre-loading)
	//--------------------------------------------------------------------
	void load_animation(const std::string& i_AnimFile);

	//----------------------------------------------------------------------------
	// Set all drivers to skip the loading of all animations in order to
	//	speed loading when they aren't needed.
	//----------------------------------------------------------------------------
	static void SetSkipAnimations(bool i_bSkip);

	//----------------------------------------------------------------------------
	// Set all drivers to wait until the first call to Operate before loading
	//	the driver data.
	//----------------------------------------------------------------------------
	static void SetDelayAnimations(bool i_bDelay);

	//--------------------------------------------------------------------
	//	This should be called to set the list of animation files
	//	possible for this driver.
	//--------------------------------------------------------------------
	void SetAnimationList(const fsysFileList& i_FileList,
						  bool i_bUpdateControl = true);

	//--------------------------------------------------------------------
	//	Find the full locator for the given animation filename.
	//	Needs to be implemented in base classes.
	//--------------------------------------------------------------------
	virtual fsLocator GetAnimationLocator( const itString &i_AnimFilename )const  = 0;

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

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
	void SetDriverInfo(	const tmlnDriverAnimationFullInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

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

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

protected:
	//--------------------------------------------------------------------
	//	PerformSplit - Split up the details of this driver between itself
	//	and the driver passed in.
	//--------------------------------------------------------------------
	virtual void PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime );

private:
	//--------------------------------------------------------------------
	// Set name of driver from filename or anim name
	//--------------------------------------------------------------------
	void check_driver_name();

	//--------------------------------------------------------------------
	// Check value of animation start properties and move 
	//	begin time if needed.
	//--------------------------------------------------------------------
	void snap_anim_start();

	//--------------------------------------------------------------------
	// Resize duration of driver to match length of animation if requested
	//--------------------------------------------------------------------
	void update_driver_length_from_anim_length();

	//--------------------------------------------------------------------
	// return true if blending and if true, return amount blending time
	//--------------------------------------------------------------------
	bool get_blend_time(float i_Time, float &o_BlendTime);

	//--------------------------------------------------------------------
	// Do actual load of animation data.
	//--------------------------------------------------------------------
	void load_animation();

	//--------------------------------------------------------------------
	// Do delete of animation data
	//--------------------------------------------------------------------
	void free_animation();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AnimFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void AnimNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void FrameRateChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void AnimationPropertiesChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void UseAnimStartChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void LengthChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnChannelAnimationFull  &m_Channel;
	chDefs::Name m_ChunkName;

	entAnimation*	m_pAnimation;
	entAnimKeys*	m_pAnimKeys;

	prtyBoolean		m_bDriverToAnimLength;
	prtyBoolean		m_bUseAnimStart;
	prtyFileName	m_AnimFilename;
	prtyText		m_AnimName;
	prtyFloat		m_FrameRate;
	prtyFloat		m_StartFrame;			// -1.0 is default / not set
	prtyFloat		m_EndFrame;				// -1.0 is default / not set
	prtyBoolean		m_bLooping;

	// This property is not saved to file, just used for display
	prtyFloat		m_AnimStartTime;

	// Need to keep track of these UI Infos because we need to
	// adjust them after the contructor
	prtyComboBoxUIInfo *m_pAnimFilenameUIInfo;
	prtyRangedFloatUIInfo *m_pStartFrameUIInfo;
	prtyRangedFloatUIInfo *m_pEndFrameUIInfo;

	static bool sm_bSkipAnimations, sm_bDelayAnimations;
};

