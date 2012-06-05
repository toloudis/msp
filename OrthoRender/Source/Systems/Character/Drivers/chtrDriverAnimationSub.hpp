/*****************************************************************************
**	chtrDriverAnimationSub.hpp
**
**	Derived driver class for sub object animation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_DRIVERANIMATIONSUB_HPP
#error chtrDriverAnimationSub.hpp multiply included
#endif
#define CHTR_DRIVERANIMATIONSUB_HPP

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
class chtrChannelAnimationSub;
class chtrDriverAnimationSubInfo;
class tmlnDriverInfo;
class entAnimation;
class entAnimKeys;


//============================================================================
//============================================================================
class chtrDriverAnimationSub : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chtrDriverAnimationSub(chtrChannelAnimationSub &i_Channel,
						   const fsLocator &i_CharacterDir);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~chtrDriverAnimationSub();

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
	void SetDriverInfo(	const chtrDriverAnimationSubInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	//virtual void  DoEditProperties();

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
	inline const fsLocator&	GetCharacterDir() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const itString&	GetAnimFilename() const;
	void SetAnimFilename( const itString& i_AnimFilename, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline bool IsDriverToAnimLength() const;
	void SetDriverToAnimLengthFlag( bool i_bSetFlag, 
									prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const std::string& GetAnimName() const;
	void SetAnimName( const std::string& i_AnimName, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline float GetFrameRate() const;
	void SetFrameRate( float i_Val, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// -1.0 is default, meaning use the beginning of the clip
	//--------------------------------------------------------------------
	inline float GetStartFrame() const;
	void SetStartFrame( float i_Val, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// -1.0 is default, meaning use the end of the clip
	//--------------------------------------------------------------------
	inline float GetEndFrame() const;
	void SetEndFrame( float i_Val, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline bool GetLooping() const;
	void SetLooping( bool i_bSetFlag, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

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
	// return true if blending and if true, return amount blending time
	//--------------------------------------------------------------------
	bool get_blend_time(float i_Time, float &o_BlendTime);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	chtrChannelAnimationSub  &m_Channel;

	entAnimation*	m_pAnimation;
	entAnimKeys*	m_pAnimKeys;

	fsLocator	m_CharacterDir;

	prtyBoolean		m_bDriverToAnimLength;
	prtyFileName	m_AnimFilename;
	prtyText		m_AnimName;
	prtyFloat		m_FrameRate;
	prtyFloat		m_StartFrame;			// -1.0 is default / not set
	prtyFloat		m_EndFrame;				// -1.0 is default / not set
	prtyBoolean		m_bLooping;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const fsLocator&	chtrDriverAnimationSub::GetCharacterDir() const
{
	return m_CharacterDir;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const itString&	chtrDriverAnimationSub::GetAnimFilename() const
{
	return m_AnimFilename.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool chtrDriverAnimationSub::IsDriverToAnimLength() const
{
	return m_bDriverToAnimLength.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const std::string& chtrDriverAnimationSub::GetAnimName() const
{
	return m_AnimName.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline float chtrDriverAnimationSub::GetFrameRate() const
{
	return m_FrameRate.GetValue();
}

//--------------------------------------------------------------------
// -1.0 is default, meaning use the beginning of the clip
//--------------------------------------------------------------------
inline float chtrDriverAnimationSub::GetStartFrame() const
{
	return m_StartFrame.GetValue();
}

//--------------------------------------------------------------------
// -1.0 is default, meaning use the end of the clip
//--------------------------------------------------------------------
inline float chtrDriverAnimationSub::GetEndFrame() const
{
	return m_EndFrame.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool chtrDriverAnimationSub::GetLooping() const
{
	return m_bLooping.GetValue();
}

