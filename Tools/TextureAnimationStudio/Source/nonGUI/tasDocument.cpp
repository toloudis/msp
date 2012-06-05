/*****************************************************************************
**	tasDocument.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "tasDocument.hpp"

#include "tasTUVData.hpp"
#include "tasTUVDataUtil.hpp"
#include "tasTUVMgr.hpp"
#include "dbgLog.hpp"


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tasDocument::tasDocument()
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tasDocument::~tasDocument()
{
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  tasDocument::Clear()
{
//	tasTUVMgr::Clear();
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
void  tasDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	tasTUVMgr::LoadTUV( i_Locator );

	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  tasDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	tasTUVData& data = tasTUVDataUtil::Data();
	data.m_TUVFilename = i_Locator;

	tasTUVMgr::SaveTUV( i_Locator );
	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  tasDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  tasDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  tasDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& tasDocument::GetFilename() const
{
	return m_Filename;
}
void  tasDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
