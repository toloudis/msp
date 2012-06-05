/*****************************************************************************
**  g3dShadowVShaderConstantsWin.hpp
**
**	This component defines the structure of the shader constants used 
**	by the application to interface with the shadow vertex shader which
**	extrudes the shadow volumes.
**	The reason #defines are used (instead of enum) is that the file is
**	also included by nvasm.exe, which is not a C++ compiler but does have a
**	C-style preprocessor.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#define CV_ZERO				0
#define CV_ONE				1
#define CV_OSPACELIGHTPOS	2
#define CV_WVP0				3
#define CV_WVP1				4
#define CV_WVP2				5
#define CV_WVP3				6
#define CV_FATNESS_SCALE	7

