/*****************************************************************************
**	guiPropertyGrid.hpp
**
**		A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_PROPERTYGRID_HPP
#error guiPropertyGrid.hpp multiply included
#endif
#define GUI_PROPERTYGRID_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif 
#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiPropertyGridImpl;


//============================================================================
// static functions define API
//============================================================================
class guiPropertyGrid : public envAbstraction<guiPropertyGridImpl>
{
public:
	enum ReturnValue
	{
		e_Cancel = 0,
		e_OK = 1
	};

	//--------------------------------------------------------------------
	//	Show a modal dialog with the given properties. 
	//	Returns a dialog result, OK or Cancel.
	//--------------------------------------------------------------------
	static ReturnValue ShowModal(	const char* i_DialogTitle, 
		std::vector<std::string>& i_RowNames,
		std::vector<std::string>& i_ColumnNames,
		std::vector<prtyObject*>& i_Rows);
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiPropertyGridImpl
{
public:
	//--------------------------------------------------------------------
	//	Show a modal dialog with the given properties. 
	//	Returns a dialog result, OK or Cancel.
	//--------------------------------------------------------------------
	virtual guiPropertyGrid::ReturnValue ShowModal(	const char* i_DialogTitle, 
		std::vector<std::string>& i_RowNames,
		std::vector<std::string>& i_ColumnNames,
		std::vector<prtyObject*>& i_Rows) = 0;
};


