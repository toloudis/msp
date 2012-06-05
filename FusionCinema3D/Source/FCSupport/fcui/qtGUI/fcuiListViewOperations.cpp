/*****************************************************************************
**	fcuiListViewOperations.hpp
**
**		Class to perform qt specific operations on panels
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiListViewOperations.hpp"

#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include <QtGui/QtGui>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fcuiListViewOperations::fcuiListViewOperations()
:  QObject(),
   m_pCurrentListView(NULL)
{}

//----------------------------------------------------------------------------
/// Shows the panel
//----------------------------------------------------------------------------
void fcuiListViewOperations::LoadListView(const fsLocator& i_DirectoryName, QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration)
{
	m_ListDirectory = i_DirectoryName;
	m_pCurrentListView = i_ListView;
	m_StartRect = i_StartRect;
	m_EndRect = i_EndRect;
	m_Duration = i_Duration;
	LoadList();
}

//----------------------------------------------------------------------------
/// First hides the panel and then shows it after changing any necessary 
/// data.
//----------------------------------------------------------------------------
void fcuiListViewOperations::ReLoadListView(const fsLocator& i_DirectoryName, QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration)
{
	m_ListDirectory = i_DirectoryName;
	m_pCurrentListView = i_ListView;
	m_StartRect = i_StartRect;
	m_EndRect = i_EndRect;
	m_Duration = i_Duration;
	HideListView(i_ListView, i_StartRect, i_EndRect, i_Duration);
}

//----------------------------------------------------------------------------
/// First hides the panel and then shows it after changing any necessary 
/// data.
//----------------------------------------------------------------------------
void fcuiListViewOperations::RemoveListView(QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration)
{
	m_ListDirectory.Clear();
	m_pCurrentListView = i_ListView;
	m_StartRect = i_StartRect;
	m_EndRect = i_EndRect;
	m_Duration = i_Duration;
	ShowListView(i_ListView, i_StartRect, i_EndRect, i_Duration);
}

//----------------------------------------------------------------------------
/// Shows the panel
//----------------------------------------------------------------------------
void fcuiListViewOperations::LoadList()
{
	PopulateListItems();
	ShowListView(m_pCurrentListView, m_StartRect, m_EndRect, m_Duration);
}

//----------------------------------------------------------------------------
/// Animate the panel into view
//----------------------------------------------------------------------------
void fcuiListViewOperations::ShowListView(QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration)
{
	QPropertyAnimation *animation = new QPropertyAnimation(i_ListView, "geometry");

	animation->setDuration(i_Duration);
	animation->setStartValue(i_StartRect);
	animation->setEndValue(i_EndRect);
	animation->setEasingCurve(QEasingCurve::Linear);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
}

//----------------------------------------------------------------------------
/// Animate the panel out of view
//----------------------------------------------------------------------------
void fcuiListViewOperations::HideListView(QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration, bool i_bLoadList)
{
	QPropertyAnimation *animation = new QPropertyAnimation(i_ListView, "geometry");

	animation->setDuration(i_Duration);
	animation->setStartValue(i_StartRect);
	animation->setEndValue(i_EndRect);
	animation->setEasingCurve(QEasingCurve::Linear);
	if( !i_bLoadList )
		animation->setEasingCurve(QEasingCurve::InBack);
	
	animation->start(QPropertyAnimation::DeleteWhenStopped);

	m_EndRect = i_StartRect;
	m_StartRect = i_EndRect;

	if(i_bLoadList)
		QObject::connect(animation, SIGNAL(finished()), this, SLOT(LoadList()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiListViewOperations::PopulateListItems()
{
	fcuiFormMgr::PopulateEditList(m_ListDirectory);
}
