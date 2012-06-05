/*****************************************************************************
**	cmraDriverFramedOverhead.hpp
**
**		Derived driver for cmra FramedOverhead.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_DRIVERFRAMEDOVERHEAD_HPP
#error cmraDriverFramedOverhead.hpp multiply included
#endif
#define CMRA_DRIVERFRAMEDOVERHEAD_HPP

#ifndef CMRA_DRIVERFRAMEDBASE_HPP
#include "Systems/Cameras/Drivers/cmraDriverFramedBase.hpp"
#endif


//============================================================================
//============================================================================
class cmraDriverFramedBaseInfo;
class tmlnChannelPosition;
class tmlnChannelTarget;


//============================================================================
//============================================================================
class cmraDriverFramedOverhead : public cmraDriverFramedBase
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFramedOverhead(nameUID i_UID, tmlnChannelPosition &i_PChannel, tmlnChannelPosition &i_TChannel);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverFramedOverhead();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

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
	void SetDriverInfo(	const cmraDriverFramedBaseInfo& i_Info );

	//--------------------------------------------------------------------
	//	Active() - called ONCE when a driver first goes active.  This
	//	function needs to reset m_bNotifyNotActive so it doesn't get
	//	anymore calls.
	//--------------------------------------------------------------------
	virtual void Active();

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

private:
	//------------------------------------------------------------------------
	//	calculate the target based on the list of objects
	//------------------------------------------------------------------------
	virtual void Calculate_Target(const maTime& i_fTime, bool i_bForceSet = false);

	//------------------------------------------------------------------------
	//	calculate the position, pitch, etc of the camera.
	//------------------------------------------------------------------------
	virtual void Calculate_CameraLocation(const maTime& i_fTime);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
};
