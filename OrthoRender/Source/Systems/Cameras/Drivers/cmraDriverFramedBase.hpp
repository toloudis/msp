/*****************************************************************************
**	cmraDriverFramedBase.hpp
**
**		Derived driver for cmra FramedBase.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_DRIVERFRAMEDBASE_HPP
#error cmraDriverFramedBase.hpp multiply included
#endif
#define CMRA_DRIVERFRAMEDBASE_HPP

#ifndef CMRA_DRIVERDATAPOSITIONINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataPositionInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATASUBJECTINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataSubjectInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATAVIEWTYPEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeInfo.hpp"
#endif

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif


//============================================================================
//============================================================================
class mnmObject;
class cmraDriverFramedBaseInfo;
class tmlnAdapterGetPosition;
class tmlnChannelPosition;
class tmlnChannelTarget;


//============================================================================
//============================================================================
class cmraDriverFramedBase : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFramedBase(nameUID i_UID, tmlnChannelPosition &i_PChannel, tmlnChannelPosition &i_TChannel);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverFramedBase();

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
	void SetDriverInfo(	const cmraDriverFramedBaseInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//	Active() - called ONCE when a driver first goes active.  This
	//	function needs to reset m_bNotifyNotActive so it doesn't get
	//	anymore calls.
	//--------------------------------------------------------------------
	virtual void Active();

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

protected:
	//--------------------------------------------------------------------
	//	Set the begin time of the driver
	//--------------------------------------------------------------------
	virtual void SetBeginTime(float i_BeginTime);

	//--------------------------------------------------------------------
	//	Calculate the Start variables based on the info + target
	//
	//	Note: this assumes the m_StartPositionInfo.m_CamTarget and
	//	m_StartPositionInfo.m_CamPosition has already been set.
	//--------------------------------------------------------------------
	virtual void Calculate_StartVariables();

	//------------------------------------------------------------------------
	//	Build the target list from the data subject info
	//------------------------------------------------------------------------
	virtual void BuildTargetList();

	//------------------------------------------------------------------------
	//	calculate the target based on the list of objects
	//------------------------------------------------------------------------
	virtual void Calculate_Target(float i_fTime, bool i_bForceSet = false);

	//------------------------------------------------------------------------
	//	calculate the position, pitch, etc of the camera.
	//------------------------------------------------------------------------
	virtual void Calculate_CameraLocation(float i_fTime);

	//------------------------------------------------------------------------
	//	calculate the position of the point projected into the camera view.
	//	values will be between -1.0 and 1.0.
	//------------------------------------------------------------------------
	virtual void Calculate_ViewPos( maPoint3d& i_Point, maPoint4d& o_View );

	//------------------------------------------------------------------------
	//	calculate a close (and valid) position based on the (normalized) view  
	//	values
	//------------------------------------------------------------------------
	virtual void Calculate_TargetPos( maPoint4d& i_View, maPoint3d& i_TargetPoint, maPoint3d& o_Point );

	//------------------------------------------------------------------------
	//	calculate the bounding box represented by the selected objects
	//------------------------------------------------------------------------
	virtual void Calculate_TargetWorldBox();

	//------------------------------------------------------------------------
	//	calculate the one-time driver data
	//------------------------------------------------------------------------
	virtual void Calculate_DriverData();

	//------------------------------------------------------------------------
	//	calculate the each-frame driver data once
	//------------------------------------------------------------------------
	virtual void Calculate_FrameData();

	//------------------------------------------------------------------------
	//	see if the new point is valid. if so, return true
	//------------------------------------------------------------------------
	virtual bool CheckIfPositionValid( maPoint3d& i_NewPoint );

	//------------------------------------------------------------------------
	//	see if the new direction is valid. if so return true
	//------------------------------------------------------------------------
	virtual bool CheckIfDirectionValid( maVector3d& i_NewDir );

	//------------------------------------------------------------------------
	//	see if the new distance (squared) is valid. if so return true
	//------------------------------------------------------------------------
	virtual bool CheckIfDistanceValid( float i_fNewDistSqr );

	//------------------------------------------------------------------------
	//	see if the new angle is valid. if so return true
	//------------------------------------------------------------------------
	virtual bool CheckIfAngleValid( float i_fNewAngle );

	//------------------------------------------------------------------------
	//	set the min and max for the form variables
	//------------------------------------------------------------------------
	//virtual void Configure_PositionForm(StudioFramework::cmraDriverDataPositionForm * i_pPForm);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

protected:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void display_data();

protected:
	tmlnChannelPosition &	m_PositionChannel;
	tmlnChannelPosition &	m_TargetChannel;

	//	interface variables
	prtyBoolean		m_bPositionRestrict;
	prtyPoint3d		m_PositionTolerance;
	prtyBoolean		m_bAngleRestrict;
	prtyPoint3d		m_AngleTolerance;
	prtyBoolean		m_bDistanceRestrict;
	prtyPoint3d		m_DistanceTolerance;
	prtyBoolean		m_bDirectionRestrict;
	prtyFloat		m_fDirectionTolerance;
	nameUID			m_CameraUID;

	//	forms + data
	cmraDriverDataPositionInfo	m_StartPositionInfo;
	cmraDriverDataSubjectInfo	m_SubjectInfo;
	cmraDriverDataViewTypeInfo	m_ViewTypeInfo;

	//	use variables
	maPoint3d	m_Position;
	maPoint3d	m_PositionLast;
	maPoint3d	m_PositionOrig;
	maPoint3d	m_Target;
	maPoint3d	m_TargetLast;
	maPoint3d	m_TargetOrig;
	float		m_fRadius;
	float		m_fRadiusOrig;
	float		m_fTargetYawAngle;
	float		m_fTargetYawAngleOrig;
	float		m_fPitchAngle;
	float		m_fPitchAngleOrig;
	float		m_fFramedStartTime;
	float		m_fLastTime;

	//	per-frame variables
	maVector3d	m_Target_FrameDiff;
	float		m_fPositionToleranceLenSqr;
	maVector3d	m_DirOrigNorm;
	float		m_fDirOrigLenSqr;
	float		m_fDistanceToleranceLenSqr;
	maAxisBox	m_TargetWorldBox;

	//
	std::vector<mnmObject*> m_Objects;
};
