/****************************************************************************\
**	pqtTemplate_TextureFilePicker.hpp
**
**		Template class that handles multiple property types with a 
**	single control type (TextureFilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEMPLATE_TEXTUREFILEPICKER_HPP
#error pqtTemplate_TextureFilePicker.hpp multiply included
#endif
#define PQT_TEMPLATE_TEXTUREFILEPICKER_HPP

#ifndef PQT_CONTROLUTIL_HPP
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif
#ifndef TQC_TEXTUREFILEPICKER_HPP
#include "ToolUIQt/tqc/tqcTextureFilePicker.hpp"
#endif 

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef PRTY_INTERESTUTIL_HPP
#include "Core/prty/prtyInterestUtil.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
template<class PropertyType, class ValueType, class Converter>
class pqtTemplate_TextureFilePicker : public pqtControl
{
#ifdef QT_FINISH_PORT
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTemplate_TextureFilePicker(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent)
		:	pqtControl(i_pUIInfo)
		{
			DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

			ValueType newvalue;
			bool bSameValue = pqtControlUtil::GetCommonValue<ValueType, PropertyType>(i_pUIInfo, newvalue);

			// create the actual control
			m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcTextureFilePicker>(i_pParent);
			//m_pActualControl = new tqcTextureFilePicker(i_pParent);

			//	set the control values
			this->UpdateControl(i_pUIInfo.get());
			if (bSameValue)
			{
				Converter::SetValueIntoControl(m_pActualControl, newvalue);
			}

			//select the choice item based on the current callback of the control
			m_pActualControl->SelectChoice(GetCallback(i_pUIInfo.get()));
			SetCallback(i_pUIInfo.get());

			m_pActualControl->SetUIInfo(i_pUIInfo.get());
			
			//	hook up events
			//
			this->Connect( m_pActualControl->GetId(), wxEVT_VALUE_CHANGED,
							wxCommandEventHandler(pqtTemplate_TextureFilePicker::TextureFilePicker_ValueChanged) );
			m_pActualControl->PushEventHandler(this);
		}

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pqtTemplate_TextureFilePicker()
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

			prtyTextureFileChooserUIInfo* pUII = static_cast<prtyTextureFileChooserUIInfo*>(i_pUIInfo);
			m_pActualControl->SetFilter( pUII->GetFileFilter() );
			m_pActualControl->SetShowFileNameOnly( pUII->GetShowFileNameOnly() );
			m_pActualControl->SetInitialDirectory( pUII->GetInitialDirectory() );
			m_pActualControl->SetDirectoryCategory( pUII->GetDirectoryCategory() );

			// Get list of string choices for combo box
			std::vector<std::string> choices;
			Converter::GetChoices(i_pUIInfo, choices);

			// Add choices to control
			m_pActualControl->ClearChoices();
			int num_choices = choices.size();
			for (int i = 0; i < num_choices; ++i)
			{
				m_pActualControl->AppendChoice(choices[i]);
			}

			if(num_choices > 0)
				m_pActualControl->SelectChoice(0);
		}

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void TextureFilePicker_ValueChanged(wxCommandEvent& i_Event)
		{
			if (m_bLocalChangeNoUpdate) return;

			m_bLocalChangeNoUpdate = true;
			//if (m_OriginalValue != m_pActualControl->GetValue())
			{
				ValueType value = Converter::GetValueFromControl(m_pActualControl);
				prtyTextureFileData new_val;
				if(m_pActualControl->GetCurrentMode() == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Revert))
				{
					new_val = GetRevertData(m_pActualControl->GetUIInfo());
					if(new_val.m_CurrentCallback != prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture))
						new_val.m_bButtonPressed = m_pActualControl->IsButtonPress();
					m_pActualControl->SetFullpath(new_val.m_TextureLocator);
					m_pActualControl->SelectChoice(new_val.m_CurrentCallback);
					m_pActualControl->CheckInterestInvoke();
				}
				else
				{
					new_val.m_TextureLocator = value;
					new_val.m_bButtonPressed = m_pActualControl->IsButtonPress();
					new_val.m_CurrentCallback = m_pActualControl->GetCurrentMode();
				}
				pqtControlUtil::SetCommonValue<prtyTextureFileData, PropertyType>(this, new_val);
				m_pActualControl->ResetButtonPress();
				if(new_val.m_CurrentCallback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Reset))
					m_pActualControl->SelectChoice(0);
			}
			m_bLocalChangeNoUpdate = false;
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		std::string GetCallback(prtyPropertyUIInfo* i_pUIInfo)
		{
			std::string callback("");
			prtyTextureFileChooserUIInfo* pUII = static_cast<prtyTextureFileChooserUIInfo*>(i_pUIInfo); 
			for (int i=0; i < pUII->GetNumberOfProperties(); ++i)
			{
				prtyTextureFileName* pActualProperty = dynamic_cast<prtyTextureFileName*>(pUII->GetProperty(i));
				DBG_ASSERT(pActualProperty != NULL, "Invalid property type, dynamic cast failed.");
				if (i==0)
					callback = pActualProperty->GetFullValue().m_CurrentCallback;
			}
			return callback;
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		prtyTextureFileData GetRevertData(prtyPropertyUIInfo* i_pUIInfo)
		{
			prtyTextureFileData revert_data;
			if(i_pUIInfo == NULL)
				return revert_data;
			prtyTextureFileChooserUIInfo* pUII = static_cast<prtyTextureFileChooserUIInfo*>(i_pUIInfo); 
			for (int i=0; i < pUII->GetNumberOfProperties(); ++i)
			{
				prtyTextureFileName* pActualProperty = dynamic_cast<prtyTextureFileName*>(pUII->GetProperty(i));
				DBG_ASSERT(pActualProperty != NULL, "Invalid property type, dynamic cast failed.");
				if (i==0)
					revert_data = pActualProperty->GetRevertValue();
			}
			return revert_data;
		}
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void SetCallback(prtyPropertyUIInfo* i_pUIInfo)
		{
			std::vector<prtyInterest*> interests;
			prtyInterestUtil::GetInterestList(interests);
			prtyTextureFileChooserUIInfo* pUII = static_cast<prtyTextureFileChooserUIInfo*>(i_pUIInfo); 
			for (int i=0; i < pUII->GetNumberOfProperties(); ++i)
			{
				prtyTextureFileName* pActualProperty = dynamic_cast<prtyTextureFileName*>(pUII->GetProperty(i));
				
				DBG_ASSERT(pActualProperty != NULL, "Invalid property type, dynamic cast failed.");
				if (i==0)
				{
					prtyTextureFileData new_val;
					new_val.m_TextureLocator = pActualProperty->GetValue();
					new_val.m_CurrentCallback = m_pActualControl->GetCurrentMode();
					pActualProperty->SetValueWithoutNotify(new_val);
				}
			}
			m_pActualControl->CheckInterestInvoke();
		}

		tqcTextureFilePicker* m_pActualControl;
#endif // USE_QT
};

