/*****************************************************************************
**	mtrDocument.cpp
**
**	 The mtrDocument class holds the chunks of information
**	that represents a file
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "mtrDocument.hpp"
#include "mtrLevel.hpp"

#include "dbgLog.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
mtrDocument::mtrDocument()
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
mtrDocument::~mtrDocument()
{
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  mtrDocument::Clear()
{
	mtrLevel::Clear();
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
void  mtrDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	mtrLevel::LoadModel(i_Locator);
	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  mtrDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	mtrLevel::Save(i_Locator);

	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  mtrDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  mtrDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  mtrDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& mtrDocument::GetFilename() const
{
	return m_Filename;
}
void  mtrDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
