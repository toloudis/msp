/****************************************************************************\
**	pwxTemplate_FloatEdit.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_TEMPLATE_FLOATEDIT_HPP
#error pwxTemplate_FloatEdit.hpp multiply included
#endif
#define PWX_TEMPLATE_FLOATEDIT_HPP

#ifndef PWX_CONTROLUTIL_HPP
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"
#endif
#ifndef TWC_FLOATEDIT_HPP
#include "ToolUIWx/twc/twcFloatEdit.hpp"
#endif
#ifndef PRTY_UNITS_HPP
#include "Core/prty/prtyUnits.hpp"
#endif 
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pwxTemplate_FloatEdit : public pwxControl, public prtyUnitsInterest
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pwxTemplate_FloatEdit(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							wxWindow* i_pParent)
		:	pwxControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = Converter::GetCommonValue(i_pUIInfo, newvalue);
			//bool bSameValue = pwxControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pwxControlUtil::CleanUpWidget<twcFloatEdit>(i_pParent);
			//m_pActualControl = new twcFloatEdit(i_pParent, wxID_ANY);

			//	set the control values
			this->UpdateControl(i_pUIInfo.get());

			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}

			// watch for units changes
			prtyUnits::RegisterUnitsInterest(this);

			//	hook up events
			//
			this->Connect( m_pActualControl->GetId(), wxEVT_VALUE_CHANGED,
							wxCommandEventHandler(pwxTemplate_FloatEdit::FloatEdit_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pwxTemplate_FloatEdit()
		{
			prtyUnits::UnRegisterUnitsInterest(this);
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

			const ValueType newvalue = Converter::GetPropertyValue(i_pProperty);
			//const ValueType newvalue = (static_cast<PropertyType*>(i_pProperty))->GetValue();

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

		//--------------------------------------------------------------------
		//	UnitsChanged - notification that the units for the
		//	user interface has changed.
		//--------------------------------------------------------------------
		virtual void UnitsChanged()
		{
			// Trigger notification artificially
			this->TriggerPropertyChanged();
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void FloatEdit_ValueChanged(wxCommandEvent& i_Event)
		{
			if (m_bLocalChangeNoUpdate) return;

			m_bLocalChangeNoUpdate = true;
			//if (m_OriginalValue != m_pActualControl->GetValue())
			{
				ValueType value = Converter::GetValueFromControl(m_pActualControl);
				Converter::SetCommonValue(this, value);
				//pwxControlUtil::SetCommonValue<ValueType, PropertyType>(this, value);
			}
			m_bLocalChangeNoUpdate = false;
		}

		twcFloatEdit* m_pActualControl;
};

#endif // USE_WXWIDGETS
