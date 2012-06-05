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
#define MSPRO_FVERSION		1,0,0,55
#define MSPRO_PVERSION		1,0,0,55
#define MSPRO_EXEVERSION	"1.0.0.55"
//----------------------------------------------------------------------------
//	MachStudio Essential
//----------------------------------------------------------------------------
#define MSESS_FVERSION		1,5,0,0
#define MSESS_PVERSION		1,5,0,0
#define MSESS_EXEVERSION	"1.5.0.0"


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
#elif (SGPU_APP == MS_ESSENTIAL)
	#define FVERSION	MSESS_FVERSION;
	#define PVERSION	MSESS_PVERSION;
	#define EVERSION	MSESS_EXEVERSION;
#else 
	#error "Unsupported application -- no version numbers set"
#endif
}

