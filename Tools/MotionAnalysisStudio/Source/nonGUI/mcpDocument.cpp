/*****************************************************************************
**	mcpDocument.cpp
**
**	 The mcpDocument class holds the chunks of information
**	that represents a file
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "mcpDocument.hpp"
#include "mcpSkeleton.hpp"

#include "dbgLog.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
mcpDocument::mcpDocument()
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
mcpDocument::~mcpDocument()
{
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  mcpDocument::Clear()
{
	mcpSkeleton::Clear();
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
void  mcpDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	itString filename = i_Locator.GetLastName();

	// Load as geometry
	mcpSkeleton::LoadModel(i_Locator);
	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  mcpDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	//mcpSkeleton::Save(i_Locator);

	//m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  mcpDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  mcpDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  mcpDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& mcpDocument::GetFilename() const
{
	return m_Filename;
}
void  mcpDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
