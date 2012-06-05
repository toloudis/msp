/*****************************************************************************
**	tmlnChannelRenderPass.hpp
**
**	 RenderPass channel
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELRENDERPASS_HPP
#error tmlnChannelFilePath.hpp multiply included
#endif
#define TMLN_CHANNELRENDERPASS_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
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

//============================================================================
//============================================================================
class prtyProperty;


//============================================================================
//============================================================================
class tmlnChannelRenderPass : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelRenderPass(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new state for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetTexValue(const prtyTextureFileData& i_Val) = 0;

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void  SetFloatValue(const prtyFloat& i_Val) = 0;

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void SetIntValue(const envType::Int8& i_Val) = 0;

	//--------------------------------------------------------------------
	//  Get current state of object
	//--------------------------------------------------------------------
	virtual const prtyTextureFileData& GetTexValue() const = 0;

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const prtyFloat& GetFloatValue() const = 0;

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const prtyInt32& GetIntValue() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" state that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalTexValue(const prtyTextureFileData& i_Val);
	const prtyTextureFileData& GetOriginalTexValue() const;

	void  SetOriginalFloatValue(const prtyFloat& i_Val);
	const prtyFloat& GetOriginalFloatValue() const;

	void  SetOriginalIntValue(const prtyInt32& i_Val);
	const prtyInt32& GetOriginalIntValue() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	prtyTextureFileData m_OriginalTexValue;
	prtyFloat			m_OriginalFloatValue;
	prtyInt32			m_OriginalIntValue;
};

//============================================================================
//============================================================================
class tmlnChannelRenderPassProperty : public tmlnChannelRenderPass
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelRenderPassProperty(prtyTextureFileName& i_PropertyTex, prtyFloat& i_PropertyScalar, prtyInt32& i_PropertyInt);

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void  SetTexValue(const prtyTextureFileData& i_Val);

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void  SetFloatValue(const prtyFloat& i_Val);

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void SetIntValue(const envType::Int8& i_Val);

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const prtyTextureFileData& GetTexValue() const;

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const prtyFloat& GetFloatValue() const;

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual const prtyInt32& GetIntValue() const;

private:
	//--------------------------------------------------------------------
	// Callbacks for when property changes, updates original value
	//--------------------------------------------------------------------
	void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	bool m_bScriptedValue;
	prtyTextureFileName&	m_PropertyTex;
	prtyFloat&				m_PropertyFloat;
	prtyInt32&				m_PropertyInt;
};
