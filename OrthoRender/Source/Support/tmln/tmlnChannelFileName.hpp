/*****************************************************************************
**	tmlnChannelFileName.hpp
**
**	 Filepath channel
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELFILENAME_HPP
#error tmlnChannelFileName.hpp multiply included
#endif
#define TMLN_CHANNELFILENAME_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif


//============================================================================
//============================================================================
class prtyFileName;
class prtyProperty;


//============================================================================
//============================================================================
class tmlnChannelFileName : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelFileName(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new state for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetValue(const itString& i_Val) = 0;

	//--------------------------------------------------------------------
	//  Get current state of object
	//--------------------------------------------------------------------
	virtual const itString& GetValue() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" state that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalValue(const itString& i_Val);
	const itString& GetOriginalValue() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	// Directory where the filename can be found, needed for UIInfo
	// for drivers attached to this channel.
	//--------------------------------------------------------------------
	const fsLocator& GetDirectory() const;
	void SetDirectory(const fsLocator& i_Locator);

protected:
	itString m_OriginalValue;
	fsLocator m_Directory;
};

//============================================================================
//============================================================================
class tmlnChannelFileNameProperty : public tmlnChannelFileName
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelFileNameProperty(const char* i_Name, prtyFileName& i_Property);

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void  SetValue(const itString& i_Val);

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const itString& GetValue() const;

	//--------------------------------------------------------------------
	// HasValueVariation - returns true if the current value of the
	//	property is different than the scripted value of the channel.
	//	This means the user has edited the values and is an opportunity 
	//	to automatically add a key frame.
	//--------------------------------------------------------------------
	virtual bool HasValueVariation();

private:
	//--------------------------------------------------------------------
	// Callbacks for when property changes, updates original value
	//--------------------------------------------------------------------
	void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	bool m_bScriptedValue;
	prtyFileName& m_Property;
};
