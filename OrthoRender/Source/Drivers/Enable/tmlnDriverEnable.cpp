/*****************************************************************************
**	tmlnDriverEnable.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Enable/tmlnDriverEnable.hpp"

#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Drivers/Enable/tmlnDriverEnableInfo.hpp"
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverEnable::tmlnDriverEnable(tmlnChannelBoolean &i_Channel, 
								   chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_ChunkName(i_ChunkName),
	m_bEnabled("Enabled", true)
{
	//this->SetRestoreOriginalValue(true);

	//	set the value of the properties
	//
	m_bEnabled.SetValue(!i_Channel.GetState());

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyCheckBoxUIInfo(&(m_bEnabled), "State", "Enabled");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_bEnabled.AddCallback(new prtyCallbackWrapper<tmlnDriverEnable>(this, &tmlnDriverEnable::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverEnable::~tmlnDriverEnable()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverEnable::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	char buffer[128];
	::sprintf(buffer, "Enabled: %s", m_bEnabled.GetValue() ? "true" : "false");

	desc += std::string(buffer);
	return desc;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverEnable::SetEnabled(bool i_Val)
{
	m_bEnabled.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverEnable::GetDriverInfo() const
{
	tmlnDriverEnableInfo *pInfo = new tmlnDriverEnableInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Enabled = this->m_bEnabled.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverEnable::SetDriverInfo(	const tmlnDriverEnableInfo& i_Info, 
										prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_bEnabled.SetValue(i_Info.m_Enabled, i_Undoable);
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverEnable::Operate(float i_Time)
{
	if (!IsBefore(i_Time))
	{
		m_Channel.SetState(this->m_bEnabled.GetValue());
	}
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool tmlnDriverEnable::AlterKey()
{
	bool cur = m_Channel.GetState();

	// Store local info, using undo
	this->m_bEnabled.SetValue( cur, prtyProperty::eNewUndo  );

	return true;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverEnable::GetClipFillColor() const
{
	// It would be nice to color the driver based on the off/on state, 
	// but for that to work, the driver clips would need to update when the
	// color changed and that doesn't work right now.
	//if (this->m_bEnabled.GetValue())
		return maFloatRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	//else
	//	return maFloatRGBA( 0.6f, 0.6f, 0.6f, 1.0f );
}

//--------------------------------------------------------------------
//	Clone - Clone this driver and return a new instace
//--------------------------------------------------------------------
//virtual 
tmlnDriver* tmlnDriverEnable::Clone()
{
	return new tmlnDriverEnable(*this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverEnable::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
