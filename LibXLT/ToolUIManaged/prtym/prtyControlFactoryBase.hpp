#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyControlFactoryBase.hpp
//**
//**		Control factory for base controls.
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_CONTROLFACTORYBASE_HPP
//#error prtyControlFactoryBase.hpp multiply included
//#endif
//#define PRTY_CONTROLFACTORYBASE_HPP
//
//#ifndef PRTY_ANGLE_NUMERICUPDOWN_HPP
//#include "ToolUIManaged/prtym/prtyAngle_NumericUpDown.hpp"
//#endif
//#ifndef PRTY_BOOLEAN_CHECKBOX_HPP
//#include "ToolUIManaged/prtym/prtyBoolean_CheckBox.hpp"
//#endif
//#ifndef PRTY_CONTROLFACTORY_HPP
//#include "ToolUIManaged/prtym/prtyControlFactory.hpp"
//#endif
//#ifndef PRTY_ENUM_COMBOBOX_HPP
//#include "ToolUIManaged/prtym/prtyEnum_ComboBox.hpp"
//#endif
//#ifndef PRTY_FLOAT_COMBOBOX_HPP
//#include "ToolUIManaged/prtym/prtyFloat_ComboBox.hpp"
//#endif
//#ifndef PRTY_FLOAT_FLOATEDIT_HPP
//#include "ToolUIManaged/prtym/prtyFloat_FloatEdit.hpp"
//#endif
//#ifndef PRTY_FLOAT_NUMERICUPDOWN_HPP
//#include "ToolUIManaged/prtym/prtyFloat_NumericUpDown.hpp"
//#endif
//#ifndef PRTY_FILENAME_COMBOBOX_HPP
//#include "ToolUIManaged/prtym/prtyFileName_ComboBox.hpp"
//#endif
//#ifndef PRTY_FILENAME_TEXTBOX_HPP
//#include "ToolUIManaged/prtym/prtyFileName_TextBox.hpp"
//#endif
//#ifndef PRTY_INT8_NUMERICUPDOWN_HPP
//#include "ToolUIManaged/prtym/prtyInt8_NumericUpDown.hpp"
//#endif
//#ifndef PRTY_INT32_COMBOBOX_HPP
//#include "ToolUIManaged/prtym/prtyInt32_ComboBox.hpp"
//#endif
//#ifndef PRTY_INT32_NUMERICUPDOWN_HPP
//#include "ToolUIManaged/prtym/prtyInt32_NumericUpDown.hpp"
//#endif
//#ifndef PRTY_LISTCHECKED_LISTBOX_HPP
//#include "ToolUIManaged/prtym/prtyListChecked_ListBox.hpp"
//#endif
//#ifndef PRTY_NAME_COMBOBOX_HPP
//#include "ToolUIManaged/prtym/prtyName_ComboBox.hpp"
//#endif
//#ifndef PRTY_NAME_LISTBOX_HPP
//#include "ToolUIManaged/prtym/prtyName_ListBox.hpp"
//#endif
//#ifndef PRTY_NAME_TEXTBOX_HPP
//#include "ToolUIManaged/prtym/prtyName_TextBox.hpp"
//#endif
//#ifndef PRTY_PROPERTY_HPP
//#include "Core/prty/prtyProperty.hpp"
//#endif
//#ifndef PRTY_PROPERTYUIINFO_HPP
//#include "Core/prty/prtyPropertyUIInfo.hpp"
//#endif
//#ifndef PRTY_TEXT_COMBOBOX_HPP
//#include "ToolUIManaged/prtym/prtyText_ComboBox.hpp"
//#endif
//#ifndef PRTY_TEXT_TEXTBOX_HPP
//#include "ToolUIManaged/prtym/prtyText_TextBox.hpp"
//#endif
//#ifndef PRTY_TRIGGER_BUTTON_HPP
//#include "ToolUIManaged/prtym/prtyTrigger_Button.hpp"
//#endif
//
//#include <limits>
//#include <string>
//
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyControlFactoryBase : public prtyControlFactory
//{
//	public:
//		//----------------------------------------------------------------------------
//		//	Check for Control + Property pairing
//		//----------------------------------------------------------------------------
//		virtual prtyControl^ CreateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyControl^ pPropertyControl = nullptr;
//			const std::string& control_name		= i_pUIInfo->GetControlName();
//
//			// only need first one (since the rest should be the same at this point)
//			const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();
//			
//			if ( strcmp(control_name.c_str(),"Button") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Trigger") == 0)
//				{
//					pPropertyControl = gcnew prtyTrigger_Button(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"CheckBox") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Boolean") == 0)
//				{
//					pPropertyControl = gcnew prtyBoolean_CheckBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"ComboBox") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Enum") == 0)
//				{
//					pPropertyControl = gcnew prtyEnum_ComboBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"FileName") == 0)
//				{
//					pPropertyControl = gcnew prtyFileName_ComboBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Float") == 0)
//				{
//					pPropertyControl = gcnew prtyFloat_ComboBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Int32") == 0)
//				{
//					pPropertyControl = gcnew prtyInt32_ComboBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Name") == 0)
//				{
//					pPropertyControl = gcnew prtyName_ComboBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Text") == 0)
//				{
//					pPropertyControl = gcnew prtyText_ComboBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"ListBox") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Name") == 0)
//				{
//					pPropertyControl = gcnew prtyName_ListBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"ListChecked") == 0)
//				{
//					pPropertyControl = gcnew prtyListChecked_ListBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"NumericUpDown") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Angle") == 0)
//				{
//					pPropertyControl = gcnew prtyAngle_NumericUpDown(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Float") == 0)
//				{
//					pPropertyControl = gcnew prtyFloat_NumericUpDown(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Int8") == 0)
//				{
//					pPropertyControl = gcnew prtyInt8_NumericUpDown(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Int32") == 0)
//				{
//					pPropertyControl = gcnew prtyInt32_NumericUpDown(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"TextBox") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"FileName") == 0)
//				{
//					pPropertyControl = gcnew prtyFileName_TextBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Name") == 0)
//				{
//					pPropertyControl = gcnew prtyName_TextBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Text") == 0)
//				{
//					pPropertyControl = gcnew prtyText_TextBox(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//
//			return nullptr;
//		};
//};
//
//#endif // _MANAGED
