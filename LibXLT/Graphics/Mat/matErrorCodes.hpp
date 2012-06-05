/****************************************************************************\
**  matErrorCodes.hpp
**
**      matErrorCodes.hpp defines the error codes used by the mat package.
**	(see envError.hpp for more about error codes).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_ERRORCODES_HPP
#error matErrorCodes.hpp multiply included
#endif
#define MAT_ERRORCODES_HPP


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace matErrorCodes
{
	enum
	{
		e_General = 1,
		e_UnsupportedPixelFormat,
		e_UnknownImageFileType,
		e_DDSGeneral,
		e_DDSUnsupportedFormat,
		e_InvalidTextureSize
	};
}

