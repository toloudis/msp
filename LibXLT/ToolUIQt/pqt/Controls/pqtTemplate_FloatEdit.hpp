/****************************************************************************\
**	pqtTemplate_FloatEdit.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEMPLATE_FLOATEDIT_HPP
#error pqtTemplate_FloatEdit.hpp multiply included
#endif
#define PQT_TEMPLATE_FLOATEDIT_HPP

#ifndef PQT_CONTROLUTIL_HPP
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"
#endif
#ifndef TQC_FLOATEDIT_HPP
#include "ToolUIQt/tqc/tqcFloatEdit.hpp"
#endif
#ifndef PRTY_UNITS_HPP
#include "Core/prty/prtyUnits.hpp"
#endif 
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif


//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pqtTemplate_FloatEdit : public pqtControl, public prtyUnitsInterest
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTemplate_FloatEdit(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent)
		:	pqtControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = Converter::GetCommonValue(i_pUIInfo, newvalue);
			//bool bSameValue = pqtControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcFloatEdit>(i_pParent);
			//m_pActualControl = new tqcFloatEdit(i_pParent, wxID_ANY);

			//	set the control values
			this->UpdateControl(i_pUIInfo.get());

			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}

			// watch for units changes
			prtyUnits::RegisterUnitsInterest(this);

#ifdef QT_FINISH_PORT
			//	hook up events
			//
			this->Connect( m_pActualControl->GetId(), wxEVT_VALUE_CHANGED,
							wxCommandEventHandler(pqtTemplate_FloatEdit::FloatEdit_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
#endif
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pqtTemplate_FloatEdit()
		{
			prtyUnits::UnRegisterUnitsInterest(this);
#ifdef QT_FINISH_PORT
			m_pActualControl->RemoveEventHandler(this);
#endif
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual QWidget* GetControl()
		{
			return m_pActualControl;
		}

#ifdef QT_FINISH_PORT
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
#endif // USE_QT

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
#ifdef QT_FINISH_PORT
		void FloatEdit_ValueChanged(wxCommandEvent& i_Event)
		{
			if (m_bLocalChangeNoUpdate) return;

			m_bLocalChangeNoUpdate = true;
			//if (m_OriginalValue != m_pActualControl->GetValue())
			{
				ValueType value = Converter::GetValueFromControl(m_pActualControl);
				Converter::SetCommonValue(this, value);
				//pqtControlUtil::SetCommonValue<ValueType, PropertyType>(this, value);
			}
			m_bLocalChangeNoUpdate = false;
		}
#endif

		tqcFloatEdit* m_pActualControl;
};

