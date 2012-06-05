/****************************************************************************\
**	mnmAppPackageMgr.hpp
**
**		mnmAppPackageMgr provides an interface to the Application Package data
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_APPPACKAGEMGR_HPP
#error mnmAppPackageMgr.hpp multiply included
#endif
#define MNM_APPPACKAGEMGR_HPP


//============================================================================
//============================================================================
class envAppVersion;
class itString;
struct mnmAppPackageData;


//============================================================================
//	Store application and scene data
//
//	ToDo - set up ability to allow multiple packages to be allowed or by version
//============================================================================
namespace mnmAppPackageMgr
{
	//------------------------------------------------------------------------
	//	If true only package name matches will be allowed
	//------------------------------------------------------------------------
	void SetRestricted(bool i_bRestrict);
	bool IsRestricted();

	//------------------------------------------------------------------------
	//	Get/Set Application Data
	//		This is the running application's info (name, version, etc)
	//------------------------------------------------------------------------
	void SetAppData( mnmAppPackageData& i_Data );
	mnmAppPackageData& GetAppData();

	//------------------------------------------------------------------------
	//	Get/Set Scene Data
	//		This is the loaded scene's info (what app: name, version, etc)
	//------------------------------------------------------------------------
	void SetSceneData( mnmAppPackageData& i_Data );
	mnmAppPackageData& GetSceneData();

	//------------------------------------------------------------------------
	//	Set the application specific data
	//------------------------------------------------------------------------
	void SetAppData( itString i_AppName, envAppVersion& i_AppVersion );
};

