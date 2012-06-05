/****************************************************************************\
**	pythPropertyUtil.hpp
**
**		Python Property Utilities related to python commands
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_PROPERTY_UTIL_HPP
#error pythPropertyUtil.hpp multiply included
#endif
#define PYTH_PROPERTY_UTIL_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Core/prty/prtyProperty.hpp"
#include "Core/prty/prtyObject.hpp"
#include <string>


//============================================================================
//============================================================================
namespace pythPropertyUtil
{
	prtyProperty* get_property(prtyObject* i_pObject, const std::string &i_PropertyName);
#if defined(PYTHON_ENABLED)
	PyObject* get_properties(prtyObject *pPrtyObj);
    PyObject* set_value(prtyProperty* pProperty, PyObject* pValueArg, bool i_bPreserveNameUID = true);
	PyObject* set_property(prtyObject *pPrtyObj, PyObject *args, bool i_bPreserveNameUID);
	PyObject* get_value(prtyProperty* pProperty);
	PyObject* get_value(prtyObject *pPrtyObj, const char *propertyName);
#endif

}