/*****************************************************************************
**	billDriverAnimatedTexture.hpp
**
**		Derived driver class for the camera key
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef BILL_DRIVERANIMATEDTEXTURE_HPP
#error billDriverAnimatedTexture.hpp multiply included
#endif
#define BILL_DRIVERANIMATEDTEXTURE_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
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
class billChannelTexture;
class billDriverAnimatedTextureInfo;
class matTexture;


//============================================================================
//============================================================================
class billDriverAnimatedTexture : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	billDriverAnimatedTexture( billChannelTexture& i_ChannelTexture );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~billDriverAnimatedTexture();

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
	void SetDriverInfo(	const billDriverAnimatedTextureInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	virtual void  DoEditProperties();

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
	// i_FirstTextureFilename defines the pattern, digits before the
	// file extension are used to specify the numbering.
	//--------------------------------------------------------------------
	void SetTextures( const itString& i_FirstTextureFilename,
					  int i_NumFrames );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline const itString&	GetFirstTextureFilename() const;

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
	int compute_frame(float i_Time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	billChannelTexture&	m_ChannelTexture;
		
	std::vector<matTexture*> m_Textures;

	prtyFileName	m_FirstTextureFilename;
	prtyInt32		m_NumberOfFrames;	
	prtyFloat		m_FrameRate;
	prtyBoolean		m_bLooping;
	prtyBoolean		m_bDriverToAnimLength;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const itString&	billDriverAnimatedTexture::GetFirstTextureFilename() const
{
	return m_FirstTextureFilename.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline float billDriverAnimatedTexture::GetFrameRate() const
{
	return m_FrameRate.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool billDriverAnimatedTexture::GetLooping() const
{
	return m_bLooping.GetValue();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool billDriverAnimatedTexture::IsDriverToAnimLength() const
{
	return m_bDriverToAnimLength.GetValue();
}

