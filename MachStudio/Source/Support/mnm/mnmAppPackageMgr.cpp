/****************************************************************************\
**	mnmAppPackageMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmAppPackageMgr.hpp"

#include "Support/mnm/data/mnmAppPackageData.hpp"


//============================================================================
//============================================================================
namespace
{
	bool				l_bRestrict = false;
	mnmAppPackageData	l_AppData;
	mnmAppPackageData	l_SceneData;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void mnmAppPackageMgr::SetRestricted(bool i_bRestrict)
{
	l_bRestrict = i_bRestrict;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool mnmAppPackageMgr::IsRestricted()
{
	return l_bRestrict;
}

//------------------------------------------------------------------------
//	Get/Set Application Data
//		This is the running application's info (name, version, etc)
//------------------------------------------------------------------------
void mnmAppPackageMgr::SetAppData( mnmAppPackageData& i_Data )
{
	l_AppData = i_Data;
}

mnmAppPackageData& mnmAppPackageMgr::GetAppData()
{
	return l_AppData;
}

//------------------------------------------------------------------------
//	Get/Set Scene Data
//		This is the loaded scene's info (what app: name, version, etc)
//------------------------------------------------------------------------
void mnmAppPackageMgr::SetSceneData( mnmAppPackageData& i_Data )
{
	l_SceneData = i_Data;
}

mnmAppPackageData& mnmAppPackageMgr::GetSceneData()
{
	return l_SceneData;
}

//------------------------------------------------------------------------
//	Set the application specific data
//------------------------------------------------------------------------
void mnmAppPackageMgr::SetAppData( itString i_AppName, envAppVersion& i_AppVersion )
{
	l_AppData.m_SGPUAppName = i_AppName;
	l_AppData.m_SGPUAppVersion = i_AppVersion;
}

