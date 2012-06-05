/*****************************************************************************
**	sbrdChannelTexture.hpp
**
**		Texture channel for billboards. This is specific for billboards
**	for now. I recommend making a tmlnChannelTexture if it turns out that
**	some other system needs this kind of channel.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef SBRD_CHANNELTEXTURE_HPP
#error sbrdChannelTexture.hpp multiply included
#endif
#define SBRD_CHANNELTEXTURE_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

//============================================================================
//============================================================================
class sbrdBillboardObject;
class matTexture;


class sbrdChannelTexture : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	sbrdChannelTexture(const char* i_Name, sbrdBillboardObject* i_pBillboard);

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

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SwapDriverTimes(int i_Index1, int i_Index2);

private:
	sbrdBillboardObject*	m_pBillboard;
	matTexture*		m_pOriginalTexture;
};
