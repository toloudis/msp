/****************************************************************************\
**	pqtTemplate_ListBox.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEMPLATE_LISTBOX_HPP
#error pqtTemplate_ListBox.hpp multiply included
#endif
#define PQT_TEMPLATE_LISTBOX_HPP

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
class pqtTemplate_ListBox : public pqtControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTemplate_ListBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent)
		:	pqtControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = pqtControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pqtControlUtil::CleanUpWidget<QListWidget>(i_pParent);
			//m_pActualControl = new wxListBox(i_pParent, wxID_ANY);

			//	set the control values
			this->UpdateControl(i_pUIInfo.get());

			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}

			//	hook up events
			//
			QObject::connect(m_pActualControl, &QListWidget::currentRowChanged, [this](int) { ListBox_ValueChanged(); });
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pqtTemplate_ListBox()
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

			// Add choices to control
			QSignalBlocker blocker(m_pActualControl);
			m_pActualControl->clear();
			int num_choices = choices.size();
			for (int i = 0; i < num_choices; ++i)
			{
				m_pActualControl->addItem(tqtQString(choices[i]));
			}
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void ListBox_ValueChanged()
		{
			if (m_bLocalChangeNoUpdate) return;

			m_bLocalChangeNoUpdate = true;
			//if (m_OriginalValue != m_pActualControl->GetValue())
			{
				ValueType value = Converter::GetValueFromControl(m_pActualControl);
				pqtControlUtil::SetCommonValue<ValueType, PropertyType>(this, value);
			}
			m_bLocalChangeNoUpdate = false;
		}

		QListWidget* m_pActualControl;
};

#endif // USE_QT
