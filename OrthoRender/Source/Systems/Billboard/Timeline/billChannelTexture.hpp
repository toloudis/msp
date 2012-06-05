/*****************************************************************************
**	billChannelTexture.hpp
**
**		Texture channel for billboards. This is specific for billboards
**	for now. I recommend making a tmlnChannelTexture if it turns out that
**	some other system needs this kind of channel.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef BILL_CHANNELTEXTURE_HPP
#error billChannelTexture.hpp multiply included
#endif
#define BILL_CHANNELTEXTURE_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

//============================================================================
//============================================================================
class billBillboardObject;
class matTexture;


class billChannelTexture : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	billChannelTexture(const char* i_Name, billBillboardObject* i_pBillboard);

	//--------------------------------------------------------------------
	//  Set new Texture for object
	//--------------------------------------------------------------------
	void  SetTexture(matTexture *i_pTexture);

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" Texture that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalTexture(matTexture *i_pTexture);
	matTexture* GetOriginalTexture() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

private:
	billBillboardObject*	m_pBillboard;
	matTexture*		m_pOriginalTexture;
};
