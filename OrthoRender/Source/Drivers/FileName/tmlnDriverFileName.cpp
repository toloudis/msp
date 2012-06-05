/*****************************************************************************
**	tmlnDriverFileName.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/FileName/tmlnDriverFileName.hpp"

#include "Support/tmln/tmlnChannelFileName.hpp"
#include "Drivers/FileName/tmlnDriverFileNameInfo.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFileName::tmlnDriverFileName(tmlnChannelFileName &i_Channel, chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_Value("Value",i_Channel.GetValue()), 
	m_ChunkName(i_ChunkName)
{
	this->SetBlendType(tmlnDriver::e_NoBlending); // filenames can't blend

	// Register the properties so they can be displayed to the user
	//
	prtyFileChooserUIInfo* pPUII = new prtyFileChooserUIInfo(&(m_Value), "Data", "FileName");
	// get initial directory from the channel
	pPUII->SetInitialDirectory( i_Channel.GetDirectory() );
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Value.AddCallback(new prtyCallbackWrapper<tmlnDriverFileName>(this, &tmlnDriverFileName::PropertyChanged));

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFileName::~tmlnDriverFileName()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverFileName::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	desc += itStringUtil::GetStdString(m_Value.GetValue());

	return desc;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverFileName::SetValue(const itString& i_Val)
{
	this->MarkDirty();
	m_Value.SetValue(i_Val);
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverFileName::GetDriverInfo() const
{
	tmlnDriverFileNameInfo *pInfo = new tmlnDriverFileNameInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Value = this->m_Value.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverFileName::SetDriverInfo(const tmlnDriverFileNameInfo& i_Info, 
									prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_Value.SetValue( i_Info.m_Value, i_Undoable );
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverFileName::Operate(float i_Time)
{
	// Nothing to do when blending, wait until we are within the 
	// driver's time range
	if (!IsBefore(i_Time))
	{
		m_Channel.SetValue(this->m_Value.GetValue());
	}
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool tmlnDriverFileName::AlterKey()
{
	itString cur = m_Channel.GetValue();

	// Store local info, using undo
	this->m_Value.SetValue( cur, prtyProperty::eNewUndo  );
	return true;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverFileName::GetClipFillColor() const
{
	return maFloatRGBA( 0.9215f, 1.0f, 0.8431f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverFileName::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
