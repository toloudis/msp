/****************************************************************************\
**	pwxControlFactoryBase.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/pwxControlFactoryBase.hpp"

#include "ToolUIWx/pwx/Controls/pwxBoolean_CheckBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxEnum_ComboBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxFileName_ComboBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxFileName_TextBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxFilePath_ComboBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxFloat_ComboBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxInt32_ComboBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxListChecked_CheckListBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxName_ComboBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxName_ListBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxName_TextBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxText_ComboBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxText_TextBox.hpp"
#include "ToolUIWx/pwx/Controls/pwxTrigger_Button.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//	Check for Control + Property pairing
//----------------------------------------------------------------------------
//virtual 
pwxControl* pwxControlFactoryBase::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
												 wxWindow* i_pParent)
{
	//pwxControl* return NULL;
	const std::string& control_name		= i_pUIInfo->GetControlName();

	// only need first one (since the rest should be the same at this point)
	const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();
	
	if ( strcmp(control_name.c_str(),"Button") == 0)
	{
		if ( strcmp(property_type.c_str(),"Trigger") == 0)
		{
			return new pwxTrigger_Button(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"CheckBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"Boolean") == 0)
		{
			return new pwxBoolean_CheckBox(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"ComboBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"Enum") == 0)
		{
			return new pwxEnum_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"FileName") == 0)
		{
			return new pwxFileName_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"File Path") == 0)
		{
			return new pwxFilePath_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pwxFloat_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int32") == 0)
		{
			return new pwxInt32_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Name") == 0)
		{
			return new pwxName_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Text") == 0)
		{
			return new pwxText_ComboBox(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"ListBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"Name") == 0)
		{
			return new pwxName_ListBox(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"ListChecked") == 0)
		{
			return new pwxListChecked_CheckListBox(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"TextBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"FileName") == 0)
		{
			return new pwxFileName_TextBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Name") == 0)
		{
			return new pwxName_TextBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Text") == 0)
		{
			return new pwxText_TextBox(i_pUIInfo, i_pParent);
		}
	}

	return NULL;
}

#endif // USE_WXWIDGETS