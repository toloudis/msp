/****************************************************************************\
**	g3dConditionalCompile.hpp
**
**	A file to contain #defines used to conditionally compile in-progress code
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

//============================================================================
//============================================================================
//#define ENABLE_MOTIONBLUR
//define if you want the use OpenGL for output display (rendering is still D3D)
//#define OPENGL_DISPLAY
//define this to enable 10 Bit component output, only if supported in OPENGL.
//#define USE_10BIT

//define this to support the new hair system
#define HAIR_SUPPORTED

//adjusts projected lights texture alignment by 1/2 of the shadow map dimensions
#define SHADOW_ALIGNMENT

//#define ENABLE_GI_VOLUME

//transitions to the new way of doing MSAA
#define NEW_AA_CODE

//#define USE_VSM_SHADOWS
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define USE_OPENEXR
