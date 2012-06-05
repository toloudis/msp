/****************************************************************************\
**	pqtControl.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/pqtControl.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/prty/prtyProperty.hpp"
#include "Core/prty/prtyPropertyUIInfo.hpp"
#include "Core/prty/prtyPropertyReference.hpp"
#include "Tool/gui/guiMessageBox.hpp"


#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtControl::pqtControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo)
:	m_bLocalChangeNoUpdate(false),
	m_pUIInfo(i_pUIInfo)
{	
	// We can't really respond to property changes when there are more than one property
	// in the UIInfo. We get into situations where the values could vary or the
	// properties become invalid while the UIInfo is still valid.
	// So, only register a property changed callback if we have a single property.
	bool bRegisterCallbacks = (i_pUIInfo->GetNumberOfProperties() == 1);

	if (bRegisterCallbacks)
	{
		// using shared_ptr in order to keep a copy of the callback
		// for removing in the destructor
		m_pCallback.reset(new prtyCallbackWrapper<pqtControl>(this, &pqtControl::PropertyChanged));
	}

	// Gather up the properties and register callbacks
	for (int i=0; i < i_pUIInfo->GetNumberOfProperties(); ++i)
	{
		prtyProperty* pProperty = i_pUIInfo->GetProperty(i);
		m_Properties.push_back(pProperty);

		m_ReferenceCreators.push_back(i_pUIInfo->GetReferenceCreator(i));

		// Set this name to the name of the first property
		if (i == 0)
		{
			m_Name = pProperty->GetPropertyName();
		}
		
		if (bRegisterCallbacks)
		{
			//	register the callback, using shared_ptr 
			pProperty->AddCallback(m_pCallback);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtControl::~pqtControl()
{
	// See if we registered any callbacks. If so, disconnect them now.
	if (m_pCallback.get())
	{
		// This lock function checks to see if the weak_ptr's contents are still valid,
		// meaning that we only have to disconnect the callback 
		if (shared_ptr<prtyPropertyUIInfo> pUIInfo = m_pUIInfo.lock())
		{
			// Disconnect the callbacks
			DBG_ASSERT( pUIInfo->GetNumberOfProperties() ==  m_Properties.size(), "UIInfo changed while control was monitoring" );
			for (int i=0; i < m_Properties.size(); ++i)
			{
				prtyProperty* pProperty = m_Properties[i];
				pProperty->RemoveCallback(m_pCallback);
			}
		}
		else
		{
			//DBG_LOG("UIInfo lock failed, weak pointer is invalid.");
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const std::string& pqtControl::GetName() const
{
	return m_Name;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//void pqtControl::SetName(const std::string& i_Name)
//{
//	m_Name = i_Name;
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int pqtControl::GetNumberOfProperties() const
{
	return m_Properties.size();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtyProperty* pqtControl::GetProperty(int i_Index)
{
	DBG_ASSERT( (i_Index >= 0) && (i_Index < m_Properties.size()), "Invalid property index" );
	return m_Properties[i_Index];
}

//----------------------------------------------------------------------------
// Create reference to property with given index in order to make an undo
// operation. Result could be NULL if no reference could be created.
//----------------------------------------------------------------------------
shared_ptr<prtyPropertyReference> pqtControl::CreatePropertyReference(int i_Index)
{
	DBG_ASSERT( (i_Index >= 0) && (i_Index < m_ReferenceCreators.size()), "Invalid property index" );
	shared_ptr<prtyPropertyReference> property_reference;
	prtyReferenceCreator *pRefCreator = m_ReferenceCreators[i_Index];
	if (pRefCreator)
	{
		property_reference = pRefCreator->CreateReferenceForProperty(*m_Properties[i_Index]);
	}
	return property_reference;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//void pqtControl::SetProperty(prtyProperty* i_pProperty, int i_Index)
//{
//	DBG_ASSERT(i_pProperty != 0, "Cannot add a NULL property");
//	DBG_ASSERT( (i_Index >= 0) && (i_Index < m_Properties->size()), "Invalid property index" );
//
//	m_Properties[i_Index] = i_pProperty;
//}
//void pqtControl::AddProperty(prtyProperty* i_pProperty)
//{
//	DBG_ASSERT(i_pProperty != 0, "Cannot add a NULL property");
//
//	m_Properties.push_back(i_pProperty);
//}

//----------------------------------------------------------------------------
// Return true if this control is attached to this UI Info
//----------------------------------------------------------------------------
bool pqtControl::HasUIInfo(prtyPropertyUIInfo* i_pUIInfo) const
{
	// Can't compare the weak and shared pointers directly?
	//return (m_pUIInfo == i_pUIInfo);

	// Is this expensive to do? Is there a better way to do the comparison?
	if (shared_ptr<prtyPropertyUIInfo> pUIInfo = m_pUIInfo.lock())
	{
		return (pUIInfo.get() == i_pUIInfo);
	}
	// If weak_ptr is not valid, then can't be the same
	return false;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pqtControl::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
}

////----------------------------------------------------------------------------
////----------------------------------------------------------------------------
//void pqtControl::SetToolTip()
//{
//	const bool lc_UseToolTips = true;
//	if (lc_UseToolTips && (m_pControl != nullptr) && (m_pUIInfo != nullptr))
//	{
//		System::Windows::Forms::ToolTip^ ToolTip1 = gcnew System::Windows::Forms::ToolTip();
//		ToolTip1->IsBalloon = true;
//		ToolTip1->SetToolTip(m_pControl, gcnew System::String(m_pUIInfo->GetDescription().c_str()));
//	}
//}
//
////----------------------------------------------------------------------------
////	Set the control
////----------------------------------------------------------------------------
//void pqtControl::SetControl( System::Windows::Forms::Control^ i_pControl )
//{
//	m_pControl = i_pControl;
//
//	//	Set-up the tooltip
//	SetToolTip();
//}

//----------------------------------------------------------------------------
// Force firing of property changed callback
//----------------------------------------------------------------------------
void pqtControl::TriggerPropertyChanged()
{
	// Trigger notification artificially
	if (!m_Properties.empty())
		this->m_Properties[0]->NotifyCallbacksPropertyChanged(false);
}

//----------------------------------------------------------------------------
// Display the confirmation string associated with the UIInfo,
// if one exists. Returns true if the user confirms the change
// or if no confirmation was needed.
//----------------------------------------------------------------------------
bool pqtControl::ConfirmChange()
{
	if (shared_ptr<prtyPropertyUIInfo> pUIInfo = m_pUIInfo.lock())
	{
		std::string confirmation = pUIInfo->GetConfirmationString();
		if (!confirmation.empty())
		{
			int retval = guiMessageBox::Show( confirmation.c_str(), "Confirm Change", guiMessageBox::e_YesNo );
			if ( retval == guiMessageBox::e_Yes )
				return true;
			else
				return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtControl::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
}

#endif
