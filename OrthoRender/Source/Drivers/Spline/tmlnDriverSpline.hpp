/*****************************************************************************
**	tmlnDriverSpline.hpp
**
**	Derived driver class which implements Spline motion
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERSPLINE_HPP
#error tmlnDriverSpline.hpp multiply included
#endif
#define TMLN_DRIVERSPLINE_HPP

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
#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif 


//============================================================================
class tmlnChannelPosition;
class tmlnDriverSplineInfo;
class splnSpline;
class pick3dPickObject;


class tmlnDriverSpline : public tmlnDriver,
	public tmlnBlendDriver<maVector3d>
{
public:
	//--------------------------------------------------------------------
	// The parent object pointer should be the tmlnScriptObject that
	//	 this driver will control. It is used to associate the 
	//	 driver icons back to the main object.
	//--------------------------------------------------------------------
	tmlnDriverSpline(tmlnChannelPosition &i_PChannel, 
					 chDefs::Name i_ChunkName, 
					 pick3dPickObject* i_pParent);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverSpline();

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
	virtual void SetDriverInfo(const tmlnDriverSplineInfo& i_Info, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	//virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	// Spline
	//--------------------------------------------------------------------
	virtual splnSpline& Spline();
	virtual const splnSpline& GetSpline() const;
	virtual void  SetSpline(const splnSpline &i_Spline);

	//--------------------------------------------------------------------
	// Update - if you are going to alter the spline directly,
	//	call this to update its representation
	//--------------------------------------------------------------------
	virtual void Update();

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
	// Return the current position of the channel, used to place
	//	new control points in position of object on channel.
	//--------------------------------------------------------------------
	maPoint3d GetChannelCurrentPosition() const;

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
	// private functions to handle button callbacks
	//--------------------------------------------------------------------
	//void select_first_point();
	//void select_all_points();
	//void append_after_selected();
	void append_at_channel_value();

	//--------------------------------------------------------------------
	// button click callbacks
	//--------------------------------------------------------------------
	void ButtonClicked(prtyProperty *i_pProperty, bool i_bDirty);
	void AppendAtObjectPositionClicked(prtyProperty *i_pProperty, bool i_bDirty);

protected:
	splnSpline *m_pSpline;
	tmlnChannelPosition &m_ChannelP;
	chDefs::Name m_ChunkNameDS;
	//prtyTrigger m_SelectFirstPoint;
	//prtyTrigger m_SelectAllPoints;
	//prtyTrigger m_AppendAfterSelected;
	prtyTrigger m_AppendAtObjectPosition;
};
