/****************************************************************************\
**	pqtTemplate_DirPicker.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (DirPicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEMPLATE_DIRPICKER_HPP
#error pqtTemplate_DirPicker.hpp multiply included
#endif
#define PQT_TEMPLATE_DIRPICKER_HPP

#ifndef PQT_CONTROLUTIL_HPP
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif


#ifdef QT_FINISH_PORT
#include <wx/filepicker.h>
#endif


//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pqtTemplate_DirPicker : public pqtControl
{
#ifdef QT_FINISH_PORT
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTemplate_DirPicker(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent)
		:	pqtControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = pqtControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pqtControlUtil::CleanUpWidget<wxDirPickerCtrl>(i_pParent, wxID_ANY);
			//m_pActualControl = new wxDirPickerCtrl(i_pParent, wxID_ANY);
			//wxFLP_OPEN | wxFLP_FILE_MUST_EXIST and, under wxMSW only, wxFLP_USE_TEXTCTRL

			//	set the control values
			this->UpdateControl(i_pUIInfo.get());

			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}

			//	hook up events
			//
			this->Connect( m_pActualControl->GetId(), wxEVT_COMMAND_DIRPICKER_CHANGED,
							wxCommandEventHandler(pqtTemplate_DirPicker::DirPicker_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pqtTemplate_DirPicker()
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
		void DirPicker_ValueChanged(wxCommandEvent& i_Event)
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

		wxDirPickerCtrl* m_pActualControl;
#endif // USE_QT
};

