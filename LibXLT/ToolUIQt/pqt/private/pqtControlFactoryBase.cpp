/****************************************************************************\
**	pqtControlFactoryBase.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/pqtControlFactoryBase.hpp"

#include "ToolUIQt/pqt/Controls/pqtBoolean_CheckBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtEnum_ComboBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtFileName_ComboBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtFileName_TextBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtFloat_ComboBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtInt32_ComboBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtListChecked_CheckListBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtName_ComboBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtName_ListBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtName_TextBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtText_ComboBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtText_TextBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtTrigger_Button.hpp"



#ifdef USE_QT
//----------------------------------------------------------------------------
//	Check for Control + Property pairing
//----------------------------------------------------------------------------
//virtual 
pqtControl* pqtControlFactoryBase::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
												 QWidget* i_pParent)
{
	//pqtControl* return NULL;
	const std::string& control_name		= i_pUIInfo->GetControlName();

	// only need first one (since the rest should be the same at this point)
	const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();

	if ( strcmp(control_name.c_str(),"CheckBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"Boolean") == 0)
		{
			return new pqtBoolean_CheckBox(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"Button") == 0)
	{
		if ( strcmp(property_type.c_str(),"Trigger") == 0)
		{
			return new pqtTrigger_Button(i_pUIInfo, i_pParent);
		}
	}
#ifdef QT_FINISH_PORT
	else if ( strcmp(control_name.c_str(),"ComboBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"Enum") == 0)
		{
			return new pqtEnum_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"FileName") == 0)
		{
			return new pqtFileName_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pqtFloat_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int32") == 0)
		{
			return new pqtInt32_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Name") == 0)
		{
			return new pqtName_ComboBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Text") == 0)
		{
			return new pqtText_ComboBox(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"ListBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"Name") == 0)
		{
			return new pqtName_ListBox(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"ListChecked") == 0)
		{
			return new pqtListChecked_CheckListBox(i_pUIInfo, i_pParent);
		}
	}
#endif
#ifdef QT_FINISH_PORT
	else if ( strcmp(control_name.c_str(),"TextBox") == 0)
	{
		if ( strcmp(property_type.c_str(),"FileName") == 0)
		{
			return new pqtFileName_TextBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Name") == 0)
		{
			return new pqtName_TextBox(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Text") == 0)
		{
			return new pqtText_TextBox(i_pUIInfo, i_pParent);
		}
	}
#endif

	return NULL;
}

#endif // USE_QT