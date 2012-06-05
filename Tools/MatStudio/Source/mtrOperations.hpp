/*****************************************************************************
**	mtrOperations.hpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef MTR_OPERATIONS_HPP
#error mtrOperations.hpp multiply included
#endif
#define MTR_OPERATIONS_HPP

class mtrMaterialTemplate;

namespace mtrOperations
{

	//--------------------------------------------------------------------
	// SetSelectedMaterialIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedMaterialIndex(int i_Index);

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeMaterialData(const mtrMaterialTemplate &i_Data);

	//--------------------------------------------------------------------
	// Use data structure from template manager to set material for 
	//	selected material
	//--------------------------------------------------------------------
	void ApplyMaterialTemplate(const mtrMaterialTemplate &i_Data);

}	// end of namespace
