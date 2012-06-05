/*****************************************************************************
**	mnmDocument.cpp
**
**	 The mnmDocument class holds the chunks of information
**	that represents a file
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "mnmDocument.hpp"

#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
mnmDocument::mnmDocument()
: m_pObject(NULL)
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
mnmDocument::~mnmDocument()
{
	if (m_pObject)
		api3dScene::RemoveObject(m_pObject);
	delete m_pObject;
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  mnmDocument::Clear()
{
	m_Filename.Clear();

	if (m_pObject)
		api3dScene::RemoveObject(m_pObject);
	delete m_pObject;
	m_pObject = NULL;
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
void  mnmDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	m_pObject = api3dImport::LoadObject(i_Locator);

	if (m_pObject)
	{
		api3dScene::AddObject(m_pObject);
		cam3dMgr::FocusCamera(m_pObject->GetWorldBox());
	}

	m_Filename = i_Locator;

}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  mnmDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	// nothing to do here
	//m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  mnmDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  mnmDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  mnmDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& mnmDocument::GetFilename() const
{
	return m_Filename;
}
void  mnmDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
