/*****************************************************************************
**  fbxSdk.hpp
**
**      Use this header to include the FBX SDK correctly for our settings.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_SDK_HPP
#error fbxSdk.hpp multiply included
#endif
#define FBX_SDK_HPP

//bga - This construction of defines wasn't working, so I just put the compiler define
// into the configurations "Release" and "Debug Iterator"

//#ifdef _DEBUG
//	//Only use FBX importing if we are using Debug Iterator configuration
//	#if _HAS_ITERATOR_DEBUGGING
//	#define USE_FBX_IMPORTEXPORT
//	#endif
//#else
//	// Release version can use FBX
//	#define USE_FBX_IMPORTEXPORT
//#endif	// #ifdef _DEBUG

#ifdef USE_FBX_IMPORTEXPORT

#define KFBX_PLUGIN
#define KFBX_FBXSDK
#define KFBX_NODLL

#include <fbxsdk.h>
 
#endif // USE_FBX_IMPORTEXPORT
