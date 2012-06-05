/*****************************************************************************
**	dynOperations.hpp
**
**	Interface for dialogs to change control info
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef DYN_OPERATIONS_HPP
#error dynOperations.hpp multiply included
#endif
#define DYN_OPERATIONS_HPP

#include <string>

//============================================================================
//============================================================================
class dynControlData;
class dynScriptObject;

//============================================================================
//============================================================================
namespace dynOperations
{
	//--------------------------------------------------------------------
	//	Prompt user for where to attach new control.
	//--------------------------------------------------------------------
	void  CreateNewControl(dynScriptObject* i_pObject);

	//--------------------------------------------------------------------
	//	Create control based on given data
	//--------------------------------------------------------------------
	void  CreateControl(dynScriptObject* i_pObject,
						const dynControlData& i_Data);

	//--------------------------------------------------------------------
	//	Delete control with given name.
	//--------------------------------------------------------------------
	void  DeleteControl(dynScriptObject* i_pObject,
						const std::string& i_PartName);

}	// end of namespace
