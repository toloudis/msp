/****************************************************************************\
**	PrefsPythLayouts.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/PrefsPythLayouts.hpp"

#ifndef PREFSDATA_HPP
#include "Features/Prefs/PrefsData.hpp"
#endif
#ifndef PREFSMGR_HPP
#include "Features/Prefs/PrefsMgr.hpp"
#endif

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

//============================================================================
//============================================================================
namespace 
{
	//---------------------------------------------------------------------------	
	//---------------------------------------------------------------------------	
	void read_current( const std::string& layout )
	{

	}
	void write_current( const std::string& layout )
	{

	}
	void save_current(const std::string& layout)
	 {
	 	PrefsData& data = PrefsMgr::Data();
		data.m_CurrentLayout.SetValue( layout );
		read_current( layout );
		write_current( layout );
	 }
	
	
	//--------------------------------------------------------------------
	// Load a saved layout
	//--------------------------------------------------------------------
	PyObject *
	load_layout(PyObject *self, PyObject *args)
	{
		const char *layoutName;
		if (!PyArg_ParseTuple(args, "s", &layoutName))
			return NULL;

		std::string set_layout(layoutName);
		PrefsData& data = PrefsMgr::Data();

		data.m_CurrentLayout.SetValue(set_layout);
		read_current(set_layout);
		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Save the current layout
	//--------------------------------------------------------------------
	PyObject *
	save_layout(PyObject *self, PyObject *args)
	{
		return NULL;
	}

	//--------------------------------------------------------------------
	// Delete a layout by name
	//--------------------------------------------------------------------
	PyObject *
	delete_layout(PyObject *self, PyObject *args)
	{
		return NULL;
	}

}


//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void prefsPython::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, 
		"loadLayout", 
		"Load the layout of the given name.", 
		load_layout);

	pythModules::AddCommand(i_ModuleName, 
		"saveLayout", 
		"Save the current layout as the given name.", 
		save_layout);

	pythModules::AddCommand(i_ModuleName, 
		"deleteLayout", 
		"delte the layout of the given name.", 
		delete_layout);
}

#endif //namespace prefsPython

