/*****************************************************************************
**	chrOperations.hpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef CHR_OPERATIONS_HPP
#error chrOperations.hpp multiply included
#endif
#define CHR_OPERATIONS_HPP

class mtrMaterialTemplate;

namespace chrOperations
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

}	// end of namespace
