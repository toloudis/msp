/****************************************************************************\
**	PrefsPythLayouts.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/PrefsPythLayouts.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#ifndef PREFSDATA_HPP
#include "Features/Prefs/PrefsData.hpp"
#endif
#include "Features/Prefs/mGUI/prefsLayoutConfigNameForm.h"
#ifndef PREFSMGR_HPP
#include "Features/Prefs/PrefsMgr.hpp"
#endif

#ifndef TMA_REGISTRYUTIL_HPP
#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif






// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace 
{
	
	void read_current( const std::string& layout )
	{
		#ifdef _MANAGED
		tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(layout.c_str());
		tmaDialogMemoryMgr::Read();
		tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default");	 
		#endif
	}
	void write_current( const std::string& layout )
	{
		#ifdef _MANAGED
		tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(layout.c_str());
		tmaDialogMemoryMgr::Write();
		tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default");	
		#endif
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
		#ifdef _MANAGED
		const char *layoutName;
		if (!PyArg_ParseTuple(args, "s", &layoutName))
			return NULL;
		
		std::string set_layout(layoutName);
		PrefsData& data = PrefsMgr::Data();

		data.m_CurrentLayout.SetValue(set_layout);
		std::string keyname(tmaDialogMemory::lc_Key_LayoutConfiguration);
		keyname += "\\";
		keyname += data.m_CurrentLayout.GetValue();
		tmaRegistryUtil::SetKey( tmaRegistryUtil::e_CurrentUser, keyname );
		
		//	add it to the list
		//this->listView_layoutconfigs->Items->Add( gcnew System::String(data.m_CurrentLayout.GetValue().c_str()) );
		//this->listView_layoutconfigs->Invalidate();

		//	set config to selected name, write out locations, reset to default name
		tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(data.m_CurrentLayout.GetValue().c_str());
		tmaDialogMemoryMgr::Write();
		tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default");	 

		save_current(set_layout);
		#endif
		return NULL;
	}

	//--------------------------------------------------------------------
	// Delete a layout by name
	//--------------------------------------------------------------------
	PyObject *
	delete_layout(PyObject *self, PyObject *args)
	{
		#ifdef _MANAGED
		const char *layoutName;
		if (!PyArg_ParseTuple(args, "s", &layoutName))
			return NULL;
		
		std::string set_layout(layoutName);
		PrefsData& data = PrefsMgr::Data();

		if ( data.m_CurrentLayout == set_layout)
		{
			data.m_CurrentLayout = tmaDialogMemory::lc_Key_LayoutConfig_Default;
			read_current( data.m_CurrentLayout.GetValue() );
		}
		std::string keyname(tmaDialogMemory::lc_Key_LayoutConfiguration);
		keyname += "\\";
		keyname += set_layout;
		tmaRegistryUtil::DeleteKey( tmaRegistryUtil::e_CurrentUser, keyname );
		#endif
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

