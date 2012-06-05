/*****************************************************************************
**	tmlnChannelFilePath.hpp
**
**	 Filepath channel
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELFILEPATH_HPP
#error tmlnChannelFilePath.hpp multiply included
#endif
#define TMLN_CHANNELFILEPATH_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//============================================================================
class prtyFilePath;
class prtyProperty;


//============================================================================
//============================================================================
class tmlnChannelFilePath : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelFilePath(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new state for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetValue(const fsLocator& i_Val) = 0;

	//--------------------------------------------------------------------
	//  Get current state of object
	//--------------------------------------------------------------------
	virtual const fsLocator& GetValue() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" state that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalValue(const fsLocator& i_Val);
	const fsLocator& GetOriginalValue() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	fsLocator m_OriginalValue;
};

//============================================================================
//============================================================================
class tmlnChannelFilePathProperty : public tmlnChannelFilePath
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelFilePathProperty(const char* i_Name, prtyFilePath& i_Property);

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void  SetValue(const fsLocator& i_Val);

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const fsLocator& GetValue() const;

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
	prtyFilePath& m_Property;
};
