/*****************************************************************************
**  twxPropertyDialog.hpp
**
**      Modal dialog for gathering data using property interface.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_PROPERTYDIALOG_HPP
#error twxPropertyDialog.hpp multiply included
#endif
#define TWX_PROPERTYDIALOG_HPP

#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#endif 
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif 
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "ToolUIWx/twx/private/twxPropertyDialogBase.h"

//============================================================================
// Class DialogTabbed
//============================================================================
class twxPropertyDialog : public twxPropertyDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twxPropertyDialog( wxWindow* parent, 
							const wxString& i_Title,
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							const char* i_Message = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twxPropertyDialog( wxWindow* parent, 
							const wxString& i_Title,
							std::vector<std::string>& i_RowNames,
							std::vector<std::string>& i_ColumnNames,
							std::vector<prtyObject*>& i_Rows);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~twxPropertyDialog();

	private:
	
};

#endif // USE_WXWIDGETS