/****************************************************************************\
**	mainVersionNumber.hpp
**
**		This file contains the version numbers for MachStudio applications
**
**		The format is:  Major.Minor.Revision.Build
**
**		The build is auto-incremented by a build server.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_VERSIONNUMBER_HPP
#error mainVersionNumber.hpp multiply included
#endif
#define MAIN_VERSIONNUMBER_HPP


//============================================================================
//	Application Versions
//
//	FVERSION - file version
//	PVERSION - product version
//	EXEVERSION - executable version (text string)
//============================================================================

//----------------------------------------------------------------------------
//	MachStudio Pro
//----------------------------------------------------------------------------
#define MSPRO_FVERSION		2,0,0,192
#define MSPRO_PVERSION		2,0,0,192
#define MSPRO_EXEVERSION	"2.0.0.192"


//----------------------------------------------------------------------------
//	MachStudio Core
//----------------------------------------------------------------------------
#define MSCORE_FVERSION		2,0,0,0
#define MSCORE_PVERSION		2,0,0,0
#define MSCORE_EXEVERSION	"2.0.0.0"


//============================================================================
//	Use the above version numbers to set the appropriate
//	generic defines that are used elsewhere in the code
//============================================================================
namespace versionNumbers
{
#if (SGPU_APP == MS_PRO)
	#define FVERSION	MSPRO_FVERSION
	#define PVERSION	MSPRO_PVERSION
	#define EXEVERSION	MSPRO_EXEVERSION
#elif (SGPU_APP == MS_CORE)
	#define FVERSION	MSCORE_FVERSION
	#define PVERSION	MSCORE_PVERSION
	#define EVERSION	MSCORE_EXEVERSION
#else 
	#error "Unsupported application -- no version numbers set"
#endif
}

