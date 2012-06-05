/*****************************************************************************
**	tmlnDriverPosition.hpp
**
**		Derived driver class which sets a position keyframe
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERPOSITION_HPP
#error tmlnDriverPosition.hpp multiply included
#endif
#define TMLN_DRIVERPOSITION_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef PNT_POINT_HPP
#include "Support/pnt/pntPoint.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelPosition;
class tmlnDriverPositionInfo;
class sel3dObject;


//============================================================================
//============================================================================
class tmlnDriverPosition : public tmlnDriver, 
	public pntPoint::PointChangedCallback,
	public tmlnBlendDriver<maVector3d>
{
public:
	//--------------------------------------------------------------------
	// The parent object pointer should be the tmlnScriptObject that
	//	 this driver will control. It is used to associate the 
	//	 driver icons back to the main object.
	//--------------------------------------------------------------------
	tmlnDriverPosition(tmlnChannelPosition &i_Channel, 
					   chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverPosition();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//	Quick Accessors
	//--------------------------------------------------------------------
	inline const maPoint3d& GetValue() const;
	void SetValue(const maPoint3d& i_Val);

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
	void SetDriverInfo(const tmlnDriverPositionInfo& i_Info );

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
	//  This is called when a driver is selected. It lets the driver
	//		select one of its icons or whatever else it wants to do.
	//--------------------------------------------------------------------
	virtual void  DoSelect();

	//--------------------------------------------------------------------
	//  Driver should select its 3D icon
	//--------------------------------------------------------------------
	virtual void  DoSelectIcon();

	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	virtual void ShowIcons( bool i_bVisible );

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	// Callback when point has been changed from manipulator
	//--------------------------------------------------------------------
	virtual void PointChanged(pntPoint *i_pPoint);

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
	//--------------------------------------------------------------------
	void PositionChanged(prtyProperty *i_pProperty, bool i_bDirty);

	tmlnChannelPosition&	m_PChannel;
	pntPoint				m_Point;
	chDefs::Name			m_ChunkName;
	prtyPoint3d				m_Position;
};


//--------------------------------------------------------------------
//	Quick Accessors
//--------------------------------------------------------------------
const maPoint3d& tmlnDriverPosition::GetValue() const
{
	//return m_Point.GetPosition();
	return m_Position.GetValue();
}
