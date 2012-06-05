/*****************************************************************************
**	tmlnChannelRenderPass.cpp
**
**	 RenderPass channel
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelRenderPass.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelRenderPass::tmlnChannelRenderPass(const char* i_Name)
:	tmlnChannel(i_Name),
	m_OriginalTexValue(true),
	m_OriginalFloatValue("OrigFloat",1.0f),
	m_OriginalIntValue("OrigInt",0)
{
}


//--------------------------------------------------------------------
//	The channel has the idea of an "original" state that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelRenderPass::SetOriginalTexValue(const prtyTextureFileData& i_Val)
{
	m_OriginalTexValue = i_Val;
	this->MarkDirty();
}
const prtyTextureFileData& tmlnChannelRenderPass::GetOriginalTexValue() const
{
	return m_OriginalTexValue;
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" state that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelRenderPass::SetOriginalFloatValue(const prtyFloat& i_Val)
{
	m_OriginalFloatValue = i_Val;
	this->MarkDirty();
}
const prtyFloat& tmlnChannelRenderPass::GetOriginalFloatValue() const
{
	return m_OriginalFloatValue;
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" state that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelRenderPass::SetOriginalIntValue(const prtyInt32& i_Val)
{
	m_OriginalIntValue = i_Val;
	this->MarkDirty();
}
const prtyInt32& tmlnChannelRenderPass::GetOriginalIntValue() const
{
	return m_OriginalIntValue;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelRenderPass::Reset()
{
	this->SetTexValue( m_OriginalTexValue );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelRenderPassProperty::tmlnChannelRenderPassProperty(prtyTextureFileName& i_PropertyTex, prtyFloat& i_PropertyScalar, prtyInt32& i_PropertyInt)
:	tmlnChannelRenderPass(i_PropertyTex.GetPropertyName().c_str()),
	m_PropertyTex(i_PropertyTex), 
	m_PropertyFloat(i_PropertyScalar),
	m_PropertyInt(i_PropertyInt),
	m_bScriptedValue(false)
{	
	m_PropertyTex.SetAnimatable(false);
	m_PropertyTex.AddCallback(new prtyCallbackWrapper<tmlnChannelRenderPassProperty>(this, &tmlnChannelRenderPassProperty::ValueChanged));
	m_PropertyFloat.AddCallback(new prtyCallbackWrapper<tmlnChannelRenderPassProperty>(this, &tmlnChannelRenderPassProperty::ValueChanged));
	m_PropertyInt.AddCallback(new prtyCallbackWrapper<tmlnChannelRenderPassProperty>(this, &tmlnChannelRenderPassProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new state for property
//--------------------------------------------------------------------
void  tmlnChannelRenderPassProperty::SetTexValue(const prtyTextureFileData& i_Val)
{
	prtyTextureFileData val;
	val.m_TextureLocator = i_Val.m_TextureLocator;
	m_PropertyTex.SetValue(val);
	m_bScriptedValue = false;
}

//--------------------------------------------------------------------
//  Set new state for property
//--------------------------------------------------------------------
void  tmlnChannelRenderPassProperty::SetFloatValue(const prtyFloat& i_Val)
{
	m_PropertyFloat = i_Val;
}

//--------------------------------------------------------------------
//  Set new state for property
//--------------------------------------------------------------------
void  tmlnChannelRenderPassProperty::SetIntValue(const envType::Int8& i_Val)
{
	m_PropertyInt.SetValue( i_Val );
}

//--------------------------------------------------------------------
//  Get current state of property
//--------------------------------------------------------------------
const prtyTextureFileData& tmlnChannelRenderPassProperty::GetTexValue() const
{
	return m_PropertyTex.GetFullValue();
}

//--------------------------------------------------------------------
//  Get current state of property
//--------------------------------------------------------------------
const prtyFloat& tmlnChannelRenderPassProperty::GetFloatValue() const
{
	return m_PropertyFloat;
}

//--------------------------------------------------------------------
//  Get current state of property
//--------------------------------------------------------------------
const prtyInt32& tmlnChannelRenderPassProperty::GetIntValue() const
{
	return m_PropertyInt;
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelRenderPassProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
	{
		m_OriginalTexValue = m_PropertyTex.GetFullValue();
		m_OriginalFloatValue = m_PropertyFloat.GetValue();
		m_OriginalIntValue = m_PropertyInt.GetValue();

	}
	m_bScriptedValue = false;
}

