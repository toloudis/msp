/****************************************************************************\
**	pwxHotKey_KeyCombo.hpp
**
**		Intermediate class between the property (prtyHotKey) and 
**	the control (HotKeyCtrl).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxHotKey_KeyCombo.hpp"
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"

#include "Core/prty/prtyHotKey.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pwxHotKey_KeyCombo::pwxHotKey_KeyCombo(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 wxWindow* i_pParent)
:	pwxControl(i_pUIInfo)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	std::string newvalue;
	bool bSameValue = pwxControlUtil::GetCommonValue<std::string, prtyHotKey>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new pwxControlUtil::CleanUpWidget<twcHotKeyCtrl>(i_pParent);
	//m_pActualControl = new twcHotKeyCtrl(i_pParent, wxID_ANY, wxT(""));

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		m_pActualControl->SetValue(wxString(newvalue.c_str(), wxConvUTF8));
	}

	//	hook up events
	//
	this->Connect( m_pActualControl->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(pwxHotKey_KeyCombo::HotKeyCtrl_ValueChanged) );
	m_pActualControl->PushEventHandler(this);
};

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pwxHotKey_KeyCombo::~pwxHotKey_KeyCombo()
{
	m_pActualControl->RemoveEventHandler(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
wxWindow* pwxHotKey_KeyCombo::GetControl()
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
void pwxHotKey_KeyCombo::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

	 wxString newvalue( (static_cast<prtyHotKey*>(i_pProperty))->GetValue().c_str(), wxConvUTF8);

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
void pwxHotKey_KeyCombo::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxHotKey_KeyCombo::HotKeyCtrl_ValueChanged(wxCommandEvent& i_Event)
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	//if (m_OriginalValue != m_pActualControl->GetValue())
	{
		std::string hot_key_str = m_pActualControl->GetValue().utf8_str();
		pwxControlUtil::SetCommonValue<std::string, prtyHotKey>(this, hot_key_str);
	}
	m_bLocalChangeNoUpdate = false;
}
#endif
