/*****************************************************************************
**	prefsLayoutMgr.hpp
**
**		API for layout configuration of panes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PREFS_LAYOUTMGR_HPP
#error prefsLayoutMgr.hpp multiply included
#endif
#define PREFS_LAYOUTMGR_HPP

#ifndef TWX_SYSTEM_HPP
#include "ToolUIWx/twx/twxSystem.hpp"
#endif 

#ifdef USE_WXWIDGETS

#include <vector>

//============================================================================
//============================================================================
class fsLocator;

//============================================================================
//============================================================================
namespace prefsLayoutMgr
{
	//------------------------------------------------------------------------
	//	load in the configuration of panes from the last run
	//------------------------------------------------------------------------
	void LoadLastLayout();

	//------------------------------------------------------------------------
	//	save the current configuration of panes to the registry
	//------------------------------------------------------------------------
	void SaveLastLayout();

	//------------------------------------------------------------------------
	// Get strings for names of layouts
	//------------------------------------------------------------------------
	void GetLayoutNames(std::vector<std::string>& o_LayoutNames);

	//------------------------------------------------------------------------
	// Get name of last layout that was saved or switch to.
	// Can be empty string.
	//------------------------------------------------------------------------
	const std::string& GetCurrentLayoutName();

	//------------------------------------------------------------------------
	//	save current layout as a named layout to save to config file
	//------------------------------------------------------------------------
	bool SaveNamedLayout(const std::string &i_Name);

	//------------------------------------------------------------------------
	//	Change Layout to named layout configuration
	//------------------------------------------------------------------------
	bool SwitchToNamedLayout(const std::string &i_Name);

	//------------------------------------------------------------------------
	//	Change Layout in a cycle through the names
	//------------------------------------------------------------------------
	void SwitchToNextLayout();
	void SwitchToPrevLayout();

	//------------------------------------------------------------------------
	//	remove layout with given name
	//------------------------------------------------------------------------
	void DeleteNamedLayout(const std::string &i_Name);

	//------------------------------------------------------------------------
	// Write layout information to XML file in Configs directory
	//------------------------------------------------------------------------
	void WriteLayoutsToConfigFile();

	//------------------------------------------------------------------------
	// Read layout information from given XML file
	//------------------------------------------------------------------------
	void ReadLayoutsFromConfigFile(const fsLocator& i_ConfigFile);

	//------------------------------------------------------------------------
	// Read layout information from XML file in Configs directory
	//	or from the default layout file installed with the application.
	//------------------------------------------------------------------------
	void ReadLayouts();
}

#endif //USE_WXWIDGETS