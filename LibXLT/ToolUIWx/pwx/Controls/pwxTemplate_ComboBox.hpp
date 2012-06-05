/****************************************************************************\
**	pwxTemplate_ComboBox.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_TEMPLATE_COMBOBOX_HPP
#error pwxTemplate_ComboBox.hpp multiply included
#endif
#define PWX_TEMPLATE_COMBOBOX_HPP

#ifndef PWX_CONTROLUTIL_HPP
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pwxTemplate_ComboBox : public pwxControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pwxTemplate_ComboBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							wxWindow* i_pParent)
		:	pwxControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = pwxControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pwxControlUtil::CleanUpWidget<wxChoice>(i_pParent, wxID_ANY);
			//m_pActualControl = new wxChoice(i_pParent, wxID_ANY);

			//	set the control values
			this->UpdateControl(i_pUIInfo.get());

			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
				m_OriginalValue = newvalue;
			}

			//	hook up events
			//
			this->Connect( m_pActualControl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
							wxCommandEventHandler(pwxTemplate_ComboBox::ComboBox_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pwxTemplate_ComboBox()
		{
			m_pActualControl->RemoveEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual wxWindow* GetControl()
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
		void PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
		{
			if (m_bLocalChangeNoUpdate) return;

			const ValueType newvalue = (static_cast<PropertyType*>(i_pProperty))->GetValue();

			m_bLocalChangeNoUpdate = true;
			if ( Converter::GetValueFromControl(m_pActualControl) != newvalue )
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
				m_OriginalValue = newvalue;
			}
			m_bLocalChangeNoUpdate = false;
		}

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		void UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
		{
			m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());
			
			// Get list of string choices for combo box
			std::vector<std::string> choices;
			Converter::GetChoices(i_pUIInfo, choices);

			// Preserve old string value
			wxString old_value = m_pActualControl->GetStringSelection();

			// Add choices to control
			m_pActualControl->Clear();
			int num_choices = choices.size();
			for (int i = 0; i < num_choices; ++i)
			{
				m_pActualControl->Append(wxString(choices[i].c_str(), wxConvUTF8));
			}

			// Put old value back in without triggering a callback.
			// This might fail to set the string if the choices have changed,
			// but that is okay. 
			m_pActualControl->SetStringSelection(old_value);
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void ComboBox_ValueChanged(wxCommandEvent& i_Event)
		{
			if (m_bLocalChangeNoUpdate) return;

			m_bLocalChangeNoUpdate = true;
			//if (m_OriginalValue != m_pActualControl->GetValue())
			{
				ValueType value = Converter::GetValueFromControl(m_pActualControl);
				if (!pwxControlUtil::SetCommonValue<ValueType, PropertyType>(this, value))
				{
					// user did not confirm change, diplay the old value
					Converter::SetValueIntoControl(m_pActualControl, m_OriginalValue);
				}
				else
					m_OriginalValue = value;
			}
			m_bLocalChangeNoUpdate = false;
		}

		wxChoice* m_pActualControl;
		ValueType m_OriginalValue;
};

#endif // USE_WXWIDGETS
