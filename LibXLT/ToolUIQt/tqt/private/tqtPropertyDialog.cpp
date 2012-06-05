/*****************************************************************************
**	tqtPropertyDialog.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtPropertyDialog.hpp"

#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"


#ifdef USE_QT

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtPropertyDialog::tqtPropertyDialog( QWidget* parent, 
									  const QString& i_Title,
									  const prtyPropertyUIInfoContainer& i_PropertyContainer,
									  const char* i_Message ) 
:	Ui_tqtPropertyDialogBase()//( parent, qtID_ANY, i_Title )
{
	if (i_Message)
	{
		m_StaticText_Message->setText(QString(i_Message));
	}

	std::string dialog_name("PropertyDialog"); // not needed, not using categories
#ifdef QT_FINISH_PORT
	pqtFormControlBuilder::BuildForm(m_panel_Properties, dialog_name, i_PropertyContainer );

	// Make sure the properties panel is big enough to see some properties,
	// or all of them if just a few.
	int scw_w=0, scw_h=0;
	m_panel_Properties->GetVirtualSize(&scw_w,&scw_h);
	m_panel_Properties->SetMinSize(qtSize(-1, (scw_h < 400) ? scw_h : 400));

	// Redo layout and try to get the dialog to resize based
	// on the new size of the property panel
	this->Layout();
	this->GetSizer()->Fit( this );
#endif

	//bga - Note the above two sizing lines only work if they have been commented out in the
	// generated code in tqtPropertyDialogBase's constructor.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtPropertyDialog::tqtPropertyDialog( QWidget* parent, 
									const QString& i_Title,
									std::vector<std::string>& i_RowNames,
									std::vector<std::string>& i_ColumnNames,
									std::vector<prtyObject*>& i_Rows)
:	Ui_tqtPropertyDialogBase()//( parent, qtID_ANY, i_Title )
{
#ifdef QT_FINISH_PORT
	pqtFormControlBuilder::BuildGridForm(m_panel_Properties, i_RowNames, i_ColumnNames, i_Rows );

	// Make sure the properties panel is big enough to see some properties,
	// or all of them if just a few.
	int scw_w=0, scw_h=0;
	m_panel_Properties->GetVirtualSize(&scw_w,&scw_h);
	m_panel_Properties->SetMinSize(qtSize(-1, (scw_h < 400) ? scw_h : 400));

	// Redo layout and try to get the dialog to resize based
	// on the new size of the property panel
	this->Layout();
	this->GetSizer()->Fit( this );
#endif

	//bga - Note the above two sizing lines only work if they have been commented out in the
	// generated code in tqtPropertyDialogBase's constructor.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtPropertyDialog::~tqtPropertyDialog()
{
#ifdef QT_FINISH_PORT
	pqtFormControlBuilder::ClearForm(m_panel_Properties);
#endif
}

#endif // USE_qtWIDGETS
