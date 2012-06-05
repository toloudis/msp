/*****************************************************************************
**	mnmConstants.hpp
**
**		general application constants
**
**	StudioGPU
**	Copyright(C) 2003-10 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_CONSTANTS_HPP
#error mnmConstants.hpp multiply included
#endif
#define MNM_CONSTANTS_HPP

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 


//============================================================================
//	Defines
//============================================================================
//#define DEMO_VERSION 1	//	works for any application

//
// possible applications
//
#define		MS_PRO			1
#define		MS_CORE			2
#define		MS_FUSION		3

//
//	define the specific application
//
#ifndef FUSION
#define SGPU_APP	MS_PRO
//#define SGPU_APP	MS_CORE
#else
#define SGPU_APP	MS_FUSION
#endif

//
//	modeling package to restrict to.  Don't define MS_MODEL_PACKAGE for all
//
//#define MODELINGPACKAGE_RHINO		1


//============================================================================
//============================================================================
namespace mnmConstants
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//const char* GetProductName()
	//{
	//	return "MachStudio Pro";
	//}

	//const wchar_t* GetProductDisplayName()
	//{
	//#ifdef DEMO_VERSION
	//	return L"MachStudio™ Pro Trial Version";
	//#else
	//	return L"MachStudio™ Pro";
	//#endif
	//}

//	const char* GetProductName();
//	const char* GetProductDisplayName();
//	const char* GetCopyright();
//	const char* GetExecutableName();

	//--------------------------------------------------------------------
	//	product constants
	//--------------------------------------------------------------------
	//
	//	Constants based on the application
	//
#if (SGPU_APP == MS_PRO)
	static const char* c_COPYRIGHT	= "Copyright© studio|gpu, Inc. 2003-10.";
	static const wchar_t* cw_COPYRIGHT	= L"Copyright© studio|gpu, Inc. 2003-10.";
	static const char* c_PRODUCT	= "MachStudio Pro";
	static const wchar_t* cw_PRODUCT	= L"MachStudio Pro";

	//	demo or not
	#ifdef DEMO_VERSION
		static const wchar_t* c_PRODUCT_FOR_DISPLAY = L"MachStudio™ Pro Trial Version";
	#else
		static const wchar_t* c_PRODUCT_FOR_DISPLAY	= L"MachStudio™ Pro";
	#endif

	//	executable
	//
	//#if defined(WIN32)
		static const char* c_EXECUTABLE	= "MachStudio.exe";
	//#else
	//	static const char* c_EXECUTABLE	= "MachStudio.exe"; // "MachStudio-x64.exe";
	//#endif

#elif (SGPU_APP == MS_CORE)
	static const char* c_COPYRIGHT	= "Copyright© studio|gpu, Inc. 2009-2010.";

	#if(MS_MODELING_PACKAGE & MODELINGPACKAGE_RHINO)
		static const char* c_PRODUCT	= "MachStudio Core for Rhino";
		//	demo or not
		#ifdef DEMO_VERSION
			static const wchar_t* c_PRODUCT_FOR_DISPLAY = L"MachStudio™ Core for Rhino Trial Version";
		#else
			static const wchar_t* c_PRODUCT_FOR_DISPLAY	= L"MachStudio™ Core for Rhino";
		#endif
	#else
		static const char* c_PRODUCT	= "MachStudio Core";
		//	demo or not
		#ifdef DEMO_VERSION
			static const wchar_t* c_PRODUCT_FOR_DISPLAY = L"MachStudio™ Core Trial Version";
		#else
			static const wchar_t* c_PRODUCT_FOR_DISPLAY	= L"MachStudio™ Core";
		#endif
	#endif

	//	executable name
	//
	#if(MS_MODELING_PACKAGE & MODELINGPACKAGE_RHINO)
		//#if defined(WIN32)
			static const char* c_EXECUTABLE	= "MachStudioCoreRhino.exe";
		//#else
		//	static const char* c_EXECUTABLE	= "MachStudioCoreRhino.exe"; // "MachStudioCoreRhino-x64.exe";
		//#endif
	#else
		//#if defined(WIN32)
			static const char* c_EXECUTABLE	= "MachStudioCore.exe";
		//#else
		//	static const char* c_EXECUTABLE	= "MachStudioCore.exe"; // "MachStudioCore-x64.exe";
		//#endif
	#endif	// modeling_package

#elif (SGPU_APP == MS_FUSION)
	static const wchar_t* cw_COPYRIGHT	= L"Copyright© studio|gpu, Inc. 2010.";
	static const char* c_COPYRIGHT	= "Copyright© studio|gpu, Inc. 2010.";

	static const char* c_PRODUCT	= "Fusion Cinema3d";
	static const wchar_t* cw_PRODUCT	= L"Fusion Cinema3d";
	static const wchar_t* c_PRODUCT_FOR_DISPLAY	= L"Fusion Cinema3d™";
	static const char* c_EXECUTABLE	= "FusionCinema3d.exe";

#else
	#error SGPU_APP is not defined for a specific application!
#endif	// if SGPU_APP == ...

	//--------------------------------------------------------------------
	//	generic constants
	//--------------------------------------------------------------------
	static const char* c_COMPANY	= "StudioGPU";
	static const char* c_TRADEMARK  = "™";
	static const wchar_t* cw_RIGHTS  = L"All rights reserved.";
	static const wchar_t* cw_64BIT  = L"64-bit";
	static const wchar_t* cw_32BIT  = L"32-bit";

	//--------------------------------------------------------------------
	//	Status Panels
	//--------------------------------------------------------------------
	enum eStatusBarPanels
	{
		e_SBPanel_Mode = 0,
		e_SBPanel_Messages,
		e_SBPanel_Key,
		e_SBPanel_Mute,
		//e_SBPanel_Particles,
		e_SBPanel_Camera,
		e_SBPanel_Render_Flags,
		e_SBPanel_Subdiv,
		e_SBPanel_Memory,
		e_SBPanel_RenderThread,
		e_SBPanel_NumberOfPanels
	};

	//--------------------------------------------------------------------
	// Font being used in 3d windows
	//--------------------------------------------------------------------
	static const char* c_ViewerFontName = "font-lucd00.png";
}
