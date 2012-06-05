/*****************************************************************************
**	tmlnDriverAttachBase.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Attach/tmlnDriverAttachBase.hpp"

#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"

#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"

//============================================================================
//============================================================================
namespace
{
}

//============================================================================
// Static property, shared by all attachment drivers
//============================================================================
prtyBoolean tmlnDriverAttachBase::sm_bPreservePosition("Preserve Position", false);

//--------------------------------------------------------------------
// Constructor initializes properties and registers callbacks.
// World space offset is initialized based on given argument.
//--------------------------------------------------------------------
tmlnDriverAttachBase::tmlnDriverAttachBase(const maPoint3d& i_InitialPosition)
:	m_bNeedsAttach(false),
	m_bPreservePosition(false),
	m_ObjectName("Object Name"),
	m_AttachName("Attach Name"),
	m_AttachOffset("Attach Offset",maPoint3d(0.0f,0.0f,0.0f)),
	m_WorldSpaceOffset("World Space Offset", i_InitialPosition)
{
	// Register the properties so they can be displayed to the user
	//
	const int lc_ATTACHCONTROL_WIDTH = 300;
	m_pObjectNameUIInfo = new prtyComboBoxUIInfo(&(m_ObjectName), "Attach", "Name of the object to which the driver should attach");
	m_pObjectNameUIInfo->SetWidth( lc_ATTACHCONTROL_WIDTH );
	AddProperty( m_pObjectNameUIInfo );
	m_pAttachNameUIInfo = new prtyComboBoxUIInfo(&(m_AttachName), "Attach", "Name of transform node or joint for matrix");
	m_pAttachNameUIInfo->SetWidth( lc_ATTACHCONTROL_WIDTH );
	AddProperty( m_pAttachNameUIInfo );	
	prtyVector3dEditUpDownUIInfo* pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_AttachOffset), "Attach", "Attach offset of position in local space");
	pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
	pPVEUDUII->SetDecimalPlaces(1);
	AddProperty( pPVEUDUII );	
	pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_WorldSpaceOffset), "Attach", "World Space Offset");
	pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
	pPVEUDUII->SetDecimalPlaces(1);
	AddProperty( pPVEUDUII );	

	// The preserve position property is static, shared by all attachment drivers
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyCheckBoxUIInfo(&(sm_bPreservePosition), "Preserve", "Preserve Position");
	AddProperty( pPUII );

	//	property callbacks
	m_ObjectName.AddCallback(new prtyCallbackWrapper<tmlnDriverAttachBase>(this, &tmlnDriverAttachBase::ObjectNameChanged));
	m_AttachName.AddCallback(new prtyCallbackWrapper<tmlnDriverAttachBase>(this, &tmlnDriverAttachBase::AttachNameChanged));
	m_AttachOffset.AddCallback(new prtyCallbackWrapper<tmlnDriverAttachBase>(this, &tmlnDriverAttachBase::AttachOffsetChanged));
	m_WorldSpaceOffset.AddCallback(new prtyCallbackWrapper<tmlnDriverAttachBase>(this, &tmlnDriverAttachBase::PropertyChanged));
	sm_bPreservePosition.AddCallback(new prtyCallbackWrapper<tmlnDriverAttachBase>(this, &tmlnDriverAttachBase::PropertyChanged));
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverAttachBase::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	std::string str = "Attach : ";
	str += m_ObjectName.GetValue().GetString();
	str += " ";
	str += m_AttachName.GetValue();

	desc += str;
	return desc;
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void  tmlnDriverAttachBase::DoEditProperties()
{
	// Before showing the properties dialog,
	// update the name list to the current list of named objects.
	m_pObjectNameUIInfo->ClearItems();
	nameList aNameList;
	nameMgr::GetNameList( aNameList );
	for ( int i = 0 ; i < aNameList.size() ; i++ )
	{
		//DBG_LOG2( "%02d) %s", i, aNameList[i]->GetString().c_str() );
		m_pObjectNameUIInfo->AddItem( aNameList[i]->GetString() );
	}
	m_pObjectNameUIInfo->UpdateControl();

	// Let base class set up the properties-based dialog
	tmlnDriver::DoEditProperties();
}

//--------------------------------------------------------------------
//	Override base class to have this driver's Operate called at end.
//--------------------------------------------------------------------
//virtual 
bool tmlnDriverAttachBase::NeedsDelayedOperate()
{
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAttachBase::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAttachBase::ObjectNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	//	if the UID is invalid, then see if the object that has a name match
	//	has a valid UID.  This should only be the case with object's that 
	//	haven't been converted to the current format.
	//
	if ( this->m_ObjectName.GetValue().GetUID() == nameString::e_InvalidUID )
	{
		// Find object by name
		nameObject* obj = nameMgr::GetObjectByName( this->m_ObjectName.GetValue() );	// name of prop to target
		if (obj && dynamic_cast<mnmObject*>(obj))
		{
			if ( obj->GetName().GetUID() != nameString::e_InvalidUID )
			{
				this->m_ObjectName.SetUID( obj->GetName().GetUID() );
			}
		}
	}

	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	if (i_bDirty)
	{
		// If this is a set from the GUI related to attaching then 
		// try to preserve the position of the object by 
		// setting the world offset accordingly.
		m_bPreservePosition = sm_bPreservePosition.GetValue();
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAttachBase::AttachNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Delay actual attachment, helps to make sure all named objects
	// are loaded and created.
	m_bNeedsAttach = true;

	if (i_bDirty)
	{
		// If this is a set from the GUI related to attaching then 
		// try to preserve the position of the object by 
		// setting the world offset accordingly.
		m_bPreservePosition = sm_bPreservePosition.GetValue();
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverAttachBase::AttachOffsetChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_Reference.SetAttachOffset( this->m_AttachOffset.GetValue() );
	this->MarkDirty();
}


//--------------------------------------------------------------------
// Check to see if we need to attach to a new object.
// Should be called at the beginning of Operate().
//--------------------------------------------------------------------
void  tmlnDriverAttachBase::ConfirmAttachment()
{
	if (m_bNeedsAttach)
	{
		// Find object by name
		nameObject* obj = tmlnDriverAttachUtil::GetObjectByName( this->m_ObjectName.GetValue() );	// name of prop to target
		if (obj && dynamic_cast<mnmObject*>(obj))
		{
			//	attach object
			//
			this->m_ObjectName.SetValue(obj->GetName()); // sets the UID correctly if we found it by string
			
			mnmObject *pObj = dynamic_cast<mnmObject*>(obj);

			// Get list of references
			std::vector<std::string> references;
			pObj->GetReferenceList(references);

			// Set the reference list UI Info now that we have the object,
			// but only if necessary
			if (m_pAttachNameUIInfo->m_List != references)
			{
				m_pAttachNameUIInfo->ClearItems();
				pObj->GetReferenceList(m_pAttachNameUIInfo->m_List);
				m_pAttachNameUIInfo->UpdateControl();
			}

			// If our attach name is empty, pick the first name in the reference list
			if ( this->m_AttachName.GetValue().empty() 
				&& !references.empty() )
			{	
				this->m_AttachName.SetValue( references[0] );
			}

			api3dReference *pRef = pObj->GetReference(this->m_AttachName.GetValue().c_str()); // reference node name
			//DBG_LOG1("Got mnmObject, and reference?: %s", (pRef ? "True" : "NULL"));
			m_Reference.AttachTo(pRef);
		}
		else m_Reference.AttachTo(NULL);
		m_Reference.SetAttachOffset( this->m_AttachOffset.GetValue() );

		m_bNeedsAttach = false;
	}
}