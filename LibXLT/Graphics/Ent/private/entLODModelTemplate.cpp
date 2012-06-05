/*****************************************************************************
**  entLODModelTemplate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entLODModelTemplate.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//	Constructor
//	i_PathIndex is the Model index for this entity template, under this
// directory the Textures, Models, Sounds and Data directories can be found.
//----------------------------------------------------------------------------
entLODModelTemplate::entLODModelTemplate()
:	m_CurrentTemplate(0)
{
	m_ModelTemplates.resize(0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
entLODModelTemplate::~entLODModelTemplate()
{
	//envSTLHelpers::DeleteContainer(m_ModelTemplates);
	int i;
	for ( i = 0 ; i < m_ModelTemplates.size() ; ++i )
	{
		delete m_ModelTemplates[i].m_pTemplate;
	}
}

//----------------------------------------------------------------------------
//	add a model template.  the return value is the index of it.
//----------------------------------------------------------------------------
int entLODModelTemplate::AddTemplate( entModelTemplate* i_pModelTemplate, std::string& i_ModelFile, float i_fLODDistance, bool i_bDefault )
{
	DBG_ASSERT0( i_pModelTemplate != 0, "Cannot add a NULL model template" );

	int index = m_ModelTemplates.size();
	m_ModelTemplates.resize( index + 1 );

	m_ModelTemplates[index].m_pTemplate = i_pModelTemplate;
	m_ModelTemplates[index].m_fDistance = i_fLODDistance;
	m_ModelTemplates[index].m_bDefault	= i_bDefault;
	m_ModelTemplates[index].m_ModelFile	= i_ModelFile;

	return index;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
int	entLODModelTemplate::GetNumberOfTemplates() const
{
	return m_ModelTemplates.size();
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
entModelTemplate* entLODModelTemplate::GetTemplate( int i_Index ) const
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	return m_ModelTemplates[ i_Index ].m_pTemplate;
}

//--------------------------------------------------------------------
//	delete the current template at i_Index and set the new one
//--------------------------------------------------------------------
void entLODModelTemplate::ReplaceTemplate( int i_Index, entModelTemplate* i_pModelTemplate )
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	if ( m_ModelTemplates[ i_Index ].m_pTemplate != 0 )
	{
		delete m_ModelTemplates[ i_Index ].m_pTemplate;
	}

	m_ModelTemplates[ i_Index ].m_pTemplate = i_pModelTemplate;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float entLODModelTemplate::GetLODDistance( int i_Index ) const
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	return m_ModelTemplates[ i_Index ].m_fDistance;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void entLODModelTemplate::SetLODDistance( int i_Index, float i_fDist )
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	m_ModelTemplates[ i_Index ].m_fDistance = i_fDist;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool entLODModelTemplate::GetLODDefault( int i_Index ) const
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	return m_ModelTemplates[ i_Index ].m_bDefault;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void entLODModelTemplate::SetLODDefault( int i_Index, bool i_bDefault )
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	m_ModelTemplates[ i_Index ].m_bDefault = i_bDefault;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string& entLODModelTemplate::GetLODModelFile( int i_Index ) const
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	return m_ModelTemplates[ i_Index ].m_ModelFile;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void entLODModelTemplate::SetLODModelFile( int i_Index, std::string& i_ModelFile )
{
	DBG_ASSERT0( i_Index < m_ModelTemplates.size(), "Index out of range" );

	//if ( i_ModelFile != m_ModelTemplates[ i_Index ].m_ModelFile )
	//	m_ModelTemplates[ i_Index ].m_bModelNeedsToBeReloaded = true;

	m_ModelTemplates[ i_Index ].m_ModelFile = i_ModelFile;
}
