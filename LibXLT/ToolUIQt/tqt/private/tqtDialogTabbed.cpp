/*****************************************************************************
**	tqtDialogTabbed.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtDialogTabbed.hpp"

#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtDialogTabbedMgr.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"

#ifdef USE_QT

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtDialogTabbed::tqtDialogTabbed( QWidget* parent,
								  const QString& name,
								  const QString& title )
: QWidget( parent )
{
	this->setMinimumSize( 450, 300 );

	QVBoxLayout* pLayout = new QVBoxLayout(this);
	pLayout->setContentsMargins(5, 5, 5, 5);

	m_Notebook = new QTabWidget(this);
	m_Notebook->setMovable(true);
	pLayout->addWidget(m_Notebook);

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(name).Caption(title).Hide().Layer(1).Float());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtDialogTabbed::~tqtDialogTabbed()
{
	tqtDialogTabbedMgr::NotifyDialogDeleted(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::AddTabPage(const std::string& i_Title)
{
	QScrollArea* pScroll = new QScrollArea(m_Notebook);
	pScroll->setWidgetResizable(true);
	pScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pScroll->setMinimumSize(100, 200);

	QWidget* pTabPage = new QWidget;
	pScroll->setWidget(pTabPage);

	m_Notebook->addTab( pScroll, tqtQString(i_Title) );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int tqtDialogTabbed::find_page(const std::string& i_Title)
{
	QString title = tqtQString(i_Title);
	for (int i=0; i<m_Notebook->count(); ++i)
	{
		if (title == m_Notebook->tabText(i))
			return i;
	}
	return -1;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
QWidget* tqtDialogTabbed::page_contents(int i_Index)
{
	QScrollArea* pScroll = dynamic_cast<QScrollArea*>(m_Notebook->widget(i_Index));
	return pScroll ? pScroll->widget() : NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::RemoveTabPage(const std::string& i_Title)
{
	// wxAuiNotebook::RemovePage left the page alive, but nothing holds on
	// to it, so it is deleted here like DeleteTabPage.
	DeleteTabPage(i_Title);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::DeleteTabPage(const std::string& i_Title)
{
	int index = find_page(i_Title);
	if (index >= 0)
	{
		if (QWidget* pPage = page_contents(index))
			pqtFormControlBuilder::ClearForm(pPage);
		QWidget* pScroll = m_Notebook->widget(index);
		m_Notebook->removeTab(index);
		delete pScroll;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::RemoveTabPages()
{
	// Remove all property controls from the tab pages before deleting
	while (m_Notebook->count() > 0)
	{
		if (QWidget* pPage = page_contents(0))
			pqtFormControlBuilder::ClearForm(pPage);
		QWidget* pScroll = m_Notebook->widget(0);
		m_Notebook->removeTab(0);
		delete pScroll;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
QWidget* tqtDialogTabbed::GetTabPage(const std::string& i_Title)
{
	int index = find_page(i_Title);
	return (index >= 0) ? page_contents(index) : NULL;
}

#endif // USE_QT
