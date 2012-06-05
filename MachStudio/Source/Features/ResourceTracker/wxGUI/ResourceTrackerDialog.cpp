/*****************************************************************************
**	ResourceTrackerDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/ResourceTracker/wxGUI/ResourceTrackerDialog.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envString.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#include <stack>

#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//	Static pointer to the instance of the dialog, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
ResourceTrackerDialog* ResourceTrackerDialog::Instance = NULL;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ResourceTrackerDialog::ResourceTrackerDialog( wxWindow* parent )
:	ResourceTrackerDialogBase(parent)
{
	ResourceTrackerDialog::Instance = this;

	// Add this panel to the AUI manager
	wxAuiPaneInfo api = wxAuiPaneInfo().Name(L"Resource Tracker").Caption(L"Resource Tracker").Show().Layer(1).Left();
	twxPaneMgr::AddPane(this, api);

	Update();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ResourceTrackerDialog::~ResourceTrackerDialog()
{
	ResourceTrackerDialog::Instance = NULL;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ResourceTrackerDialog::Update()
{
	fsResourceTrackerData resource_tracker_data;
	docSingleDocumentMgr::GetResourceList(resource_tracker_data);

	update_tree_resources(resource_tracker_data);
	update_tree_hierarchy(resource_tracker_data);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void ResourceTrackerDialog::OnInitDialog( wxInitDialogEvent& event )
{
	Update();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void ResourceTrackerDialog::button_refresh_OnButtonClick( wxCommandEvent& event )
{
	Update();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ResourceTrackerDialog::update_tree_resources(const fsResourceTrackerData& i_ResourceTrackerData)
{
	std::set<fsLocator> file_paths;
	file_paths.clear();
	for (int j = 0; j < i_ResourceTrackerData.m_Resources.size(); ++j)
	{
		fsLocator filepath;
		filepath = i_ResourceTrackerData.m_Resources[j].GetFilePath();
		file_paths.insert(filepath);
	}

	//
	m_treeCtrl_resources->DeleteAllItems();
	fsLocator scenefile = docSingleDocumentMgr::GetFilename();
	itString scenefilestring;
	fsFileUtil::LocatorToUnicodeString(scenefile, scenefilestring);
	m_treeCtrl_resources->AddRoot(scenefilestring.GetString());

	fsLocator assetfile;
	std::set<fsLocator>::const_iterator it;
	for (it = file_paths.begin(); it != file_paths.end(); ++it)
	{
		assetfile = (*it);
		itString assetfile_string;
		fsFileUtil::LocatorToUnicodeString(assetfile, assetfile_string);

		if (assetfile.GetNumNames() > 0)
			wxTreeItemId node_id = m_treeCtrl_resources->AppendItem(m_treeCtrl_resources->GetRootItem(),
																	assetfile_string.GetString());
	}

	//	expand the tree
	m_treeCtrl_resources->ExpandAll();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ResourceTrackerDialog::update_tree_hierarchy(const fsResourceTrackerData& i_ResourceTrackerData)
{
	std::stack<wxTreeItemId> parents;
	m_treeCtrl_hierarchy->DeleteAllItems();
	fsLocator scenefile = docSingleDocumentMgr::GetFilename();
	itString scenefilestring;
	fsFileUtil::LocatorToUnicodeString(scenefile, scenefilestring);
	m_treeCtrl_hierarchy->AddRoot(scenefilestring.GetString());

	//
	//DBG_TRACE("=-- RESOURCES --=");
	wxTreeItemId node_id;
	int last_depth = -1;

	parents.push(m_treeCtrl_hierarchy->GetRootItem());	// root of the tree
	for (int i = 0; i < i_ResourceTrackerData.m_Resources.size(); ++i)
	{
		//	get the depth and determine the parent
		int depth = i_ResourceTrackerData.m_Resources[i].GetDepth();
		if (depth == last_depth)
		{
			parents.pop();
		}
		else if (depth < last_depth)
		{
			int diff = last_depth - depth;
			for (int j=0; j <= diff; ++j)
			{
				parents.pop();
			}
		}

		//	add the item to the tree
		itString assetfile_string;
		fsFileUtil::LocatorToUnicodeString(i_ResourceTrackerData.m_Resources[i].GetFilePath(), assetfile_string);
		if (assetfile_string.GetLength() > 0)
		{
			node_id = m_treeCtrl_hierarchy->AppendItem(parents.top(),
														assetfile_string.GetString());
		}
		else
		{
			std::wstring name_wstring;
			name_wstring = envString::UTF8ToWideChar(i_ResourceTrackerData.m_Resources[i].GetName().GetString());

			node_id = m_treeCtrl_hierarchy->AppendItem(parents.top(), name_wstring);
		}
		//DBG_TRACE( std::setw(2) << i << "-" << depth << " " << space.c_str() << " " << i_ResourceTrackerData.m_Resources[i].GetName() << " = " << fullpath.c_str() << " (" << i_ResourceTrackerData.m_Resources[i].GetReferenceCount() << ")" );

		//	keep track of the node id + depth
		parents.push(node_id);
		last_depth = depth;
	}

	//	collapse the tree
	m_treeCtrl_hierarchy->CollapseAll();
}

#endif
