/*****************************************************************************
**  gsupTreeCtrlUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/gsup/gsupTreeCtrlUtil.hpp"

#include "Core/it/itStringUtil.hpp"


#ifdef USE_WXWIDGETS

namespace
{
	//--------------------------------------------------------------------
	//	add a tree branch (and the corresponding parent nodes based on the
	//	path)
	//--------------------------------------------------------------------
	void add_tree_branch( wxTreeCtrl* io_pTreeCtrl, 
						  const fsLocator& i_Path, 
						  const itString& i_Filename )
	{
		DBG_ASSERT( io_pTreeCtrl != NULL, "Cannot have a nullptr wxTreeCtrl" );

		wxTreeItemId parent = io_pTreeCtrl->GetRootItem();
		DBG_ASSERT(parent.IsOk(), "Assuming that there is a root node already in tree control.");
		for ( int i = 0; i < i_Path.GetNumNames(); ++i )
		{
			wxString dname(i_Path.GetName(i).GetString());

			bool bFoundNode = false;

			// Iterate through children
			wxTreeItemId id;
			wxTreeItemIdValue cookie;
			for (id = io_pTreeCtrl->GetFirstChild(parent, cookie);
				 id.IsOk(); 
				 id = io_pTreeCtrl->GetNextChild(parent, cookie))
			{
				if (dname == io_pTreeCtrl->GetItemText(id))
				{
					bFoundNode = true;
					parent = id;
					break;
				}
			}

			if ( !bFoundNode )
			{
				parent = io_pTreeCtrl->AppendItem(parent, dname);
			}
		}

		//	now add the filename
		//
		wxString fname(i_Filename.GetString());
		wxTreeItemId node = gsupTreeCtrlUtil::FindNode(io_pTreeCtrl, parent, fname);
		if (!node.IsOk()) // If not already added..
		{
			parent = io_pTreeCtrl->AppendItem(parent, fname);
		}
	}
	
	//--------------------------------------------------------------------
	// Find node with given text within subtree rooted at i_Parent. 
	// Check IsOk() of returned value to see if found.
	//--------------------------------------------------------------------
	wxTreeItemId find_node_recursive(wxTreeCtrl* io_pTreeCtrl, 
								   wxTreeItemId i_Parent, 
								   const wxString &i_Text)
	{
		wxTreeItemId id;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(i_Parent, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(i_Parent, cookie))
		{
			if (i_Text == io_pTreeCtrl->GetItemText(id))
			{
				// found it
				break;
			}
			else
			{
				wxTreeItemId found_node = find_node_recursive(io_pTreeCtrl, id, i_Text);
				if (found_node.IsOk())
					return found_node;
			}
		}
		return id; // id.IsOk() == false if not found
	}
}

//============================================================================
//============================================================================
namespace gsupTreeCtrlUtil
{

	//--------------------------------------------------------------------
	//	Populate the TreeView with the FileList hierarchy.  Directories
	//	will be branches in the tree and files will be the leaves.
	//--------------------------------------------------------------------
	void PopulateTreeView( wxTreeCtrl* io_pTreeCtrl, 
						   const fsysFileList& i_FileList, 
						   bool i_bExpandAll )
	{
		io_pTreeCtrl->DeleteAllItems();

		io_pTreeCtrl->AddRoot(L"Root");
	
		fsLocator path;
		int num_items = i_FileList.Size();
		for (int i=0; i<num_items; i++)
		{
			i_FileList.GetFilePath(i, path);
			add_tree_branch( io_pTreeCtrl, path, i_FileList.GetFilename(i) );
		}

		if (i_bExpandAll)
			gsupTreeCtrlUtil::ExpandRoot(io_pTreeCtrl);
	}

	//--------------------------------------------------------------------
	// Find node with given text as child of parent node. 
	// Check IsOk() of returned value to see if found.
	//--------------------------------------------------------------------
	wxTreeItemId FindNode(wxTreeCtrl* io_pTreeCtrl, 
						   wxTreeItemId i_Parent, 
						   const wxString &i_Text)
	{
		wxTreeItemId id;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(i_Parent, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(i_Parent, cookie))
		{
			if (i_Text == io_pTreeCtrl->GetItemText(id))
			{
				// found it
				break;
			}
		}
		return id; // id.IsOk() == false if not found
	}

	//--------------------------------------------------------------------
	// Find node with given text within whole tree control. 
	// Check IsOk() of returned value to see if found.
	//--------------------------------------------------------------------
	wxTreeItemId FindNode(wxTreeCtrl* io_pTreeCtrl, 
						   const wxString &i_Text)
	{
		wxTreeItemId root = io_pTreeCtrl->GetRootItem();
		DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");

		// Iterate through children
		wxTreeItemId id, found_node;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(root, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(root, cookie))
		{
			found_node = find_node_recursive(io_pTreeCtrl, id, i_Text);
			if (found_node.IsOk())
				return found_node;
		}

		return found_node; // wil be IsOk() == false if not found
	}

	//--------------------------------------------------------------------
	// Hidden root nodes cannot be expanded and collapsed, so we have to
	// expand or collapse the children of the root node
	//--------------------------------------------------------------------
	void ExpandRoot(wxTreeCtrl* io_pTreeCtrl, bool i_bAllChildren)
	{
		wxTreeItemId root = io_pTreeCtrl->GetRootItem();
		DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");

		// Iterate through children
		wxTreeItemId id;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(root, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(root, cookie))
		{
			if (i_bAllChildren)
				io_pTreeCtrl->ExpandAllChildren(id);
			else
				io_pTreeCtrl->Expand(id);
		}
	}
	void CollapseAll(wxTreeCtrl* io_pTreeCtrl)
	{
		wxTreeItemId root = io_pTreeCtrl->GetRootItem();
		DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");

		// Iterate through children
		wxTreeItemId id;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(root, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(root, cookie))
		{
			io_pTreeCtrl->CollapseAllChildren(id);
		}
	}
	

}	// end of namespace

#elif defined(USE_QT)

#include <QTreeWidget>

namespace
{
	//--------------------------------------------------------------------
	//	add a tree branch (and the corresponding parent nodes based on the
	//	path)
	//--------------------------------------------------------------------
	void add_tree_branch( QTreeWidget* io_pTreeCtrl,
						  const fsLocator& i_Path,
						  const itString& i_Filename )
	{
		DBG_ASSERT( io_pTreeCtrl != NULL, "Cannot have a nullptr QTreeWidget" );
		QTreeWidgetItem* parent = io_pTreeCtrl->invisibleRootItem();

		for ( int i = 0; i < i_Path.GetNumNames(); ++i )
		{
			QString dname = QString::fromWCharArray(i_Path.GetName(i).GetString());
			QTreeWidgetItem* node = gsupTreeCtrlUtil::FindNode(parent, dname);
			if ( node == NULL )
				node = new QTreeWidgetItem(parent, QStringList(dname));
			parent = node;
		}

		//	now add the filename
		//
		QString fname = QString::fromWCharArray(i_Filename.GetString());
		if (gsupTreeCtrlUtil::FindNode(parent, fname) == NULL) // If not already added..
		{
			new QTreeWidgetItem(parent, QStringList(fname));
		}
	}

	//--------------------------------------------------------------------
	// Find node with given text within subtree rooted at i_Parent.
	//--------------------------------------------------------------------
	QTreeWidgetItem* find_node_recursive(QTreeWidgetItem* i_pParent,
										 const QString &i_Text)
	{
		for (int i = 0; i < i_pParent->childCount(); ++i)
		{
			QTreeWidgetItem* pChild = i_pParent->child(i);
			if (i_Text == pChild->text(0))
				return pChild;

			QTreeWidgetItem* pFound = find_node_recursive(pChild, i_Text);
			if (pFound)
				return pFound;
		}
		return NULL;
	}
}

//============================================================================
//============================================================================
namespace gsupTreeCtrlUtil
{
	//--------------------------------------------------------------------
	//	Populate the TreeView with the FileList hierarchy.  Directories
	//	will be branches in the tree and files will be the leaves.
	//--------------------------------------------------------------------
	void PopulateTreeView( QTreeWidget* io_pTreeCtrl,
						   const fsysFileList& i_FileList,
						   bool i_bExpandAll )
	{
		io_pTreeCtrl->clear();

		fsLocator path;
		int num_items = i_FileList.Size();
		for (int i=0; i<num_items; i++)
		{
			i_FileList.GetFilePath(i, path);
			add_tree_branch( io_pTreeCtrl, path, i_FileList.GetFilename(i) );
		}

		if (i_bExpandAll)
			gsupTreeCtrlUtil::ExpandRoot(io_pTreeCtrl);
	}

	//--------------------------------------------------------------------
	// Find node with given text as child of parent node.
	//--------------------------------------------------------------------
	QTreeWidgetItem* FindNode(QTreeWidgetItem* i_pParent,
							  const QString &i_Text)
	{
		for (int i = 0; i < i_pParent->childCount(); ++i)
		{
			if (i_Text == i_pParent->child(i)->text(0))
				return i_pParent->child(i);
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	// Find node with given text within whole tree control.
	//--------------------------------------------------------------------
	QTreeWidgetItem* FindNode(QTreeWidget* io_pTreeCtrl,
							  const QString &i_Text)
	{
		return find_node_recursive(io_pTreeCtrl->invisibleRootItem(), i_Text);
	}

	//--------------------------------------------------------------------
	// Expand or collapse the children of the (hidden) root node
	//--------------------------------------------------------------------
	void ExpandRoot(QTreeWidget* io_pTreeCtrl, bool i_bAllChildren)
	{
		if (i_bAllChildren)
		{
			io_pTreeCtrl->expandAll();
		}
		else
		{
			QTreeWidgetItem* root = io_pTreeCtrl->invisibleRootItem();
			for (int i = 0; i < root->childCount(); ++i)
				root->child(i)->setExpanded(true);
		}
	}

	void CollapseAll(QTreeWidget* io_pTreeCtrl)
	{
		io_pTreeCtrl->collapseAll();
	}

}	// end of namespace

#endif
