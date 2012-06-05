/*****************************************************************************
**  gsupTreeCtrlUtil.hpp
**
**	Helper functions to easily add tree nodes to wxWidget's wxTreeCtrl
**	based on a fsLocator path.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GSUP_TREECTRLUTIL_HPP
#error gsupTreeCtrlUtil.hpp multiply included
#endif
#define GSUP_TREECTRLUTIL_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif


#ifdef USE_WXWIDGETS
#include <wx/treectrl.h>
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
						   const std::string &i_Text);

	//--------------------------------------------------------------------
	// Find node with given text within whole tree control. 
	// Check IsOk() of returned value to see if found.
	//--------------------------------------------------------------------
	wxTreeItemId FindNode(wxTreeCtrl* io_pTreeCtrl, 
						   const std::string &i_Text);

	//--------------------------------------------------------------------
	// Hidden root nodes cannot be expanded and collapsed, so we have to
	// expand the or collapse the children of the root node
	//--------------------------------------------------------------------
	void ExpandRoot(wxTreeCtrl* io_pTreeCtrl, bool i_bAllChildren = true);
	void CollapseAll(wxTreeCtrl* io_pTreeCtrl);

#endif // USE_WXWIDGETS
}
