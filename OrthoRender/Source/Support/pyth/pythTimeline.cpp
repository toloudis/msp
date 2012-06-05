/****************************************************************************\
**	pythTimeline.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythTimeline.hpp"

#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{
	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// get current timeline time
	//--------------------------------------------------------------------
	PyObject* get_current_time(PyObject *self, PyObject *args)
	{
		return Py_BuildValue("f", tmlnTimeLine::GetValue());
	}

	//--------------------------------------------------------------------
	// set current timeline time
	//--------------------------------------------------------------------
	PyObject* set_current_time(PyObject *self, PyObject *args)
	{
		float value;
		if (!PyArg_ParseTuple(args, "f", &value))
			return NULL;

		tmlnTimeLine::SetValue( value );

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// get minimum timeline time
	//--------------------------------------------------------------------
	PyObject* get_minimum_time(PyObject *self, PyObject *args)
	{
		return Py_BuildValue("f", tmlnTimeLine::GetMinimum());
	}

	//--------------------------------------------------------------------
	// get maximum timeline time
	//--------------------------------------------------------------------
	PyObject* get_maximum_time(PyObject *self, PyObject *args)
	{
		return Py_BuildValue("f", tmlnTimeLine::GetMaximum());
	}

	//--------------------------------------------------------------------
	// set minimum and maximum timeline time
	//--------------------------------------------------------------------
	PyObject* set_time_range(PyObject *self, PyObject *args)
	{
		float min, max;
		if (!PyArg_ParseTuple(args, "ff", &min, &max))
			return NULL;

		//tmlnTimeLine::SetMaximum( value );
		tmlnTimeLine::SetTimeRange(min,max);

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// update drivers at current time
	//--------------------------------------------------------------------
	PyObject* update_time(PyObject *self, PyObject *args)
	{
		tmlnTimelineMgr::Update( tmlnTimeLine::GetValue() );

		Py_INCREF(Py_None);
		return Py_None;
	}
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void pythTimeline::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, 
		"getCurrentTime", "Get current time in timeline.", get_current_time);
	pythModules::AddCommand(i_ModuleName, 
		"setCurrentTime", "Set current time in timeline.", set_current_time);
	pythModules::AddCommand(i_ModuleName, 
		"getMinimumTime", "Get begin time in timeline.", get_minimum_time);
	pythModules::AddCommand(i_ModuleName, 
		"getMaximumTime", "Get end time in timeline.", get_maximum_time);
	pythModules::AddCommand(i_ModuleName, 
		"setTimeRange", "Set time range in timeline.", set_time_range);

	pythModules::AddCommand(i_ModuleName, "updateTime", 
		"Execute all the drivers at current time in order to put the scene in the state at that time.", 
		update_time);

}

#endif