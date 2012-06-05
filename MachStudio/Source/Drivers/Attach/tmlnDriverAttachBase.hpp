/*****************************************************************************
**	tmlnDriverAttachBase.hpp
**
**	Base driver class which deals with attachments.
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERATTACHBASE_HPP
#error tmlnDriverAttachBase.hpp multiply included
#endif
#define TMLN_DRIVERATTACHBASE_HPP

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

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class tmlnAdapterGetPosition;
class tmlnChannelPosition;
class tmlnDriverAttachBaseInfo;
class prtyComboBoxUIInfo;


//============================================================================
//============================================================================
class tmlnDriverAttachBase : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	// Constructor initializes properties and registers callbacks.
	// World space offset is initialized based on given argument.
	//--------------------------------------------------------------------
	tmlnDriverAttachBase(const maPoint3d& i_InitialPosition);

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	//	Override base class to have this driver's Operate called at end.
	//--------------------------------------------------------------------
	virtual bool NeedsDelayedOperate();

	//--------------------------------------------------------------------
	//	Remap internal name attachments using the given map.
	//  This is part of the duplication process and makes sures 
	//	internal attachments are passed onto the duplicated objects.
	//--------------------------------------------------------------------
	virtual void RemapNames(const std::map<nameString, nameString> &i_DuplicateNameMap);

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void ObjectNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void AttachNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void AttachOffsetChanged(prtyProperty *i_pProperty, bool i_bDirty);

protected:
	//--------------------------------------------------------------------
	// Check to see if we need to attach to a new object.
	// Should be called at the beginning of Operate().
	//--------------------------------------------------------------------
	void  ConfirmAttachment();

	tmlnAdapterReference m_Reference;

	prtyName	m_ObjectName;
	prtyText	m_AttachName;
	prtyPoint3d	m_AttachOffset;
	prtyPoint3d	m_WorldSpaceOffset;

	// Combo box of object names, update it when we open the dialog
	prtyComboBoxUIInfo* m_pObjectNameUIInfo;
	// Combo box of attachment nodes, need to change it when we 
	// attach to a new object.
	prtyComboBoxUIInfo* m_pAttachNameUIInfo;

	bool m_bNeedsAttach;
	bool m_bPreservePosition;

	// This property is shared by all attachment drivers
	static prtyBoolean sm_bPreservePosition;
};
