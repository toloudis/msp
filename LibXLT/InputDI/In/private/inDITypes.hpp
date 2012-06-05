/****************************************************************************\
**  inDITypes.hpp
**
**      Defines some DI typedefs to hide the specific
**	DirectInput version being used.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_DITYPES_HPP
#error inDITypes.hpp multiply included
#endif
#define IN_DITYPES_HPP


// I'm putting all DI includes from all public header files into
// typedefs so that we can more easily switch the D3D version

#include <dinput.h>


typedef LPDIRECTINPUTDEVICE8		inDIDevicePtr;
typedef LPDIRECTINPUT8W				inDIPtr;

//typedef IID_IDirectInput8W			g2dIDirectInput;
#define g2dIDirectInput				IID_IDirectInput8W;

//static const char* c_g2dD3DLIBRARYINPUT	= "dinput8.lib";
#define	c_g2dD3DLIBRARYINPUT		"dinput8.lib"

//
//	PORT NOTES
//
//	see g2dDX11Types.h for a complete list
//

