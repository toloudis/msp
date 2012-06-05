/*****************************************************************************
**	dynOperations.hpp
**
**	Interface for dialogs to change control info
**
**	StudioGPU
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

	//--------------------------------------------------------------------
	// Set the function pointer to update the scene dialog when a control
	// has gone through an undo/redo
	//--------------------------------------------------------------------
	void SetUpdatePlacedFunction(void (*i_CallbackFunction)());

	//--------------------------------------------------------------------
	// Call the update placed function
	//--------------------------------------------------------------------
	void UpdatePlacedDialog();

}	// end of namespace
