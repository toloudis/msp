/*****************************************************************************
**  quiPropertyGrid.hpp
**
**      A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_PROPERTYGRID_HPP
#error quiPropertyGrid.hpp multiply included
#endif
#define QUI_PROPERTYGRID_HPP

#ifndef GUI_PROPERTYGRID_HPP
#include "Tool/gui/guiPropertyGrid.hpp"
#endif 

//============================================================================
// forward declaration
//============================================================================
class quiPropertyGridImpl;

//============================================================================
// static functions define API
//============================================================================
class quiPropertyGrid : public guiPropertyGridImpl
{
public:

	//--------------------------------------------------------------------
	//	Show a modal dialog with the given properties. 
	//	Returns a dialog result, OK or Cancel.
	//--------------------------------------------------------------------
	virtual guiPropertyGrid::ReturnValue ShowModal(const char* i_DialogTitle, 
		std::vector<std::string>& i_RowNames,
		std::vector<std::string>& i_ColumnNames,
		std::vector<prtyObject*>& i_Rows);
};


