/*****************************************************************************
**	tqtPropertyDialog.hpp
**
**		Modal dialog showing property controls for a list of properties
**	(or a grid of property objects).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_PROPERTYDIALOG_HPP
#error tqtPropertyDialog.hpp multiply included
#endif
#define TQT_PROPERTYDIALOG_HPP

#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#endif

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

#include "ToolUIQt/tqt/private/tqtPropertyDialogBase.h"

//============================================================================
//============================================================================
class tqtPropertyDialog : public tqtPropertyDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqtPropertyDialog( QWidget* parent,
							const QString& i_Title,
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							const char* i_Message = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqtPropertyDialog( QWidget* parent,
							const QString& i_Title,
							std::vector<std::string>& i_RowNames,
							std::vector<std::string>& i_ColumnNames,
							std::vector<prtyObject*>& i_Rows);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~tqtPropertyDialog();

	private:
		void fit_to_properties();
};

#endif // USE_QT
