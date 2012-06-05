/****************************************************************************\
**	pythPropertyUtil.hpp
**
**		Python Property Utilities related to python commands
**
**	StudioGPU
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

#ifndef PRTY_PROPERTY_HPP
#include "Core/prty/prtyProperty.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace pythPropertyUtil
{
	prtyProperty* get_property(prtyObject* i_pObject, const std::string &i_PropertyName);
#if defined(PYTHON_ENABLED)
	PyObject* get_properties(prtyObject *pPrtyObj);
    PyObject* set_value(prtyProperty* pProperty, prtyObject *pPrtyObj, PyObject* pValueArg, bool i_bPreserveNameUID = true);
	PyObject* set_property(prtyObject *pPrtyObj, PyObject *args, bool i_bPreserveNameUID);
	PyObject* get_value(prtyProperty* pProperty);
	PyObject* get_value(prtyObject *pPrtyObj, const char *propertyName);
#endif

}