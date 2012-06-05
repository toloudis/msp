/*****************************************************************************
**	tmlnDriverAnimatedFileName.hpp
**
**		Derived driver class for animating a set of filenames in
**	numeric order.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERANIMATEDFILENAME_HPP
#error tmlnDriverAnimatedFileName.hpp multiply included
#endif
#define TMLN_DRIVERANIMATEDFILENAME_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
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

#include <vector>


//============================================================================
//============================================================================
class tmlnChannelFileName;
class tmlnDriverAnimatedFileNameInfo;
class matTexture;


//============================================================================
//============================================================================
class tmlnDriverAnimatedFileName : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAnimatedFileName( tmlnChannelFileName& i_ChannelTexture, 
								chDefs::Name i_ChunkName );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverAnimatedFileName();

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
	void SetDriverInfo(	const tmlnDriverAnimatedFileNameInfo& i_Info );

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
	// Set Textures as list of texture names in numbered order.
	// i_FirstFileName defines the pattern, digits before the
	// file extension are used to specify the numbering.
	//--------------------------------------------------------------------
	void SetTextures( const itString& i_FirstFileName,
					   int i_NumFrames );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const itString&	GetFirstFileName() const;

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

private:
	//--------------------------------------------------------------------
	// Compute frame of animation to use based on frame rate, 
	//	start/end frame, etc.
	//--------------------------------------------------------------------
	int compute_frame(const maTime& i_Time);

	//--------------------------------------------------------------------
	// Set Textures as list of texture names in numbered order.
	// i_FirstFileName defines the pattern, digits before the
	// file extension are used to specify the numbering.
	//--------------------------------------------------------------------
	void set_textures( const itString& i_FirstFileName,
					   int i_NumFrames );

	//--------------------------------------------------------------------
	// Resize duration of driver to match length of animation if requested
	//--------------------------------------------------------------------
	void update_driver_length_from_anim_length();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void FileNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void DriverToAnimLengthChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnChannelFileName&	m_ChannelTexture;
	chDefs::Name m_ChunkName;
		
	std::vector<itString> m_Textures;

	prtyFileName	m_FirstFileName;
	prtyInt32		m_NumberOfFrames;	
	prtyFloat		m_FrameRate;
	prtyBoolean		m_bLooping;
	prtyBoolean		m_bDriverToAnimLength;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const itString&	tmlnDriverAnimatedFileName::GetFirstFileName() const
{
	return m_FirstFileName.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline float tmlnDriverAnimatedFileName::GetFrameRate() const
{
	return m_FrameRate.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool tmlnDriverAnimatedFileName::GetLooping() const
{
	return m_bLooping.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool tmlnDriverAnimatedFileName::IsDriverToAnimLength() const
{
	return m_bDriverToAnimLength.GetValue();
}

