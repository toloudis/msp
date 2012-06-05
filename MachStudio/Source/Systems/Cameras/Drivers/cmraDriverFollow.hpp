/*****************************************************************************
**	cmraDriverFollow.hpp
**
**		driver for camera driver Follow.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DRIVERFOLLOW_HPP
#error cmraDriverFollow.hpp multiply included
#endif
#define CMRA_DRIVERFOLLOW_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelPosition;
class cmraDriverFollowInfo;


//============================================================================
//============================================================================
class cmraDriverFollow : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFollow(tmlnChannelPosition &i_PChannel, tmlnChannelPosition &i_TChannel);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverFollow();

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
	void SetDriverInfo(	const cmraDriverFollowInfo& i_Info);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_fTime);

	//--------------------------------------------------------------------
	//	AlterKey - look at the values in the channels to which this 
	//	driver is connected and alter the driver in order to match 
	//	these values.
	//	Returns true if this driver was able to alter its value.
	//--------------------------------------------------------------------
	virtual bool AlterKey();

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	//	Clone - Clone this driver and return a new instace
	//--------------------------------------------------------------------
	virtual tmlnDriver* Clone();

private:
	//------------------------------------------------------------------------
	//	calculate the position, pitch, etc of the camera.
	//------------------------------------------------------------------------
	void Calculate_CameraLocation(const maTime& i_fTime);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnChannelPosition &m_PChannel;	// position
	tmlnChannelPosition &m_TChannel;	// target

private:
	prtyVector3d	m_Direction;
};

