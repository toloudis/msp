/********************************************************************************************\
**	mnmAppPackageData.hpp
**
**		Data structure for the AppPackage chunk
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef MNM_APPPACKAGEDATA_HPP
#error mnmAppPackageData.hpp multiply included
#endif
#define MNM_APPPACKAGEDATA_HPP

#ifndef ENV_APPVERSION_HPP
#include "Core/env/envAppVersion.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//	Which application did this data come from
//
//	TODO - do we want to add machine, user and other information?
//============================================================================
struct mnmAppPackageData
{
	itString		m_SGPUAppName;
	envAppVersion	m_SGPUAppVersion;
};

