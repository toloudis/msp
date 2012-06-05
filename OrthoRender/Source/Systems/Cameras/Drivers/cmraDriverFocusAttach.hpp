/*****************************************************************************
**	cmraDriverFocusAttach.hpp
**
**	Derived driver class which deals with attachments.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_DRIVERFOCUSATTACH_HPP
#error cmraDriverFocusAttach.hpp multiply included
#endif
#define CMRA_DRIVERFOCUSATTACH_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_ADAPTERREFERENCE_HPP
#include "Drivers/Attach/tmlnAdapterReference.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class tmlnAdapterGetPosition;
class tmlnChannelFloatProperty;
class cmraDriverFocusAttachInfo;
class prtyComboBoxUIInfo;

//============================================================================
//============================================================================
class cmraDriverFocusAttach : public tmlnDriver
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFocusAttach(nameUID i_UID, tmlnChannelFloatProperty &i_NearFocusChannel, 
		tmlnChannelFloatProperty &i_FarFocusChannel, 
		tmlnChannelFloatProperty &i_NearBlurChannel, 
		tmlnChannelFloatProperty &i_FarBlurChannel, 
		chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverFocusAttach();

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
	void SetDriverInfo(	const cmraDriverFocusAttachInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	//	Override base class to have this driver's Operate called at end.
	//--------------------------------------------------------------------
	virtual bool NeedsDelayedOperate();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ObjectNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void AttachNameChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	maMatrix4x4 GetCameraMatrix();
	maAxisBox XForm(const maAxisBox& i_bounds, const maMatrix4x4& i_xform);

private:
	nameUID m_cameraUID;
	tmlnChannelFloatProperty &m_ChannelNearFocus;
	tmlnChannelFloatProperty &m_ChannelFarFocus;
	tmlnChannelFloatProperty &m_ChannelNearBlur;
	tmlnChannelFloatProperty &m_ChannelFarBlur;
	tmlnAdapterReference m_Reference;
	chDefs::Name m_ChunkName;

	prtyName	m_ObjectName;
	prtyText	m_AttachName;
	prtyFloat	m_NearFocusOffset;
	prtyFloat	m_FarFocusOffset;
	prtyFloat	m_NearFalloffDist;
	prtyFloat	m_FarFalloffDist;

	// Combo box of object names, update it when we open the dialog
	prtyComboBoxUIInfo* m_pObjectNameUIInfo;
	// Combo box of attachment nodes, need to change it when we 
	// attach to a new object.
	prtyComboBoxUIInfo* m_pAttachNameUIInfo;

	bool m_bNeedsAttach;
};
