/*****************************************************************************
**	tmlnDriverTextureFileName.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/TextureFileName/tmlnDriverTextureFileName.hpp"

#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Drivers/FilePath/tmlnDriverFilePathInfo.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileNameInfo.hpp"

#include "Core/It/itStringUtil.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverTextureFileName::tmlnDriverTextureFileName(tmlnChannelTextureFileName &i_Channel, chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_Value("Value"), 
	m_ChunkName(i_ChunkName)
{
	m_Value.SetValue(i_Channel.GetValue());
	this->SetBlendType(tmlnDriver::e_NoBlending); // filenames can't blend

	// Register the properties so they can be displayed to the user
	//
	prtyFileChooserUIInfo* pPUII = new prtyFileChooserUIInfo(&(m_Value), "Data", "TextureFileName");
	// get initial directory from the channel
	//pPUII->SetInitialDirectory( i_Channel.GetDirectory() );
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Value.AddCallback(new prtyCallbackWrapper<tmlnDriverTextureFileName>(this, &tmlnDriverTextureFileName::PropertyChanged));

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverTextureFileName::~tmlnDriverTextureFileName()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverTextureFileName::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	if (m_Value.GetValue().GetNumNames() > 0)
		desc += itStringUtil::GetStdString(m_Value.GetValue().GetLastName());

	return desc;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverTextureFileName::SetValue(const prtyTextureFileName& i_Val)
{
	this->MarkDirty();
	m_Value.SetValue(i_Val.GetFullValue());
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverTextureFileName::GetDriverInfo() const
{
	tmlnDriverTextureFileNameInfo *pInfo = new tmlnDriverTextureFileNameInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Value = this->m_Value.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverTextureFileName::SetDriverInfo(const tmlnDriverTextureFileNameInfo& i_Info )
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_Value.SetValue( i_Info.m_Value.GetFullValue() );
}
void tmlnDriverTextureFileName::SetDriverInfo(	const tmlnDriverFilePathInfo& i_Info )
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_Value.SetValue( i_Info.m_Value );
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverTextureFileName::Operate(const maTime& i_Time)
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
bool tmlnDriverTextureFileName::AlterKey()
{
	prtyTextureFileData cur = m_Channel.GetValue();

	// Store local info, using undo
	this->CreateUndoForProperty(m_Value);
	const bool bSetDirty = true;
	this->m_Value.SetValue( cur, bSetDirty  );
	return true;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverTextureFileName::GetClipFillColor() const
{
	return maFloatRGBA( 0.9215f, 1.0f, 0.8431f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverTextureFileName::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
