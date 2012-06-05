/*****************************************************************************
**	tqtPropertyDialog.hpp
**
**	Modal dialog for gathering data using property interface.
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
#include <QtGui/QWidget>

#include "ToolUIQt/tqt/GeneratedFiles/tqtPropertyDialogBase.h"		// Base class generated from Qt Designer



//============================================================================
// Class DialogTabbed
//============================================================================
class tqtPropertyDialog : public Ui_tqtPropertyDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqtPropertyDialog(  QWidget* parent, 
							const QString& i_Title,
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							const char* i_Message = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqtPropertyDialog(  QWidget* parent, 
							const QString& i_Title,
							std::vector<std::string>& i_RowNames,
							std::vector<std::string>& i_ColumnNames,
							std::vector<prtyObject*>& i_Rows);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~tqtPropertyDialog();

	private:
	
};

#endif // QT_FINISH_PORT