/****************************************************************************\
**	pwxFilePath_ComboBox.cpp
**
**		see .hpp
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxFilePath_ComboBox.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyFilePathComboBoxUIInfo.hpp"
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"

#include <algorithm>

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pwxFilePath_ComboBox::pwxFilePath_ComboBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										   wxWindow* i_pParent)
:	pwxControl(i_pUIInfo),
	m_pActualControl(NULL),
	m_bHasValue(false)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	fsLocator newvalue;
	bool bSameValue = pwxControlUtil::GetCommonValue<fsLocator, prtyFilePath>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new pwxControlUtil::CleanUpWidget<wxChoice>(i_pParent, wxID_ANY);

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		SetValueIntoControl(newvalue);
		m_OriginalValue = newvalue;
	}

	//	hook up events
	//
	this->Connect( m_pActualControl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
					wxCommandEventHandler(pwxFilePath_ComboBox::ComboBox_ValueChanged) );
	m_pActualControl->PushEventHandler(this);
}

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pwxFilePath_ComboBox::~pwxFilePath_ComboBox()
{
	m_pActualControl->RemoveEventHandler(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
wxWindow* pwxFilePath_ComboBox::GetControl()
{
	return m_pActualControl;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pwxFilePath_ComboBox::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());

	// Preserve old value
	const bool bHadValue = m_bHasValue;
	const fsLocator old_value = GetValueFromControl();

	// Add choices to control
	m_pActualControl->Clear();
	m_Values.clear();
	m_bHasValue = false;

	prtyFilePathComboBoxUIInfo* pUII = dynamic_cast<prtyFilePathComboBoxUIInfo*>(i_pUIInfo);
	DBG_ASSERT(pUII != NULL, "pwxFilePath_ComboBox needs a prtyFilePathComboBoxUIInfo");
	if (pUII)
	{
		const std::vector<fsLocator>& values = pUII->GetValues();
		const size_t num_choices = (std::min)(values.size(), pUII->m_List.size());
		for (size_t i = 0; i < num_choices; ++i)
		{
			m_pActualControl->Append(wxString(pUII->m_List[i].c_str(), wxConvUTF8));
			m_Values.push_back(values[i]);
		}
	}

	// Put old value back in without triggering a callback.
	if (bHadValue)
		SetValueIntoControl(old_value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxFilePath_ComboBox::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

	const fsLocator newvalue = (static_cast<prtyFilePath*>(i_pProperty))->GetValue();

	m_bLocalChangeNoUpdate = true;
	if ( !m_bHasValue || !prtyFilePathComboBoxUIInfo::IsSamePath(GetValueFromControl(), newvalue) )
	{
		SetValueIntoControl(newvalue);
		m_OriginalValue = newvalue;
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxFilePath_ComboBox::ComboBox_ValueChanged(wxCommandEvent& i_Event)
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	const int selection = m_pActualControl->GetSelection();
	// picking the entry that is already chosen changes nothing
	if (selection != wxNOT_FOUND &&
		!prtyFilePathComboBoxUIInfo::IsSamePath(GetValueFromControl(), m_OriginalValue))
	{
		const fsLocator value = GetValueFromControl();
		if (!pwxControlUtil::SetCommonValue<fsLocator, prtyFilePath>(this, value))
		{
			// user did not confirm change, diplay the old value
			SetValueIntoControl(m_OriginalValue);
		}
		else
			m_OriginalValue = value;
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
// Select the entry for the given path, adding one if there is none
//----------------------------------------------------------------------------
void pwxFilePath_ComboBox::SetValueIntoControl(const fsLocator& i_Value)
{
	if (i_Value.GetNumNames() == 0)
	{
		m_pActualControl->SetSelection(wxNOT_FOUND);
		m_bHasValue = false;
		return;
	}

	int index = -1;
	for (size_t i = 0; i < m_Values.size(); i++)
	{
		if (prtyFilePathComboBoxUIInfo::IsSamePath(m_Values[i], i_Value))
		{
			index = (int)i;
			break;
		}
	}

	if (index < 0)
	{
		// not one of the choices (an old or custom file): show its path
		itString label;
		fsFileUtil::LocatorToUnicodeString(i_Value, label);
		index = m_pActualControl->Append(wxString(label.GetString()));
		m_Values.push_back(i_Value);
	}

	m_pActualControl->SetSelection(index);
	m_bHasValue = true;
}

//----------------------------------------------------------------------------
// Path of the selected entry (empty if nothing is selected)
//----------------------------------------------------------------------------
fsLocator pwxFilePath_ComboBox::GetValueFromControl() const
{
	const int selection = m_pActualControl->GetSelection();
	if (selection == wxNOT_FOUND || selection >= (int)m_Values.size())
		return fsLocator();
	return m_Values[selection];
}

#endif // USE_WXWIDGETS
