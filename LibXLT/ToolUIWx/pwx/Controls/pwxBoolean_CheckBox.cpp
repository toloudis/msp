/****************************************************************************\
**	pwxBoolean_CheckBox.hpp
**
**		Intermediate class between the property (prtyBoolean) and 
**	the control (CheckBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxBoolean_CheckBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"

#include "Core/prty/prtyBoolean.hpp"

#ifdef USE_WXWIDGETS

namespace
{
	// Create derivation of widget that notifies the control manager
	// when it is deleted by wxWidgets
	class CleanUpCheckBox : public wxCheckBox
	{
	public:
		CleanUpCheckBox(wxWindow* i_pParent)
			: wxCheckBox(i_pParent, wxID_ANY, wxT("")) {}
		~CleanUpCheckBox()
		{
			pwxControlMgr::DeleteControl(this);
		}
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pwxBoolean_CheckBox::pwxBoolean_CheckBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 wxWindow* i_pParent)
:	pwxControl(i_pUIInfo)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	bool newvalue = false;
	bool bSameValue = pwxControlUtil::GetCommonValue<bool, prtyBoolean>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new CleanUpCheckBox(i_pParent);
	//m_pActualControl = new wxCheckBox(i_pParent, wxID_ANY, wxT(""));

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		m_pActualControl->SetValue(newvalue);
	}

	//	hook up events
	//
	this->Connect( m_pActualControl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
					wxCommandEventHandler(pwxBoolean_CheckBox::CheckBox_ValueChanged) );
	m_pActualControl->PushEventHandler(this);
};

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pwxBoolean_CheckBox::~pwxBoolean_CheckBox()
{
	m_pActualControl->RemoveEventHandler(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
wxWindow* pwxBoolean_CheckBox::GetControl()
{
	return m_pActualControl;
}

//----------------------------------------------------------------------------
//	Note: right now, PropertyChanged will not create a circular update because
//	of the m_bLocalChangeNoUpdate flag.  This IS NOT true the other way
//	around.  If a control calls its "ValueChanged" then the property will
//	call all of its callback controls and one of them could have been the one
//	that originally updated the property value(s).  By checking the diff of 
//	the values we can avoid a repetitive setting of the control's values.
//----------------------------------------------------------------------------
void pwxBoolean_CheckBox::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

	const bool newvalue = (static_cast<prtyBoolean*>(i_pProperty))->GetValue();

	m_bLocalChangeNoUpdate = true;
	if ( m_pActualControl->GetValue() != newvalue )
	{
		m_pActualControl->SetValue(newvalue);
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pwxBoolean_CheckBox::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxBoolean_CheckBox::CheckBox_ValueChanged(wxCommandEvent& i_Event)
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	//if (m_OriginalValue != m_pActualControl->GetValue())
	{
		pwxControlUtil::SetCommonValue<bool, prtyBoolean>(this, m_pActualControl->GetValue());
	}
	m_bLocalChangeNoUpdate = false;
}
#endif
