/*****************************************************************************
**	tmlnDriverOrientation.hpp
**
**		Derived driver class which sets a orientation keyframe
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERORIENTATION_HPP
#error tmlnDriverOrientation.hpp multiply included
#endif
#define TMLN_DRIVERORIENTATION_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelOrientation;
class tmlnDriverOrientationInfo;


//============================================================================
//============================================================================
class tmlnDriverOrientation : public tmlnDriver, 
	public tmlnBlendDriver<maRotation>,
	public tmlnBlendDriver<maVector3d>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverOrientation(tmlnChannelOrientation &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverOrientation();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//	Quick Accessors
	//--------------------------------------------------------------------
	//inline const maRotation& GetValue() const;
	//void SetValue(const maRotation& i_Val);
	void SetEulerInterpolation(bool i_bVal);

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
	void SetDriverInfo(	const tmlnDriverOrientationInfo& i_Info );

	//--------------------------------------------------------------------
	//  Update orientation of things that are being driven
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
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

//============================================================================
//	tmlnBlendDriver interface
//============================================================================

	//--------------------------------------------------------------------
	// Get the value of the driver at its begin time for this channel.
	//--------------------------------------------------------------------
	virtual void GetBeginValue(tmlnChannel* i_pChannel, maRotation& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, maRotation& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, maRotation& o_Gradient);

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
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	// Quaternion and Euler interpolation variations
	//--------------------------------------------------------------------
	void  OperateQuaternion(const maTime& i_Time);
	void  OperateEuler(const maTime& i_Time);

private:
	tmlnChannelOrientation &m_Channel;
	prtyRotation	m_Value;
	prtyBoolean		m_bEulerInterpolation;

	chDefs::Name m_ChunkName;

};

//--------------------------------------------------------------------
//	Quick Accessors
//--------------------------------------------------------------------
//const maRotation& tmlnDriverOrientation::GetValue() const
//{
//	return m_Value.GetValue();
//}
