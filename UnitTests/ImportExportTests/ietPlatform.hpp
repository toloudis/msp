/*****************************************************************************
**  ietPlatform.hpp
**
**      Platform setup for running the unit tests unattended.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

namespace ietPlatform
{
	//	SetUpUnattended makes runtime errors (CRT asserts, abort(),
	//	crashes) print to stderr and fail the process instead of opening
	//	a dialog, so a CI run can't hang waiting for someone to click OK.
	void SetUpUnattended();
}
