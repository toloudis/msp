/*****************************************************************************
**	dcutDriverCamera.hpp
**
**		Derived driver class for turning on camera capturing
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_DRIVERCAMERA_HPP
#error dcutDriverCamera.hpp multiply included
#endif
#define DCUT_DRIVERCAMERA_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif


//============================================================================
//============================================================================
class dcutChannelCamera;
class dcutDriverCameraInfo;
class prtyComboBoxUIInfo;
class tmlnDriverInfo;


//============================================================================
//============================================================================
class dcutDriverCamera : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dcutDriverCamera(dcutChannelCamera& i_Channel);

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
	void SetDriverInfo(const dcutDriverCameraInfo& i_Info,
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	// Return name of camera being indexed
	//--------------------------------------------------------------------
	const nameString& GetCameraName();

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CameraNameChanged(prtyProperty *i_pProperty, bool i_bDirty);

	dcutChannelCamera &m_Channel;
	prtyName	m_CameraName;
	prtyComboBoxUIInfo* m_pCameraNameUIInfo;
};
