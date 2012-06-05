/****************************************************************************\
**	pythFunctionUtil.hpp
**
**		Utilities for converting between PyObject and our types for
**	use with writing python functions.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_FUNCTIONUTIL_HPP
#error pythFunctionUtil.hpp multiply included
#endif
#define PYTH_FUNCTIONUTIL_HPP

#ifndef PYTH_PYTHON_HPP
#include "Support/pyth/pythPython.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <string>
#include <vector>

class maFloatRGBA;
class nameString;

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

//============================================================================
//============================================================================
namespace pythFunctionUtil
{
	//--------------------------------------------------------------------
	// Increment reference to Py_None and return it, to represent
	//	a function that returns void.
	//--------------------------------------------------------------------
	PyObject* ReturnNone();

	//--------------------------------------------------------------------
	// Convert string to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertString(const std::string &i_String);

	//--------------------------------------------------------------------
	// Variations of integer conversion
	//--------------------------------------------------------------------
	PyObject* ConvertInt(envType::Int32 i_Value);
	PyObject* ConvertInt(envType::Int64 i_Value);
	PyObject* ConvertInt(envType::UInt32 i_Value);
	PyObject* ConvertInt(envType::UInt64 i_Value);

	//--------------------------------------------------------------------
	// Convert floating point
	//--------------------------------------------------------------------
	PyObject* ConvertFloat(float i_Value);

	//--------------------------------------------------------------------
	// Convert boolean to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertBoolean(bool i_Value);

	//--------------------------------------------------------------------
	// Convert color to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	// Convert name list to pyObject to return from function
	//--------------------------------------------------------------------
	PyObject* ConvertNameList(const std::vector<nameString> &i_NameList);
};

#endif
