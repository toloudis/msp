/*****************************************************************************
**  ietPlatformWin.cpp
**
**      see ietPlatform.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "ietPlatform.hpp"

#include <crtdbg.h>
#include <stdlib.h>
#include <windows.h>

void ietPlatform::SetUpUnattended()
{
	::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);

	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
}
