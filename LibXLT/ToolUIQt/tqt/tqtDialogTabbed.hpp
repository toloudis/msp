/*****************************************************************************
**	tqtDialogTabbed.hpp
**
**		A dockable pane holding scrolling tab pages of property controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_DIALOGTABBED_HPP
#error tqtDialogTabbed.hpp multiply included
#endif
#define TQT_DIALOGTABBED_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class tqtDialogTabbed : public QWidget
{
	public:
		//--------------------------------------------------------------------
		// Creates the pane (hidden, floating) with the given name and title
		//--------------------------------------------------------------------
		tqtDialogTabbed( QWidget* parent,
						 const QString& name,
						 const QString& title );

		~tqtDialogTabbed();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddTabPage(const std::string& i_Title);
		void RemoveTabPage(const std::string& i_Title);
		void DeleteTabPage(const std::string& i_Title);
		void RemoveTabPages();

		//--------------------------------------------------------------------
		// The widget inside the tab's scroll area that holds the controls
		//--------------------------------------------------------------------
		QWidget* GetTabPage(const std::string& i_Title);

	private:
		int find_page(const std::string& i_Title);
		QWidget* page_contents(int i_Index);

		QTabWidget* m_Notebook;
};

#endif // USE_QT
