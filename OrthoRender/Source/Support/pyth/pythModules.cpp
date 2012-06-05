/****************************************************************************\
**	pythModules.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythModules.hpp"
//#include "Support/pyth/pythPython.hpp"

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"

#include <vector>
#include <map>

#if defined(PYTHON_ENABLED)

namespace
{
	bool l_bSubmitted = false;

	struct MethodFunc
	{
		std::string m_CommandName;
		std::string m_CommandDescription;
		pythModules::CommandFunctionPtr m_FunctionPtr;
	};

	typedef std::vector<MethodFunc> MethodList;
	struct ModuleData
	{
		MethodList m_MethodList;
		std::vector<PyMethodDef> m_PyMethods;

	};
	//std::map<std::string, MethodList> l_ModuleMap;
	std::map<std::string, ModuleData> l_ModuleMap;

	// Make a copy of the strings so that we can be sure that
	// the strings are valid throughout their use in the 
	// interpretor.
	std::vector<char*> l_StaticStrings;

	const PyMethodDef c_EmptyMethodDef = {NULL, NULL, 0, NULL};

	// functions to support our copies of the static strings
	// used in Python module definitions
	char* copy_string(const char* i_String)
	{
		char *str = new char [::strlen(i_String) + 1];
		::strcpy(str, i_String);
		l_StaticStrings.push_back(str); // store a copy
		return str;
	}
	void delete_string(char* &io_String)
	{
		delete [] io_String;
		io_String = NULL;
	}

} // end of anonymous namespace

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythModules::Initialize()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythModules::DeInitialize()
{
	l_bSubmitted = false;
	l_ModuleMap.clear();
	
	// Delete the copies of the strings that we kept around
	std::vector<char*>::iterator it;
	for (it = l_StaticStrings.begin(); it != l_StaticStrings.end(); ++it)
	{
		delete_string(*it);
	}
	l_StaticStrings.clear();
}

//--------------------------------------------------------------------
// Add a command to the given module.
//--------------------------------------------------------------------
void pythModules::AddCommand(const std::string &i_ModuleName,
							 const std::string &i_CommandName,
							 const std::string &i_CommandDescription,
							 CommandFunctionPtr i_FunctionPtr)
{
	DBG_ASSERT1(!l_bSubmitted, "Cannot add command %s after modules have been submitted.", i_CommandName.c_str());

	if (l_bSubmitted) return;

	// Create structure defining the method. Make static copies of
	// strings since the structure just contains char* pointers.
	//PyMethodDef method_def = { copy_string(i_CommandName.c_str()), 
	//						   i_FunctionPtr, 
	//						   METH_VARARGS, // Maybe need option for this? 
	//						   copy_string(i_CommandDescription.c_str()) };
	
	MethodFunc method_def;
	method_def.m_CommandName = i_CommandName;
	method_def.m_CommandDescription = i_CommandDescription;
	method_def.m_FunctionPtr = i_FunctionPtr;

	ModuleData &methods = l_ModuleMap[i_ModuleName];
	methods.m_MethodList.push_back( method_def );
	//methods.push_back( c_EmptyMethodDef );

	//l_ModuleMap[i_ModuleName].push_back( method_def );
}
#endif

//--------------------------------------------------------------------
// Submit commands that have been gathered for each module.
//--------------------------------------------------------------------
void pythModules::SubmitModules()
{
#if defined(PYTHON_ENABLED)
	DBG_ASSERT0(!l_bSubmitted, "Already submitted modules to Python");
	l_bSubmitted = true;

	pythThreadLock thread_lock;
	std::map<std::string, ModuleData>::iterator it;
	for (it = l_ModuleMap.begin(); it != l_ModuleMap.end(); ++it)
	{
		ModuleData &mdata = it->second;
		MethodList &methods = mdata.m_MethodList;

		// Add sentinel to end array
		//methods.push_back(c_EmptyMethodDef);

		// Add our commands to module by name
		//Py_InitModule( it->first.c_str(), &(methods[0]) );

		// Create an array of python method definitions
		int num_methods = methods.size();
		//PyMethodDef *method_defs = new PyMethodDef[num_methods+1];
		mdata.m_PyMethods.resize(num_methods+1);
		std::vector<PyMethodDef> &method_defs = mdata.m_PyMethods;
		for (int i=0; i<num_methods; i++)
		{
			method_defs[i].ml_name = methods[i].m_CommandName.c_str();
			method_defs[i].ml_meth = methods[i].m_FunctionPtr;
			method_defs[i].ml_flags = METH_VARARGS;
			method_defs[i].ml_doc = methods[i].m_CommandDescription.c_str();
		}
		method_defs[num_methods].ml_name = NULL;
		method_defs[num_methods].ml_meth = NULL;
		method_defs[num_methods].ml_flags = 0;
		method_defs[num_methods].ml_doc = NULL;

		//Py_InitModule( it->first.c_str(), method_defs );
		Py_InitModule( it->first.c_str(), &method_defs[0] );

		// Go ahead and import our module so that we can use them in 
		// our scripts right away
		std::string import_str("import ");
		import_str += it->first;
		PyRun_SimpleString(import_str.c_str());
	}
#endif
}

