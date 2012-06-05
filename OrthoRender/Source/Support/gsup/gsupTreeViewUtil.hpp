/*****************************************************************************
**  gsupTreeViewUtil.hpp
**
**	Helper functions to easily add tree nodes based on a fsLocator path.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef GSUP_TREEVIEWUTIL_HPP
#error gsupTreeViewUtil.hpp multiply included
#endif
#define GSUP_TREEVIEWUTIL_HPP

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
enum gsupTreeViewType
{
	eHierarchy = 0,
	eFlat
};

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::Collections;		// iEnumerator
using namespace System::Windows::Forms;

//============================================================================
//	forward references
//============================================================================
class fsLocator;
class itString;

//============================================================================
//============================================================================
public ref class gsupTreeViewUtil
{
	public:
		//--------------------------------------------------------------------
		//	find a tree node based on its Text field
		//--------------------------------------------------------------------
		static TreeNode^ FindTreeNode( TreeNodeCollection^ i_pTNC, String^ i_pTreeNodeText )
		{
			IEnumerator ^ myNodes = (safe_cast<IEnumerable^>(i_pTNC))->GetEnumerator();
			try
			{
				while (myNodes->MoveNext())
				{
					TreeNode ^ pFoundNode = find_treenode(safe_cast<TreeNode^>(myNodes->Current),
														  i_pTreeNodeText);
					if (pFoundNode != nullptr)
					{
						return pFoundNode;
					}
				}
			}
			__finally
			{
				IDisposable ^ disposable = dynamic_cast<System::IDisposable^>(myNodes);
				if (disposable != nullptr) 
					delete disposable;
			}

			return nullptr;
		}

public:
		//--------------------------------------------------------------------
		//	find a tree node based on its Text field
		//--------------------------------------------------------------------
		static TreeNode^ FindTreeNodeThatStartsWith( TreeNodeCollection^ i_pTNC, String^ i_pTreeNodeText )
		{
			IEnumerator ^ myNodes = (safe_cast<IEnumerable^>(i_pTNC))->GetEnumerator();
			try
			{
				while (myNodes->MoveNext())
				{
					TreeNode ^ pFoundNode = find_treenode_thatstartswith(safe_cast<TreeNode^>(myNodes->Current),
																		 i_pTreeNodeText);
					if (pFoundNode != nullptr)
					{
						return pFoundNode;
					}
				}
			}
			__finally
			{
				IDisposable ^ disposable = dynamic_cast<System::IDisposable^>(myNodes);
				if (disposable != nullptr) 
					delete disposable;
			}

			return nullptr;
		}

		//--------------------------------------------------------------------
		//	Populate the TreeView with the FileList hierarchy.  Directories
		//	will be branches in the tree and files will be the leaves.
		//--------------------------------------------------------------------
		static void PopulateTreeView( TreeView^ io_pTreeView, const fsysFileList& i_FileList, bool i_bExpandAll )
		{
			PopulateTreeView(io_pTreeView, i_FileList, eHierarchy, i_bExpandAll);
		}

		//--------------------------------------------------------------------
		//	Populate the TreeView with the FileList hierarchy.  Directories
		//	will be branches in the tree and files will be the leaves.
		//--------------------------------------------------------------------
		static void PopulateTreeView( TreeView^ io_pTreeView, const fsysFileList& i_FileList, gsupTreeViewType i_Type, bool i_bExpandAll )
		{
			DBG_ASSERT0( io_pTreeView != nullptr, "treeView is nullptr" );

			io_pTreeView->Nodes->Clear();

			fsLocator path;
			int num_items = i_FileList.Size();

			switch (i_Type)
			{
				case eHierarchy:
					{
						for (int i=0; i<num_items; i++)
						{
							i_FileList.GetFilePath(i,path);
							//path.RemoveBefore(itString("Data"));	// make it relative

							AddTreeBranch( io_pTreeView, path, i_FileList.GetFilename(i) );
						}

						if (i_bExpandAll)
							io_pTreeView->ExpandAll();
						break;
					}
				case eFlat:
					{
						for (int i=0; i<num_items; i++)
						{
							i_FileList.GetFilePath(i,path);
							AddTreeNode( io_pTreeView, i_FileList.GetFilename(i) );
						}

						if (i_bExpandAll)
							io_pTreeView->ExpandAll();
						break;
					}
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		static void SortTreeView( TreeView^ i_pTreeView, bool i_bAscending, bool i_bShallow )
		{
			//DBG_LOG0("Sorting------------------------");
			//std::string strng;
			//tmaManagedStringUtils::ManagedStringToStdString(i_pTreeView->Text, strng);
			//DBG_LOG1("  [%s]", strng.c_str() );

			sort_treenode_children( i_pTreeView->Nodes, i_bAscending, i_bShallow );
		}

		//
		//	Tree Flagging Functions
		//
private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static TreeNode^ flag_treenode( TreeNode^ i_pTreeNode, bool i_bFlag )
		{
			if (i_pTreeNode != nullptr)
			{
				// set the flag
				i_pTreeNode->Tag = i_bFlag;

				//	traverse the tree
				for (int i = 0; i < i_pTreeNode->Nodes->Count; ++i)
				{
					TreeNode^ pTN = flag_treenode(i_pTreeNode->Nodes[i], i_bFlag);
					if (pTN != nullptr)
					{
						return pTN;
					}
				}
			}

			return nullptr;
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static TreeNode^ remove_unflagged_treenode( TreeNode^ i_pTreeNode )
		{
			if (i_pTreeNode != nullptr)
			{
				//std::string tempstr;
				//tmaManagedStringUtils::ManagedStringToStdString(i_pTreeNode->Text, tempstr);
				//DBG_LOG3("--Traversing tree node (%s) to Remove [%d,%s]", tempstr.c_str(), i_pTreeNode->Nodes->Count, (i_pTreeNode->Tag->Equals(false) ? "true":"false") );

				// if leaf node + unflagged, remove this tree node
				//
				//	Note: remove it first so the parent node does NOT get
				//	removed.  (i.e. Don't want "Character" node to get removed
				//	if all the children get removed. (or do we?) [rjk]
				//
				if (   (i_pTreeNode->Nodes->Count == 0)
					&& (i_pTreeNode->Tag->Equals(false)) )
				{
					//std::string tempstr;
					//tmaManagedStringUtils::ManagedStringToStdString(i_pTreeNode->Text, tempstr);
					//DBG_LOG1("--Trying to Remove (%s)", tempstr.c_str() );

					//	remove the node from the tree
					//
					if (i_pTreeNode->Parent != nullptr)
						i_pTreeNode->Parent->Nodes->Remove(i_pTreeNode);
					return nullptr;
				}

				//	traverse the tree
				for (int i = i_pTreeNode->Nodes->Count - 1; i >= 0; --i)
				{
					TreeNode^ pTN = remove_unflagged_treenode(i_pTreeNode->Nodes[i]);
					if (pTN != nullptr)
					{
						return pTN;
					}
				}
			}

			return nullptr;
		}
public:
		//------------------------------------------------------------------------
		// flag the tree nodes with the passed in flag
		//------------------------------------------------------------------------
		static void FlagTreeNodes(TreeView^ io_pTreeView, bool i_bFlag)
		{
			//DBG_LOG1("Tree node has %d children", i_pTreeNodeCollection->get_Count() );
			for (int i=0; i < io_pTreeView->Nodes->Count; ++i)
			{
				flag_treenode( io_pTreeView->Nodes[i], i_bFlag);
			}
		}

		//------------------------------------------------------------------------
		//	flag one tree node (not recursively)
		//------------------------------------------------------------------------
		static void FlagTreeNode(TreeNode^ i_pTreeNode, bool i_bFlag)
		{
			if (i_pTreeNode != nullptr)
			{
				i_pTreeNode->Tag = i_bFlag;
			}
		}

		//------------------------------------------------------------------------
		// remove unflagged (false) tree nodes (leaf nodes only)
		//------------------------------------------------------------------------
		static void RemoveUnflaggedTreeNodes(TreeView^ io_pTreeView)
		{
			//TreeNodeCollection^ tiernodes = io_pTreeView->Nodes;

			//DBG_LOG1("***RemoveUTN count = %d", io_pTreeView->Nodes->Count);
			for (int i=0; i < io_pTreeView->Nodes->Count; ++i)
			{
				TreeNode^ pTN = io_pTreeView->Nodes[i];

				//std::string tempstr;
				//tmaManagedStringUtils::ManagedStringToStdString( pTN->Text, tempstr );
				//DBG_LOG2("%02d %s", i, tempstr.c_str());

				remove_unflagged_treenode(pTN);
			}
		}

private:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		static void sort_treenode_children( TreeNodeCollection^ i_pTreeNodeCollection, bool i_bAscending, bool i_bShallow )
		{
			//DBG_LOG1("Tree node has %d children", i_pTreeNodeCollection->get_Count() );
			for (int i=0; i < i_pTreeNodeCollection->Count-1; ++i)
			{
				//	find the smallest/largest
				//
				int picked_index = i;
				for (int j=i+1; j < i_pTreeNodeCollection->Count; ++j)
				{
					if (i_bAscending)
					{
						if ( String::Compare(i_pTreeNodeCollection[picked_index]->Text, i_pTreeNodeCollection[j]->Text) < 0 )
						{
							picked_index = j;
						}
					}
					else
					{
						if ( String::Compare(i_pTreeNodeCollection[picked_index]->Text, i_pTreeNodeCollection[j]->Text) > 0 )
						{
							picked_index = j;
						}
					}
				}

				//	swap the nodes
				//
				if (picked_index != i)
				{
					//TreeNode^ pTN1 = i_pTreeNodeCollection[i];
					//TreeNode^ pTN2 = i_pTreeNodeCollection->get_Item(picked_index);
					//i_pTreeNodeCollection->set_Item(i,pTN2);
					//i_pTreeNodeCollection->set_Item(picked_index,pTN1);
				}

				//	recurse and do the children
				if (!i_bShallow)
				{
					sort_treenode_children( i_pTreeNodeCollection[i]->Nodes, i_bAscending, i_bShallow );
				}
			}
			//DBG_LOG0("-end");
		}

		//--------------------------------------------------------------------
		//	add a tree node
		//--------------------------------------------------------------------
		static void AddTreeNode( TreeView^ io_pTreeView, const itString& i_Filename )
		{
			String^ pFname = tmaManagedStringUtils::ItStringToManagedString(i_Filename);
			io_pTreeView->Nodes->Add( gcnew TreeNode( pFname ) );
		}

		//--------------------------------------------------------------------
		//	add a tree branch (and the corresponding parent nodes based on the
		//	path)
		//--------------------------------------------------------------------
		static void AddTreeBranch( TreeView^ io_pTreeView, const fsLocator& i_Path, const itString& i_Filename )
		{
			DBG_ASSERT0( io_pTreeView != nullptr, "Cannot have a nullptr TreeView" );

			TreeNodeCollection^ tiernodes = io_pTreeView->Nodes;

			for ( int i = 0; i < i_Path.GetNumNames(); ++i )
			{
				String^ pFname = tmaManagedStringUtils::ItStringToManagedString(i_Path.GetName(i));

				bool bFoundNode = false;

				for ( int j = 0; j < tiernodes->Count; ++j )
				{
					if (tiernodes[j]->Text->Equals(pFname))
					{
						bFoundNode = true;
						tiernodes = tiernodes[j]->Nodes;
						break;
					}
				}

				if ( !bFoundNode )
				{
					//TreeNode node = treeView1.Nodes.Add("Level one node");
					//node.Nodes.Add("Level two node");
					//io_pTreeView->
					TreeNode^ pTN = gcnew TreeNode( pFname );
					tiernodes->Add( pTN );
					tiernodes = pTN->Nodes;
				}
			}

			//	now add the filename
			//
			String^ pFilenameString = tmaManagedStringUtils::ItStringToManagedString(i_Filename);
			TreeNode^ pFoundTN = FindTreeNode( tiernodes, pFilenameString );
			if ( pFoundTN == nullptr )
			{
				tiernodes->Add( gcnew TreeNode( pFilenameString ) );
			}
		}

private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static TreeNode^ find_treenode( TreeNode^ i_pTreeNode, String^ i_pTreeNodeText )
		{
			if (i_pTreeNode != nullptr)
			{
				if ( i_pTreeNode->Text->Equals(i_pTreeNodeText) )
				{
					return i_pTreeNode;
				}

				for (int i = 0; i < i_pTreeNode->Nodes->Count; ++i)
				{
					TreeNode^ pTN = find_treenode(i_pTreeNode->Nodes[i], i_pTreeNodeText);
					if (pTN != nullptr)
					{
						return pTN;
					}
				}
			}

			return nullptr;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static TreeNode^ find_treenode_thatstartswith( TreeNode^ i_pTreeNode, String^ i_pTreeNodeText )
		{
			if (i_pTreeNode != nullptr)
			{
				if ( i_pTreeNode->Text->StartsWith(i_pTreeNodeText) )
				{
					return i_pTreeNode;
				}

				for (int i = 0; i < i_pTreeNode->Nodes->Count; ++i)
				{
					TreeNode^ pTN = find_treenode_thatstartswith(i_pTreeNode->Nodes[i], i_pTreeNodeText);
					if (pTN != nullptr)
					{
						return pTN;
					}
				}
			}

			return nullptr;
		}
};
#endif // _MANAGED
