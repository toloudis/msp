/*****************************************************************************\
**  rprfPrefsCategory.cpp
**		see .hpp	
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\*****************************************************************************/
#include "Support/rprf/rprfPrefsCategory.hpp"

#include "Core/dbg/dbgMsg.hpp"
//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
rprfPrefsCategory::rprfPrefsCategory(std::string i_Name)
:	m_Name(i_Name)
{
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
rprfPrefsCategory::~rprfPrefsCategory()
{
}

//------------------------------------------------------------------------
// Add the given render type to the category's map
//------------------------------------------------------------------------
void rprfPrefsCategory::AddRenderer( int i_RendererID )
{
	//initialize category's visibility with this render type
	m_VisibilityMap[i_RendererID] = true;
}

//------------------------------------------------------------------------
// Returns whether or not this category should be populated for the
// given renderer
//------------------------------------------------------------------------
bool rprfPrefsCategory::GetCategoryVisible( int i_RendererID )
{	
	std::map<int, bool>::iterator it, end = m_VisibilityMap.end();
	it = m_VisibilityMap.find(i_RendererID);
	if( it != end )
	{
		return (*it).second;
	}
	return false;
}

//------------------------------------------------------------------------
// Sets whether or not this category should be populated for the
// given renderer
//------------------------------------------------------------------------
void rprfPrefsCategory::SetCategoryVisible( int i_RendererID, bool i_bVisible )
{
	std::map<int, bool>::iterator it, end = m_VisibilityMap.end();
	it = m_VisibilityMap.find(i_RendererID);
	if( it != end )
	{
		(*it).second = i_bVisible;
	}
}
