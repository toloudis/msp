/*****************************************************************************
**	cmraDriverDirector.hpp
**
**		Derived driver class for turning on camera director
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DRIVERDIRECTOR_HPP
#error cmraDriverDirector.hpp multiply included
#endif
#define CMRA_DRIVERDIRECTOR_HPP

#ifndef CMRA_DRIVERCAPTURE_HPP
#include "Systems/Cameras/Drivers/cmraDriverCapture.hpp"
#endif


//============================================================================
//============================================================================
class cmraChannelCapture;


//============================================================================
//============================================================================
class cmraDriverDirector : public cmraDriverCapture
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverDirector(cmraChannelCapture& i_Channel);

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;
};
