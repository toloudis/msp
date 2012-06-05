/*****************************************************************************
**	dcutDriverCapture.hpp
**
**		Derived driver class for turning on camera capturing
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_DRIVERCAPTURE_HPP
#error dcutDriverCapture.hpp multiply included
#endif
#define DCUT_DRIVERCAPTURE_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif


//============================================================================
//============================================================================
class dcutChannelCapture;
class dcutDriverCaptureInfo;
class tmlnChannelBoolean;
class tmlnDriverInfo;


//============================================================================
//============================================================================
class dcutDriverCapture : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dcutDriverCapture(dcutChannelCapture& i_Channel);

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	// Set internal variables from data structure
	//--------------------------------------------------------------------
	void SetDriverInfo(const dcutDriverCaptureInfo& i_Info);

	//--------------------------------------------------------------------
	// Update - update the object
	//--------------------------------------------------------------------
	void Update();

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

private:
	tmlnChannelBoolean &m_Channel;
};
