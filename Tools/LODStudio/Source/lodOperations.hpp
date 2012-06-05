/*****************************************************************************
**	lodOperations.hpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef LOD_OPERATIONS_HPP
#error lodOperations.hpp multiply included
#endif
#define LOD_OPERATIONS_HPP

class lodMaterialTemplate;

namespace lodOperations
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
	void ChangeMaterialData(const lodMaterialTemplate &i_Data);

}	// end of namespace
