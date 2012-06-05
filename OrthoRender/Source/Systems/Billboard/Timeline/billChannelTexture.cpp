/*****************************************************************************
**	billChannelTexture.cpp
**
**	 Texture channel for billboards
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Billboard/Timeline/billChannelTexture.hpp"

#include "Systems/Billboard/Object/billBillboardObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
billChannelTexture::billChannelTexture(const char* i_Name, 
									   billBillboardObject* i_pBillboard)
:	tmlnChannel(i_Name), 
	m_pBillboard(i_pBillboard), 
	m_pOriginalTexture(NULL)
{
}


//--------------------------------------------------------------------
//  Set new Texture for object
//--------------------------------------------------------------------
void  billChannelTexture::SetTexture(matTexture *i_pTexture)
{
	m_pBillboard->SetBillboardTexture(i_pTexture);
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" Texture that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  billChannelTexture::SetOriginalTexture(matTexture *i_pTexture)
{
	m_pOriginalTexture = i_pTexture;
	this->MarkDirty();
}
matTexture* billChannelTexture::GetOriginalTexture() const
{
	return m_pOriginalTexture;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void billChannelTexture::Reset()
{
	this->SetTexture( m_pOriginalTexture );
}
