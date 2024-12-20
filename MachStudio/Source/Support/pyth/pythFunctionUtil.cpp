/****************************************************************************\
**	pythFunctionUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythFunctionUtil.hpp"

#include "Core/ma/maFloatRGBA.hpp"
#include "Core/name/nameString.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace pythFunctionUtil
{
	//--------------------------------------------------------------------
	// Increment reference to Py_None and return it, to represent
	//	a function that returns void.
	//--------------------------------------------------------------------
	PyObject* ReturnNone()
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// Convert string to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertString(const std::string &i_String)
	{
		return Py_BuildValue("s", i_String.c_str());
	}

	//--------------------------------------------------------------------
	// Variations of integer conversion
	//--------------------------------------------------------------------
	PyObject* ConvertInt(envType::Int32 i_Value)
	{
		return Py_BuildValue("l", i_Value);
	}
	PyObject* ConvertInt(envType::Int64 i_Value)
	{
		return Py_BuildValue("L", i_Value);
	}
	PyObject* ConvertInt(envType::UInt32 i_Value)
	{
		return Py_BuildValue("k", i_Value);
	}
	PyObject* ConvertInt(envType::UInt64 i_Value)
	{
		return Py_BuildValue("K", i_Value);
	}

	//--------------------------------------------------------------------
	// Convert floating point
	//--------------------------------------------------------------------
	PyObject* ConvertFloat(float i_Value)
	{
		return Py_BuildValue("f", i_Value);
	}

	//--------------------------------------------------------------------
	// Convert boolean to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertBoolean(bool i_Value)
	{
		//return Py_BuildValue("b", i_Value);

		if (i_Value)
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
		else
		{
			Py_INCREF(Py_False);
			return Py_False;
		}
	}

	//--------------------------------------------------------------------
	// Convert color to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertColor(const maFloatRGBA &i_Color)
	{
		return Py_BuildValue("(f,f,f,f)",i_Color.GetRed(),i_Color.GetGreen(),i_Color.GetBlue(),i_Color.GetAlpha());
	}

	//--------------------------------------------------------------------
	// Convert name list to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertNameList(const std::vector<nameString> &i_NameList)
	{
		const int num_names = i_NameList.size();
		PyObject* NameList = PyList_New(num_names);
		if (NameList != NULL)
		{
			for (int i=0; i<num_names; ++i)
			{
				PyList_SetItem(NameList, i, PyUnicode_FromString(i_NameList[i].GetString().c_str()));
			}
		//? Py_DECREF(NameList);
		}
		return NameList;
	}

}	// end of namespace

#endif