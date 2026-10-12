/****************************************************************************\
**	pqtTemplate_ComboBox.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEMPLATE_COMBOBOX_HPP
#error pqtTemplate_ComboBox.hpp multiply included
#endif
#define PQT_TEMPLATE_COMBOBOX_HPP

#ifndef PQT_CONTROLUTIL_HPP
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pqtTemplate_ComboBox : public pqtControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTemplate_ComboBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent)
		:	pqtControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = pqtControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pqtControlUtil::CleanUpWidget<QComboBox>(i_pParent);
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
			QObject::connect(m_pActualControl, QOverload<int>::of(&QComboBox::activated), [this](int) { ComboBox_ValueChanged(); });
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pqtTemplate_ComboBox()
		{
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual QWidget* GetControl()
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
			m_pActualControl->setEnabled(!i_pUIInfo->GetReadOnly());
			
			// Get list of string choices for combo box
			std::vector<std::string> choices;
			Converter::GetChoices(i_pUIInfo, choices);

			// Preserve old string value
			QString old_value = m_pActualControl->currentText();

			// Add choices to control
			QSignalBlocker blocker(m_pActualControl);
			m_pActualControl->clear();
			int num_choices = choices.size();
			for (int i = 0; i < num_choices; ++i)
			{
				m_pActualControl->addItem(tqtQString(choices[i]));
			}

			// Put old value back in without triggering a callback.
			// This might fail to set the string if the choices have changed,
			// but that is okay. 
			pqtControlUtil::SelectString(m_pActualControl, old_value);
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void ComboBox_ValueChanged()
		{
			if (m_bLocalChangeNoUpdate) return;

			m_bLocalChangeNoUpdate = true;
			//if (m_OriginalValue != m_pActualControl->GetValue())
			{
				ValueType value = Converter::GetValueFromControl(m_pActualControl);
				if (!pqtControlUtil::SetCommonValue<ValueType, PropertyType>(this, value))
				{
					// user did not confirm change, diplay the old value
					Converter::SetValueIntoControl(m_pActualControl, m_OriginalValue);
				}
				else
					m_OriginalValue = value;
			}
			m_bLocalChangeNoUpdate = false;
		}

		QComboBox* m_pActualControl;
		ValueType m_OriginalValue;
};

#endif // USE_QT
