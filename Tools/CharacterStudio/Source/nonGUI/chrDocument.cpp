/*****************************************************************************
**	chrDocument.cpp
**
**	 The chrDocument class holds the chunks of information
**	that represents a file
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "nonGUI/chrDocument.hpp"
#include "nonGUI/chrLevel.hpp"

#include "Core/dbg/dbgLog.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
chrDocument::chrDocument(int i_WriteFormat)
: m_WriteFormat( i_WriteFormat )
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
chrDocument::~chrDocument()
{
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  chrDocument::Clear()
{
	chrLevel::Clear();
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
void  chrDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	itString filename = i_Locator.GetLastName();
	if (filename.HasSubString(itString(".chd")))
	{
		chrLevel::Load(i_Locator);
		
		m_Filename = i_Locator;
	}
	else
	{
		// Load as geometry
		chrLevel::LoadModel(i_Locator);

		// don't want to name file unless we loaded a .chd file
		m_Filename.Clear();
	}
	
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  chrDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	chrLevel::Save(i_Locator);

	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  chrDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  chrDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  chrDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// SetWriteFormat - set the format for output
//--------------------------------------------------------------------
//virtual 
void chrDocument::SetWriteFormat( int i_WriteFormat )
{
	m_WriteFormat = i_WriteFormat;
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& chrDocument::GetFilename() const
{
	return m_Filename;
}
void  chrDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
