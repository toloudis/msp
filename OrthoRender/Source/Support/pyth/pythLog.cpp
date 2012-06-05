/****************************************************************************\
**	pythLog.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythLog.hpp"

#include "Support/pyth/pythPython.hpp"

// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)
namespace
{
	pythLog::LogFunction l_pLogOutputFunc = NULL;
	pythLog::LogFunction l_pLogErrorFunc = NULL;

	//============================================================================
	// Capturing print and err messages from python
	//============================================================================
	PyObject* log_CaptureStdout(PyObject* self, PyObject* pArgs)
	{
		char* LogStr = NULL;
		if (!PyArg_ParseTuple(pArgs, "s", &LogStr)) return NULL;

		if (l_pLogOutputFunc != NULL)
			(*l_pLogOutputFunc)(LogStr);

		Py_INCREF(Py_None);
		return Py_None;
	}

	// Notice we have STDERR too.
	PyObject* log_CaptureStderr(PyObject* self, PyObject* pArgs)
	{
		char* LogStr = NULL;
		if (!PyArg_ParseTuple(pArgs, "s", &LogStr)) return NULL;

		if (l_pLogErrorFunc != NULL)
			(*l_pLogErrorFunc)(LogStr);

		Py_INCREF(Py_None);
		return Py_None;
	}

	static PyMethodDef logMethods[] = {
		{"CaptureStdout", log_CaptureStdout, METH_VARARGS, "Logs stdout"},
		{"CaptureStderr", log_CaptureStderr, METH_VARARGS, "Logs stderr"},
		{NULL, NULL, 0, NULL}
	};

} // end of anonymous namespace

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythLog::Initialize(LogFunction i_pLogOutputFunc,
						 LogFunction i_pLogErrorFunc)
{
	l_pLogOutputFunc = i_pLogOutputFunc;
	l_pLogErrorFunc = i_pLogErrorFunc;

	if (l_pLogOutputFunc != NULL || l_pLogErrorFunc != NULL)
	{
		pythThreadLock thread_lock;

		// Add in the "log" module for capturing standard out and standard error messages
		Py_InitModule("log", logMethods);
		PyRun_SimpleString(
			"import log\n"
			"import sys\n"
			"class StdoutCatcher:\n"
			"\tdef write(self, str):\n"
			"\t\tlog.CaptureStdout(str)\n"
			"class StderrCatcher:\n"
			"\tdef write(self, str):\n"
			"\t\tlog.CaptureStderr(str)\n"
			"sys.stdout = StdoutCatcher()\n"
			"sys.stderr = StderrCatcher()\n"
			);
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pythLog::DeInitialize()
{
	l_pLogOutputFunc = NULL;
	l_pLogErrorFunc = NULL;
}
#endif

