/*****************************************************************************
**	cmraDriverFramedArc.hpp
**
**		Derived driver for cmra Arc.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DRIVERFRAMEDARC_HPP
#error cmraDriverFramedArc.hpp multiply included
#endif
#define CMRA_DRIVERFRAMEDARC_HPP

#ifndef CMRA_DRIVERFRAMEDBASE_HPP
#include "Systems/Cameras/Drivers/cmraDriverFramedBase.hpp"
#endif
#ifndef CMRA_DRIVERDATAPOSITIONINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataPositionInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATASUBJECTINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataSubjectInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATAVIEWTYPEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeInfo.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class cmraDriverFramedArcInfo;


//============================================================================
//============================================================================
class cmraDriverFramedArc : public cmraDriverFramedBase
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFramedArc( nameUID i_UID, tmlnChannelPosition &i_PChannel, tmlnChannelPosition &i_TChannel );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverFramedArc();

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
	void SetDriverInfo(	const cmraDriverFramedArcInfo& i_Info, 
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
	virtual void  Operate(float i_fTime);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

protected:
	//--------------------------------------------------------------------
	//	Set the begin time of the driver
	//--------------------------------------------------------------------
	virtual void SetBeginTime(float i_fBeginTime);

private:
	//------------------------------------------------------------------------
	//	calculate the target based on the list of objects
	//------------------------------------------------------------------------
	virtual void Calculate_Target(float i_fTime, bool i_bForceSet = false);

	//------------------------------------------------------------------------
	//	calculate the position, pitch, etc of the camera.
	//------------------------------------------------------------------------
	virtual void Calculate_CameraLocation(float i_fTime);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	//	interface variables
	prtyFloat	m_fRevolutions;
	prtyBoolean	m_bClockwise;

	//	use variables
	float		m_ArcStartTime;
};
