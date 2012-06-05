/****************************************************************************\
**  lodExceptionX.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "lodExceptionX.hpp"


//---------------------------------------------------------------------------=
//	The lodModeExitX exception object can be thrown by modes which wish to
//	terminate the mode operation, without going back to any other modes that
//	may be on the stack.  lodModeExitX must be thrown during the mode Think.
//---------------------------------------------------------------------------=
lodModeExitX::lodModeExitX()
{
}

//---------------------------------------------------------------------------=
//---------------------------------------------------------------------------=
lodModeExitX::~lodModeExitX()
{
}

//---------------------------------------------------------------------------=
//	Index() returns the error code/string index.
//---------------------------------------------------------------------------=
envError::Code lodModeExitX::Index() const
{
	return envError::GetCode(1000, 0);
}

