/****************************************************************************\
**	pythTimeline.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythTimeline.hpp"

#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

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
		return Py_BuildValue("f", tmlnTimeLine::GetValue().AsSeconds());
	}
	
	//--------------------------------------------------------------------
	// get current timeline frame
	//--------------------------------------------------------------------
	PyObject* get_current_frame(PyObject *self, PyObject *args)
	{
		maTime time = tmlnTimeLine::GetValue();
		int frame;
		tmlnTimeUtil::GetTimeInFrames(time, frame);
		return Py_BuildValue("i", frame);
	}

	//--------------------------------------------------------------------
	// set current timeline time
	//--------------------------------------------------------------------
	PyObject* set_current_time(PyObject *self, PyObject *args)
	{
		float value;
		if (!PyArg_ParseTuple(args, "f", &value))
			return NULL;

		tmlnTimeLine::SetValue( maTime::FromSeconds(value) );

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// set current timeline time by using frame index
	//--------------------------------------------------------------------
	PyObject* set_current_frame(PyObject *self, PyObject *args)
	{
		int value;
		if (!PyArg_ParseTuple(args, "i", &value))
			return NULL;
		
		tmlnTimeLine::SetValue( tmlnTimeUtil::GetTimeFromFrames(value) );

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// get minimum timeline time
	//--------------------------------------------------------------------
	PyObject* get_minimum_time(PyObject *self, PyObject *args)
	{
		return Py_BuildValue("f", tmlnTimeLine::GetMinimum().AsSeconds());
	}

	PyObject* get_minimum_frame(PyObject *self, PyObject *args)
	{
		maTime time = tmlnTimeLine::GetMinimum();
		int frame;
		tmlnTimeUtil::GetTimeInFrames(time, frame);
		return Py_BuildValue("i", frame);
	}

	//--------------------------------------------------------------------
	// get maximum timeline time
	//--------------------------------------------------------------------
	PyObject* get_maximum_time(PyObject *self, PyObject *args)
	{
		return Py_BuildValue("f", tmlnTimeLine::GetMaximum());
	}

	PyObject* get_maximum_frame(PyObject *self, PyObject *args)
	{
		maTime time = tmlnTimeLine::GetMaximum();
		int frame;
		tmlnTimeUtil::GetTimeInFrames(time, frame);
		return Py_BuildValue("i", frame);
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
		tmlnTimeLine::SetTimeRange(maTime::FromSeconds(min),maTime::FromSeconds(max));

		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject* set_frame_range(PyObject *self, PyObject *args)
	{
		int min, max;
		if (!PyArg_ParseTuple(args, "ii", &min, &max))
			return NULL;
		
		tmlnTimeLine::SetTimeRange(tmlnTimeUtil::GetTimeFromFrames(min),
								   tmlnTimeUtil::GetTimeFromFrames(max));
		
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
		"getCurrentFrame", "Get current frame in timeline.", get_current_frame);
	pythModules::AddCommand(i_ModuleName, 
		"setCurrentTime", "Set current time in timeline.", set_current_time);
	pythModules::AddCommand(i_ModuleName, 
		"setCurrentFrame", "Set current frame in timeline.", set_current_frame);
	pythModules::AddCommand(i_ModuleName, 
		"getMinimumTime", "Get begin time in timeline.", get_minimum_time);
	pythModules::AddCommand(i_ModuleName, 
		"getMinimumFrame", "Get begin frame in timeline.", get_minimum_frame);
	pythModules::AddCommand(i_ModuleName, 
		"getMaximumTime", "Get end time in timeline.", get_maximum_time);
	pythModules::AddCommand(i_ModuleName, 
		"getMaximumFrame", "Get end frame in timeline.", get_maximum_frame);
	pythModules::AddCommand(i_ModuleName, 
		"setTimeRange", "Set time range in timeline.", set_time_range);
	pythModules::AddCommand(i_ModuleName, 
		"setFrameRange", "Set frame range in timeline.", set_frame_range);

	pythModules::AddCommand(i_ModuleName, "updateTime", 
		"Execute all the drivers at current time in order to put the scene in the state at that time.", 
		update_time);

}

#endif