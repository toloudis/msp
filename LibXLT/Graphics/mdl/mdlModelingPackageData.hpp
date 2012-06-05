/*****************************************************************************
**	mdlModelingPackageData.hpp
**
**		structure holding information about a modeling package
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MODELINGPACKAGE_HPP
#error mdlModelingPackageData.hpp multiply included
#endif
#define MDL_MODELINGPACKAGE_HPP

#ifndef ENV_APPVERSION_HPP
#include "Core/env/envAppVersion.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
struct mdlModelingPackageData
{
	envType::Int32	m_CreationDate;			// Julian date
	envAppVersion	m_ExporterVersion;
	envAppVersion	m_SDKVersion;
	itString		m_ModelingPackageName;
	itString		m_ModelingPackageVersion;
};

