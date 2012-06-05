/*****************************************************************************
**	ptclDocument.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "ptclDocument.hpp"

//#include "ptclParticleTemplate.hpp"
//#include "Graphics/prt/prtGeneratorUtil.hpp"
#include "ptclLevel.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
ptclDocument::ptclDocument()
{
}


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
ptclDocument::~ptclDocument()
{
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  ptclDocument::Clear()
{
//	ptclLevel::Clear();
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
void  ptclDocument::Load(const fsLocator &i_Locator, bool i_bAppend)
{
	ptclLevel::Load( i_Locator );

	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  ptclDocument::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	ptclLevel::Save( i_Locator );

	m_Filename = i_Locator;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  ptclDocument::IsDirty()
{
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  ptclDocument::SetActive()
{
	//if (m_pObject)
	//	api3dScene::AddObject(m_pObject);
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  ptclDocument::SetInactive( bool i_bUpdateData )
{
	//if (m_pObject)
	//	api3dScene::RemoveObject(m_pObject);
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& ptclDocument::GetFilename() const
{
	return m_Filename;
}
void  ptclDocument::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}
