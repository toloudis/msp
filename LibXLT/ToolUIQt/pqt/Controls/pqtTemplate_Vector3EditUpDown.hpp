/****************************************************************************\
**	pqtTemplate_Vector3EditUpDown.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (Vector3EditUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEMPLATE_VECTOR3EDITUPDOWN_HPP
#error pqtTemplate_Vector3EditUpDown.hpp multiply included
#endif
#define PQT_TEMPLATE_VECTOR3EDITUPDOWN_HPP

#ifndef PQT_CONTROLUTIL_HPP
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"
#endif
#ifndef TQC_VECTOR3EDITUPDOWN_HPP
#include "ToolUIQt/tqc/tqcVector3EditUpDown.hpp"
#endif
#ifndef PRTY_VECTOR3DEDITUPDOWNUIINFO_HPP
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
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
class pqtTemplate_Vector3EditUpDown : public pqtControl, public prtyUnitsInterest
{
#ifdef QT_FINISH_PORT
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTemplate_Vector3EditUpDown(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
									  QWidget* i_pParent)
		:	pqtControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = Converter::GetCommonValue(i_pUIInfo, newvalue);
			//bool bSameValue = pqtControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcVector3EditUpDown>(i_pParent);
			//m_pActualControl = new tqcVector3EditUpDown(i_pParent);

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
							wxCommandEventHandler(pqtTemplate_Vector3EditUpDown::Vector3EditUpDown_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pqtTemplate_Vector3EditUpDown()
		{
			prtyUnits::UnRegisterUnitsInterest(this);
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

			const ValueType newvalue = Converter::GetPropertyValue(i_pProperty);
			//const ValueType newvalue = (static_cast<PropertyType*>(i_pProperty))->GetValue();

			m_bLocalChangeNoUpdate = true;
			if ( Converter::GetValueFromControl(m_pActualControl) != newvalue )
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}
			m_bLocalChangeNoUpdate = false;
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

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		void UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
		{
			m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());

			prtyVector3dEditUpDownUIInfo* pUII = static_cast<prtyVector3dEditUpDownUIInfo*>(i_pUIInfo);
			m_pActualControl->SetIncrement( pUII->GetIncrement() );
			m_pActualControl->SetDecimalPlaces( pUII->GetDecimalPlaces() );
			m_pActualControl->SetMaximum( pUII->GetMaximum() );
			m_pActualControl->SetMinimum( pUII->GetMinimum() );
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void Vector3EditUpDown_ValueChanged(wxCommandEvent& i_Event)
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

		tqcVector3EditUpDown* m_pActualControl;
#endif // USE_QT
};

