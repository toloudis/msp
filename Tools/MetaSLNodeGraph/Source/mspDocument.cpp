/*****************************************************************************
**	mspDocument.cpp
**
**	 The mspDocument class holds the chunks of information
**	that represents a file
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "mspDocument.hpp"
#include "msl/mslMetaSLMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
mspDocument::mspDocument()
: m_bIsLoading(false)
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
mspDocument::~mspDocument()
{
	mslMetaSLMgr::ClearGraph();
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  mspDocument::Clear()
{
	m_Filename.Clear();

	mslMetaSLMgr::ClearGraph();
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
bool  mspDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	m_bIsLoading = true;
	if (mslMetaSLMgr::LoadGraph(i_Locator))
		m_Filename = i_Locator;
	m_bIsLoading = false;
	return true;
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  mspDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	mslMetaSLMgr::SaveGraph(i_Locator);
	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsLoading - return true if this document is currently loading
//--------------------------------------------------------------------
bool  mspDocument::IsLoading()
{
	return m_bIsLoading;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  mspDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  mspDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  mspDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& mspDocument::GetFilename() const
{
	return m_Filename;
}
void  mspDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
