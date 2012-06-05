/*****************************************************************************
**	cmraDriverKeyTarget.hpp
**
**		Derived driver class for the camera key
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DRIVERKEYTARGET_HPP
#error cmraDriverKeyTarget.hpp multiply included
#endif
#define CMRA_DRIVERKEYTARGET_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class tmlnChannelPosition;
class cmraDriverKeyTargetInfo;


//============================================================================
//============================================================================
class cmraDriverKeyTarget : public tmlnDriver, 
	public tmlnBlendDriver<maVector3d>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverKeyTarget( tmlnChannelPosition& i_ChannelTarget );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverKeyTarget();

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
	void SetDriverInfo(	const cmraDriverKeyTargetInfo& i_Info);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	AlterKey - look at the values in the channels to which this 
	//	driver is connected and alter the driver in order to match 
	//	these values.
	//	Returns true if this driver was able to alter its value.
	//--------------------------------------------------------------------
	virtual bool AlterKey();

	//--------------------------------------------------------------------
	//	Set the target
	//--------------------------------------------------------------------
	void SetTarget( maPoint3d& i_Target );

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

//============================================================================
//	tmlnBlendDriver interface
//============================================================================

	//--------------------------------------------------------------------
	// Get the value of the driver at its begin time for this channel.
	//--------------------------------------------------------------------
	virtual void GetBeginValue(tmlnChannel* i_pChannel, maVector3d& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, maVector3d& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, maVector3d& o_Gradient);

private:
	//--------------------------------------------------------------------
	//	Perform the operate on a single channel
	//--------------------------------------------------------------------
	void OperateChannel( const maTime& i_Time, tmlnChannelPosition& i_Channel, const maPoint3d& i_Goal );

private:
	tmlnChannelPosition&	m_ChannelTarget;

	prtyPoint3d	m_CameraKeyTarget;
};

