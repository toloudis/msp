/****************************************************************************\
**  g2dErrorCodes.hpp
**
**      g2dErrorCodes.hpp defines the error codes used by the g2d package.
**	(see envError.hpp for more about error codes).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_ERRORCODES_HPP
#error g2dErrorCodes.hpp multiply included
#endif
#define G2D_ERRORCODES_HPP


//============================================================================
//============================================================================
namespace g2dErrorCodes
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	enum
	{
		e_UnsupportedMode = 1,
		e_ScreenInit,
		e_UnknownFont,
		e_UnsupportedPixelFormat,
		e_General,
		e_UnknownImageFileType,
		e_OutOfVideoMemory,
		e_OutOfSystemMemory,
		e_RLEBufferOverrun, // during TGA capture of D3D surface
	};
}

