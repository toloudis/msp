/****************************************************************************\
**  sgpuExportLib.hpp
**
**      Sets up basic defines for sgpuExportLib
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_EXPORTLIB_HPP
#define SGPU_EXPORTLIB_HPP

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SGPUEXPORTLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SGPUEXPORTLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
// KG: If we build sgpuexport static libraries,
//     then, we need to expect all the users (including extgernal users) 
//	   of this header file
//     to define 'SGPU_EXPORT_STATIC_LIB'.
#ifndef SGPU_EXPORT_STATIC_LIB
#ifdef SGPUEXPORTLIB_EXPORTS
#define SGPUEXPORTLIB_API __declspec(dllexport)
#else
#define SGPUEXPORTLIB_API __declspec(dllimport)
#endif
#else
#define SGPUEXPORTLIB_API
#endif


#define SGPU_EPSILON_EQUAL_PRECISION 1.0e-5f
#define SGPU_ERROR_MATERIAL_NAME "SgpuErrorMaterial"
#define SGPU_SUPPORT_1200

#include <cassert>
#endif // #ifndef SGPU_EXPORTLIB_HPP
