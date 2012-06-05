/*****************************************************************************
**  mainCheckAppVersion.hpp
**
**     Check to see if the user has the most recent version of the app
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_CHECKAPPVERSION_HPP
#error mainCheckAppVersion.hpp multiply included
#endif
#define MAIN_CHECKAPPVERSION_HPP

#ifndef ENV_APPVERSION_HPP
#include "Core/env/envAppVersion.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#include <string>


//============================================================================
//	mainCheckAppVersion
//============================================================================
namespace mainCheckAppVersion
{
	//----------------------------------------------------------------------------
	//	IsOnline - returns true if the computer is online
	//----------------------------------------------------------------------------
	bool IsOnline();

	//----------------------------------------------------------------------------
	//	SetInternetFile - set the server and file to download.
	//
	//	i_Server - http://www.studiogpu.com
	//	i_DownloadFile - download/version.xml
	//----------------------------------------------------------------------------
	//bool SetInternetFile(itString& i_Server, itString& i_DownloadFile);

	//----------------------------------------------------------------------------
	//	IsLatestVersion - returns true if app is up to date. If not, it also
	//	returns the version and the file to download.
	//----------------------------------------------------------------------------
	bool IsLatestVersion(const envAppVersion& i_UserVersion, const std::string& i_VersionURL, envAppVersion& o_LatestVersion, itString& o_DownloadFile);
};

