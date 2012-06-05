/*****************************************************************************
**	sbrdChannelTexture.cpp
**
**	 Texture channel for billboards
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Storyboards/Timeline/sbrdChannelTexture.hpp"

#include "Systems/Storyboards/Object/sbrdBillboardObject.hpp"

#include "Support/tmln/tmlnDriver.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
sbrdChannelTexture::sbrdChannelTexture(const char* i_Name, 
									   sbrdBillboardObject* i_pBillboard)
:	tmlnChannel(i_Name), 
	m_pBillboard(i_pBillboard), 
	m_pOriginalTexture(NULL)
{
}


//--------------------------------------------------------------------
//  Set new Texture for object
//--------------------------------------------------------------------
void  sbrdChannelTexture::SetTexture(matTexture *i_pTexture)
{
	m_pBillboard->SetBillboardTexture(i_pTexture);
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" Texture that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  sbrdChannelTexture::SetOriginalTexture(matTexture *i_pTexture)
{
	m_pOriginalTexture = i_pTexture;
	this->MarkDirty();
}
matTexture* sbrdChannelTexture::GetOriginalTexture() const
{
	return m_pOriginalTexture;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void sbrdChannelTexture::Reset()
{
	this->SetTexture( m_pOriginalTexture );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void sbrdChannelTexture::SwapDriverTimes(int i_Index1, int i_Index2)
{
	float begintime = GetDriver(i_Index1).GetBeginTime();
	this->Driver(i_Index1).SetBeginTime(GetDriver(i_Index2).GetBeginTime());
	this->Driver(i_Index2).SetBeginTime(begintime);

	float endtime = GetDriver(i_Index1).GetEndTime();
	this->Driver(i_Index1).SetEndTime(GetDriver(i_Index2).GetEndTime());
	this->Driver(i_Index2).SetEndTime(endtime);
}


