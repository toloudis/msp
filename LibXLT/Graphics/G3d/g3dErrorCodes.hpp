/****************************************************************************\
**	g3dErrorCodes.hpp
**
**		g3dErrorCodes.hpp defines the error codes used by the g3d package.
**	(see envError.hpp for more about error codes).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_ERRORCODES_HPP
#error g3dErrorCodes.hpp multiply included
#endif
#define G3D_ERRORCODES_HPP


//============================================================================
//============================================================================
namespace g3dErrorCodes
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	enum
	{
		e_General = 1,
		e_No3DHardware,
		e_InvalidNodeName,
		e_BadShader,
	};
}

