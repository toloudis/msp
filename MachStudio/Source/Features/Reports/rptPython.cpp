#include "Features/Reports/rptPython.hpp"

#include "Features/Reports/ReportMem/rptReportMem.hpp"
#include "Support/pyth/pythModules.hpp"
#if defined(PYTHON_ENABLED)
namespace
{

	PyObject *
	log_video_stats(PyObject *self, PyObject *args)
	{
		rptReportMem::Generate();
		return Py_None;
	}
}

void rptPython::AddCommands(const std::string &i_ModuleName)
{

	pythModules::AddCommand(i_ModuleName, "logVideoStats", 
		"Write the Video statistics to a log file ", 
		log_video_stats);
}
#endif