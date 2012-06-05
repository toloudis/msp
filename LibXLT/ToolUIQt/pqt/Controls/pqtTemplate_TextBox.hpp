/****************************************************************************\
**	pqtTemplate_TextBox.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEMPLATE_TEXTBOX_HPP
#error pqtTemplate_TextBox.hpp multiply included
#endif
#define PQT_TEMPLATE_TEXTBOX_HPP

#ifndef PQT_CONTROLUTIL_HPP
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"
#endif
#ifndef TQC_TEXTBOX_HPP
#include "ToolUIQt/tqc/tqcTextBox.hpp"
#endif
#ifndef PRTY_TEXTBOXUIINFO_HPP
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#endif 
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif


//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pqtTemplate_TextBox : public pqtControl
{
#ifdef QT_FINISH_PORT
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTemplate_TextBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent)
		:	pqtControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = pqtControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// See if it needs to be multiline
			prtyTextBoxUIInfo* pTBUII = static_cast<prtyTextBoxUIInfo*>(i_pUIInfo.get());

			// create the actual control
			if (pTBUII->GetMultiline())
			{
				if (pTBUII->GetMultilineFixed())
				{
					m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcMultilineFixedTextBox>(i_pParent, wxID_ANY);
				}
				else
				{
					m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcMultilineTextBox>(i_pParent, wxID_ANY);
				}
			}
			else
			{
				m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcTextBox>(i_pParent, wxID_ANY);
			}
			
			//	set the control values
			this->UpdateControl(i_pUIInfo.get());

			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}

			//	hook up events
			//
			this->Connect( m_pActualControl->GetId(), wxEVT_VALUE_CHANGED,
							wxCommandEventHandler(pqtTemplate_TextBox::TextBox_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pqtTemplate_TextBox()
		{
			m_pActualControl->RemoveEventHandler(this);
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
			m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void TextBox_ValueChanged(wxCommandEvent& i_Event)
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

		tqcTextBox* m_pActualControl;
#endif // USE_QT
};

