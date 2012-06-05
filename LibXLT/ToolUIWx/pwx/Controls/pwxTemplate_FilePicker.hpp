/****************************************************************************\
**	pwxTemplate_FilePicker.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_TEMPLATE_FILEPICKER_HPP
#error pwxTemplate_FilePicker.hpp multiply included
#endif
#define PWX_TEMPLATE_FILEPICKER_HPP

#ifndef PWX_CONTROLUTIL_HPP
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"
#endif
#ifndef PRTY_FILECHOOSERUIINFO_HPP
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#endif
#ifndef TWC_FILEPICKER_HPP
#include "ToolUIWx/twc/twcFilePicker.hpp"
#endif 
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pwxTemplate_FilePicker : public pwxControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pwxTemplate_FilePicker(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							wxWindow* i_pParent)
		:	pwxControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = pwxControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pwxControlUtil::CleanUpWidget<twcFilePicker>(i_pParent);
			//m_pActualControl = new twcFilePicker(i_pParent);

			//	set the control values
			this->UpdateControl(i_pUIInfo.get());

			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}

			//	hook up events
			//
			this->Connect( m_pActualControl->GetId(), wxEVT_VALUE_CHANGED,
							wxCommandEventHandler(pwxTemplate_FilePicker::FilePicker_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pwxTemplate_FilePicker()
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
			}
			m_bLocalChangeNoUpdate = false;
		}

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		void UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
		{
			m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());

			prtyFileChooserUIInfo* pUII = static_cast<prtyFileChooserUIInfo*>(i_pUIInfo);
			m_pActualControl->SetFilter( pUII->GetFileFilter() );
			m_pActualControl->SetShowFileNameOnly( pUII->GetShowFileNameOnly() );
			m_pActualControl->SetInitialDirectory( pUII->GetInitialDirectory() );
			m_pActualControl->SetDirectoryCategory( pUII->GetDirectoryCategory() );
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void FilePicker_ValueChanged(wxCommandEvent& i_Event)
		{
			if (m_bLocalChangeNoUpdate) return;

			m_bLocalChangeNoUpdate = true;
			//if (m_OriginalValue != m_pActualControl->GetValue())
			{
				ValueType value = Converter::GetValueFromControl(m_pActualControl);
				pwxControlUtil::SetCommonValue<ValueType, PropertyType>(this, value);
			}
			m_bLocalChangeNoUpdate = false;
		}

		twcFilePicker* m_pActualControl;
};

#endif // USE_WXWIDGETS
