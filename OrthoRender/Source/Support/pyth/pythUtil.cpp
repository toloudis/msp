/****************************************************************************\
**	pythUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"

#include "Support/pyth/pythPython.hpp"

namespace
{
}

//--------------------------------------------------------------------
// Execute the following code in the python interpretor
//--------------------------------------------------------------------
bool pythUtil::ExecuteCommand(const std::string& i_Command)
{
	return pythUtil::ExecuteCommand(i_Command.c_str());
}
bool pythUtil::ExecuteCommand(const char *i_Command)
{
#if defined(PYTHON_ENABLED)
	// thread lock object ensures the thread state on constructor
	// and releases on destructor
	pythThreadLock thread_lock;
	if (PyRun_SimpleString( i_Command ) == 0)
		return true;
	else
#endif
		return false;
}

//--------------------------------------------------------------------
// Execute the following python script file.
//--------------------------------------------------------------------
void pythUtil::ScriptFile(fsLocator &i_ScriptLoc)
{
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_ScriptLoc, filename);

	// Cheating, just using python's execfile() command instead of API
	std::string command = std::string("execfile(\"") + filename.c_str() + std::string("\")");
	pythUtil::ExecuteCommand(command);

//	if (PyRun_SimpleFile( FILE *fp, filename.c_str())  == 0)
//		return true;
//	else
//		return false;

	//PyObject* PyFileObject = PyFile_FromString(FileName, "r");
	//if (PyFileObject == NULL) return NULL; 
	//PyRun_SimpleFile(PyFile_AsFile(PyFileObject), FileName);
	//Py_DECREF(PyFileObject);

}

//--------------------------------------------------------------------
// Convert a general string to a token without whitespace
//	that can be used as a Python command or argument.
//--------------------------------------------------------------------
std::string pythUtil::MakeToken(const std::string &i_String)
{
	std::string result;

	const int num_chars = i_String.size();
	result.reserve(num_chars);

	char ch;
	bool have_lower_case = false; // force the first upper case letters to lower case
	bool force_upper = false; // force the first letter after whitespace to upper case
	for (int i=0; i<num_chars; i++)
	{
		ch = i_String[i];
		if (::isalnum(ch))
		{
			if (force_upper)
			{
				result.push_back( ::toupper(ch) );
				force_upper = false;
			}
			else if (have_lower_case)
				result.push_back(ch);
			else
			{
				if (::islower(ch))
					have_lower_case = true;

				result.push_back( ::tolower(ch) );
			}
		}
		else 
		{
			have_lower_case = true; // white space marks end of tolower section
			force_upper = true;	// make next alpha character be upper case
		}
	}

	return result;
}