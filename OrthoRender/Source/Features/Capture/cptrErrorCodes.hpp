/****************************************************************************\
**  cptrErrorCodes.hpp
**
**      cptrErrorCodes.hpp defines the error codes used by the cptr package.
**	(see envError.hpp for more about error codes).
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_ERRORCODES_HPP
#error cptrErrorCodes.hpp multiply included
#endif
#define CPTR_ERRORCODES_HPP


//============================================================================
//============================================================================
namespace cptrErrorCodes
{
	enum
	{
		e_WriteBufferOverrun = 1,
		e_Unknown
	};
}

