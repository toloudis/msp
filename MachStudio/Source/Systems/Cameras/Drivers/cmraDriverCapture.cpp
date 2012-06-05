/*****************************************************************************
**	cmraDriverCapture.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverCapture.hpp"

#include "Systems/Cameras/Timeline/cmraChannelCapture.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCaptureInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCaptureParser.hpp"

#include "Support/mnm/mnmDebugInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverCapture::cmraDriverCapture(cmraChannelCapture& i_Channel)
:	m_Channel(i_Channel)
{
	this->SetRestoreOriginalValue(true);
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void cmraDriverCapture::Operate(const maTime& i_Time)
{
	// nothing needs to be done since the Active() and NotActive()
	// functions get called by the channel.
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverCapture::GetHoverDescription()
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
tmlnDriverInfo* cmraDriverCapture::GetDriverInfo() const
{
	cmraDriverCaptureInfo *pInfo = new cmraDriverCaptureInfo(cmraDriverCaptureParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverCapture::SetDriverInfo(	const cmraDriverCaptureInfo& i_Info)
{
	this->SetBaseDriverInfo(i_Info);
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverCapture::GetClipFillColor() const
{
	return maFloatRGBA( 0.2f, 0.8f, 0.3f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverCapture::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
