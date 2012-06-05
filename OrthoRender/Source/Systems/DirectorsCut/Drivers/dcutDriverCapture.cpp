/*****************************************************************************
**	dcutDriverCapture.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Drivers/dcutDriverCapture.hpp"

#include "Systems/DirectorsCut/Timeline/dcutChannelCapture.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCaptureInfo.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCaptureParser.hpp"

#include "Support/mnm/mnmDebugInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDriverCapture::dcutDriverCapture(dcutChannelCapture& i_Channel)
:	m_Channel(i_Channel)
{
	this->SetRestoreOriginalValue(true);
}

//--------------------------------------------------------------------
//  Update the object that is being driven
//--------------------------------------------------------------------
void  dcutDriverCapture::Operate(float i_Time)
{
	// nothing needs to be done
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string dcutDriverCapture::GetHoverDescription()
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
tmlnDriverInfo*  dcutDriverCapture::GetDriverInfo() const
{
	dcutDriverCaptureInfo *pInfo = new dcutDriverCaptureInfo(dcutDriverCaptureParser::GetChunkName());
	this->GetBaseDriverInfo(*pInfo);

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void dcutDriverCapture::SetDriverInfo(const dcutDriverCaptureInfo& i_Info)
{
	this->SetBaseDriverInfo(i_Info);
}

//--------------------------------------------------------------------
// Update - update the object
//--------------------------------------------------------------------
void dcutDriverCapture::Update()
{
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA dcutDriverCapture::GetClipFillColor() const
{
	return maFloatRGBA( 0.2f, 0.8f, 0.3f, 1.0f );
}
