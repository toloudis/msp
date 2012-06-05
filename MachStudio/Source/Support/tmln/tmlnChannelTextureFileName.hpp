/*****************************************************************************
**	tmlnChannelTextureFileName.hpp
**
**	 TextureFileName channel
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELTEXTUREFILENAME_HPP
#error tmlnChannelFilePath.hpp multiply included
#endif
#define TMLN_CHANNELTEXTUREFILENAME_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif

//============================================================================
//============================================================================
class prtyProperty;


//============================================================================
//============================================================================
class tmlnChannelTextureFileName : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelTextureFileName(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new state for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetValue(const prtyTextureFileData& i_Val) = 0;

	//--------------------------------------------------------------------
	//  Get current state of object
	//--------------------------------------------------------------------
	virtual const prtyTextureFileData& GetValue() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" state that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalValue(const prtyTextureFileData& i_Val);
	const prtyTextureFileData& GetOriginalValue() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	prtyTextureFileData m_OriginalValue;
};

//============================================================================
//============================================================================
class tmlnChannelTextureFileNameProperty : public tmlnChannelTextureFileName
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelTextureFileNameProperty(prtyTextureFileName& i_Property);

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void  SetValue(const prtyTextureFileData& i_Val);

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const prtyTextureFileData& GetValue() const;

	//--------------------------------------------------------------------
	// HasValueVariation - returns true if the current value of the
	//	property is different than the scripted value of the channel.
	//	This means the user has edited the values and is an opportunity 
	//	to automatically add a key frame.
	//--------------------------------------------------------------------
	virtual bool HasValueVariation();

	//--------------------------------------------------------------------
	// Returns true if a channel can be keyed.
	//--------------------------------------------------------------------
	virtual bool CanBeKeyed();

private:
	//--------------------------------------------------------------------
	// Callbacks for when property changes, updates original value
	//--------------------------------------------------------------------
	void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	bool m_bScriptedValue;
	prtyTextureFileName& m_Property;
};
