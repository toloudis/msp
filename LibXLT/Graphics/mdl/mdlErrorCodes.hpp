/****************************************************************************\
**	mdlErrorCodes.hpp
**
**		mdlErrorCodes.hpp defines the error codes used by the env package.
**	(see envError.hpp for more about error codes).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_ERRORCODES_HPP
#error mdlErrorCodes.hpp multiply included
#endif
#define MDL_ERRORCODES_HPP


//============================================================================
//============================================================================
namespace mdlErrorCodes
{
	enum
	{
		e_InvalidModelFile = 1		// base exception class - you should never see this code in a real error
	};
}
