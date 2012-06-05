/*****************************************************************************\
**  rprfPrefsCategory.hpp
**		Defines the class that contains the property objects for a 
**		category in the render prefs dialog.	
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\*****************************************************************************/
#ifdef RPRF_PREFSCATEGORY_HPP
#error rprfPrefsCategory.hpp multiply included
#endif
#define RPRF_PREFSCATEGORY_HPP

#include <string>
#include <map>

//============================================================================
//============================================================================
class rprfPrefsCategory
{
public:
	//------------------------------------------------------------------------
	// Constructor
	//------------------------------------------------------------------------
	rprfPrefsCategory(std::string i_Name);

	//------------------------------------------------------------------------
	// Destructor
	//------------------------------------------------------------------------
	~rprfPrefsCategory();

	//------------------------------------------------------------------------
	// Add the given render type to the category's map
	//------------------------------------------------------------------------
	void AddRenderer( int i_RendererID );

	//------------------------------------------------------------------------
	// Returns whether or not this category should be populated for the
	// given renderer
	//------------------------------------------------------------------------
	bool GetCategoryVisible( int i_RendererID );

	//------------------------------------------------------------------------
	// Sets whether or not this category should be populated for the
	// given renderer
	//------------------------------------------------------------------------
	void SetCategoryVisible( int i_RendererID, bool i_bVisible );

private:
	std::string m_Name;
	std::map<int, bool> m_VisibilityMap;  //rendererID & visibility value
};