/*****************************************************************************
**	lodDocument.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "lodDocument.hpp"

//#include "lodLODTemplate.hpp"
//#include "prtGeneratorUtil.hpp"
#include "lodLevel.hpp"

#include "dbgLog.hpp"


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
lodDocument::lodDocument()
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
lodDocument::~lodDocument()
{
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  lodDocument::Clear()
{
//	lodLevel::Clear();
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
void  lodDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	lodLevel::Load( i_Locator );

	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  lodDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	lodLevel::Save( i_Locator );

	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  lodDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  lodDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  lodDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& lodDocument::GetFilename() const
{
	return m_Filename;
}
void  lodDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
