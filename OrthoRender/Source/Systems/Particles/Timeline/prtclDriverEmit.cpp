/*****************************************************************************
**	prtclDriverEmit.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Timeline/prtclDriverEmit.hpp"

//	App
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Systems/Particles/Timeline/prtclChannelEmit.hpp"
#include "Systems/Particles/Timeline/prtclDriverEmitInfo.hpp"
#include "Systems/Particles/Timeline/prtclDriverEmitParser.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclDriverEmit::prtclDriverEmit(prtclChannelEmit &i_Channel)
:	m_Channel(i_Channel)
{
	this->SetRestoreOriginalValue(true);
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void  prtclDriverEmit::Operate(float i_Time)
{
	// nothing needs to be done since the Active() and NotActive()
	// functions get called by the channel.
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string prtclDriverEmit::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  prtclDriverEmit::GetDriverInfo() const
{
	prtclDriverEmitInfo *pInfo = new prtclDriverEmitInfo(prtclDriverEmitParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void prtclDriverEmit::SetDriverInfo(const prtclDriverEmitInfo& i_Info, 
									prtyProperty::UndoFlags i_Undoable)
{
	this->SetBaseDriverInfo(i_Info, i_Undoable);
}

//--------------------------------------------------------------------
// Update - update the object
//--------------------------------------------------------------------
void prtclDriverEmit::Update()
{
}

//--------------------------------------------------------------------
//	NotActive() - called ONCE when a driver goes from active to not
//	active.  This function needs to reset m_bNotifyNotActive so it
//	doesn't get anymore calls.
//--------------------------------------------------------------------
//virtual
void prtclDriverEmit::NotActive()
{
	tmlnDriver::NotActive();

	m_Channel.Deactivate();
}

//--------------------------------------------------------------------
//	Active() - called ONCE when a driver first goes active.  This
//	function needs to reset m_bNotifyNotActive so it doesn't get
//	anymore calls.
//--------------------------------------------------------------------
//virtual
void prtclDriverEmit::Active()
{
	tmlnDriver::Active();

	m_Channel.Activate( this->GetBeginTime(), this->GetDuration() );
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA prtclDriverEmit::GetClipFillColor() const
{
	return maFloatRGBA( 0.8863f, 0.5412f, 1.0f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDriverEmit::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	this->MarkDirty();
}
