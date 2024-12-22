/****************************************************************************\
**	pythUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Support/pyth/pythPython.hpp"

#include <algorithm>
#include <string>

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

void replace_all(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'xx'
	}
}

//--------------------------------------------------------------------
// Execute the following python script file with a list of arguments
//--------------------------------------------------------------------
void pythUtil::ScriptFile(fsLocator &i_ScriptLoc, std::string& i_Arguments)
{	
	//first set the arguments
	std::string import = std::string("import sys");
	pythUtil::ExecuteCommand(import);

	char quote[] = "\"";
	
	//quote_tok = strtok( i_Arguments.c_str(), quote );

	replace_all(i_Arguments, "\\", "\\\\");
	replace_all(i_Arguments, "\"", "\\\"");
	replace_all(i_Arguments, "[", "\\\"");
	replace_all(i_Arguments, "]", "\\\"");

	std::string arguments = std::string("arg_string = \"") + i_Arguments + std::string("\"\n");
	//arguments += std::string("arg_string = arg_string.strip(\"\\\"\")") + std::string("\n");
	arguments += std::string("arg_string = arg_string.strip()") + std::string("\n");
	arguments += std::string("temp_list = arg_string.split(\"\\\"\")") + std::string("\n");
	arguments += std::string("arg_list = []") + std::string("\n");
	arguments += std::string("i = 0") + std::string("\n");
	
	//Maybe extract this to an external script?
	arguments += std::string("while i < len(temp_list):") + std::string("\n");
	arguments += std::string("\targs = []") + std::string("\n");
	arguments += std::string("\tif (i%2) == 0:") + std::string("\n");
	arguments += std::string("\t\targs = str(temp_list[i]).split(\" \")") + std::string("\n");
	arguments += std::string("\t\tfor a in args:") + std::string("\n");
	arguments += std::string("\t\t\tif a != \"\":") + std::string("\n");
	arguments += std::string("\t\t\t\targ_list.append(a)") + std::string("\n");
	arguments += std::string("\telse:") + std::string("\n");
	arguments += std::string("\t\targ_list.append(str(temp_list[i]))") + std::string("\n");
	arguments += std::string("\ti += 1") + std::string("\n");
	arguments += std::string("sys.argv = arg_list");
	pythUtil::ExecuteCommand(arguments);

	//now execute the script
	pythUtil::ScriptFile(i_ScriptLoc);
}

void pythUtil::ScriptFile(fsLocator &i_ScriptLoc)
{
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_ScriptLoc, filename);

	// Cheating, just using python's execfile() command instead of API
	std::string command = std::string("execfile(\"") + filename.c_str() + std::string("\")");
	replace_all( command, "\\", "/" );
	DBG_TRACE("Executing Python command (" << command << ")");
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