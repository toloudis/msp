/*****************************************************************************
**	fcuiListViewOperations.hpp
**
**		Class to perform qt specific operations on panels
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_LISTVIEWOPERATIONS_HPP
#error fcuiListViewOperations.hpp multiply included
#endif
#define FCUI_LISTVIEWOPERATIONS_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <QtGui>


//============================================================================
//============================================================================
class fcuiListViewOperations : public QObject
{
	Q_OBJECT

public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fcuiListViewOperations();

	//------------------------------------------------------------------------
	/// Shows the panel
	//------------------------------------------------------------------------
	void LoadListView(const fsLocator& i_DirectoryName, QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration);
	
	//------------------------------------------------------------------------
	/// First hides the panel and then shows it after changing any necessary 
	/// data.
	//------------------------------------------------------------------------
	void ReLoadListView(const fsLocator& i_DirectoryName, QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration);

	//------------------------------------------------------------------------
	/// Hides the list view
	//------------------------------------------------------------------------
	void RemoveListView(QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration);

	//------------------------------------------------------------------------
	/// Animate the panel into view
	//------------------------------------------------------------------------
	void ShowListView(QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration);

	//------------------------------------------------------------------------
	/// Animate the panel out of view
	//------------------------------------------------------------------------
	void HideListView(QWidget* i_ListView, const QRect& i_StartRect, const QRect& i_EndRect, int i_Duration,  bool i_bLoadList = true);

private slots:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void LoadList();

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PopulateListItems();

private:
	fsLocator m_ListDirectory;
	QWidget* m_pCurrentListView; 
	QRect m_StartRect;
	QRect m_EndRect; 
	int m_Duration;
};