/*****************************************************************************
**  gsupTreeCtrlUtil.hpp
**
**	Helper functions to easily add tree nodes to wxWidget's wxTreeCtrl
**	based on a fsLocator path.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GSUP_TREECTRLUTIL_HPP
#error gsupTreeCtrlUtil.hpp multiply included
#endif
#define GSUP_TREECTRLUTIL_HPP

#ifdef MSP_GUI_QT
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#else
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#endif

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif


#ifdef USE_WXWIDGETS
#include <wx/treectrl.h>
#elif defined(USE_QT)
class QTreeWidget;
class QTreeWidgetItem;
#endif


//============================================================================
//============================================================================
namespace gsupTreeCtrlUtil
{
#ifdef USE_WXWIDGETS

	//--------------------------------------------------------------------
	//	Populate the TreeView with the FileList hierarchy.  Directories
	//	will be branches in the tree and files will be the leaves.
	//--------------------------------------------------------------------
	void PopulateTreeView( wxTreeCtrl* io_pTreeCtrl, 
						   const fsysFileList& i_FileList, 
						   bool i_bExpandAll );

	//--------------------------------------------------------------------
	// Find node with given text as child of parent node. 
	// Check IsOk() of returned value to see if found.
	//--------------------------------------------------------------------
	wxTreeItemId FindNode(wxTreeCtrl* io_pTreeCtrl, 
						   wxTreeItemId i_Parent, 
						   const wxString &i_Text);

	//--------------------------------------------------------------------
	// Find node with given text within whole tree control. 
	// Check IsOk() of returned value to see if found.
	//--------------------------------------------------------------------
	wxTreeItemId FindNode(wxTreeCtrl* io_pTreeCtrl, 
						   const wxString &i_Text);

	//--------------------------------------------------------------------
	// Hidden root nodes cannot be expanded and collapsed, so we have to
	// expand the or collapse the children of the root node
	//--------------------------------------------------------------------
	void ExpandRoot(wxTreeCtrl* io_pTreeCtrl, bool i_bAllChildren = true);
	void CollapseAll(wxTreeCtrl* io_pTreeCtrl);

#elif defined(USE_QT)
	//--------------------------------------------------------------------
	//	Populate the TreeView with the FileList hierarchy.  Directories
	//	will be branches in the tree and files will be the leaves.
	//	The invisible root item of the tree widget plays the hidden root.
	//--------------------------------------------------------------------
	void PopulateTreeView( QTreeWidget* io_pTreeCtrl,
						   const fsysFileList& i_FileList,
						   bool i_bExpandAll );

	//--------------------------------------------------------------------
	// Find node with given text as child of parent node.
	// Returns NULL if not found.
	//--------------------------------------------------------------------
	QTreeWidgetItem* FindNode(QTreeWidgetItem* i_pParent,
							  const QString &i_Text);

	//--------------------------------------------------------------------
	// Find node with given text within whole tree control.
	// Returns NULL if not found.
	//--------------------------------------------------------------------
	QTreeWidgetItem* FindNode(QTreeWidget* io_pTreeCtrl,
							  const QString &i_Text);

	//--------------------------------------------------------------------
	// Expand or collapse the children of the (hidden) root node
	//--------------------------------------------------------------------
	void ExpandRoot(QTreeWidget* io_pTreeCtrl, bool i_bAllChildren = true);
	void CollapseAll(QTreeWidget* io_pTreeCtrl);
#endif // USE_WXWIDGETS
}
