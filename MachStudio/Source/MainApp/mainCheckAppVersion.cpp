/****************************************************************************\
**	mainCheckAppVersion.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainCheckAppVersion.hpp"
#include "MainApp/mainConstants.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsXMLParser.hpp"

#include <iostream>

//uncomment this to use statically linked libraries of curl (must be defined before the include!)
//#define CURL_STATICLIB

// http://curl.haxx.se/download.html
#include "curl/curl.h"


//==============================================================================
//	library pragmas
//==============================================================================
#ifdef CURL_STATICLIB
#ifdef _DEBUG
	#pragma comment(lib,"libcurl.lib")		//static
#else//!_DEBUG
	#pragma comment(lib,"libcurl.lib")		//static
#endif//_DEBUG

#pragma comment(lib,"ws2_32.lib")		//link with dependent libraries when statically linking
#pragma comment(lib,"wldap32.lib")

#else//!CURL_STATICLIB
#ifdef _DEBUG
	#pragma comment(lib,"libcurld_imp.lib")	//dynamic
#else//!_DEBUG
	#pragma comment(lib,"libcurl_imp.lib")	//dynamic
#endif//_DEBUG
#endif//CURL_STATICLIB

//============================================================================
//
//http://studiogpu/download/version.xml
//
//This should be readable to determine the need for an update.
//If there is an update version higher then the current version, then redirect the person to the downloads page.
//
//I think that is the simplest way to handle this.
//Then everytime there is an update, simply update this file with the version numbers. The filename is really not even essential.
//
//
//
//<?xml version="1.0" encoding="ISO8859-1" ?>
//<machstudiopro>
//   <version>1.0001</version>
//   <filename>machstudio-pro-1.0001</filename>
//</machstudiopro>
//
//============================================================================


namespace
{
	envAppVersion l_UserVersion;
	envAppVersion l_LatestVersion;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	static int first_time=1;
	char outfilename[FILENAME_MAX] = "./body.txt";
	static FILE *outfile;
	size_t written;
	if (first_time) 
	{
		first_time = 0;
		outfile = fopen(outfilename,"w");
		if (outfile == NULL) 
		{
			return -1;
		}
		fprintf(stderr,"The body is <%s>\n", outfilename);
	}
	written = fwrite(buffer, size, nmemb, outfile);
	return written;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
size_t parse_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	char* membuff = new char[(nmemb*size)+1];
	strncpy(membuff, (const char*)buffer, nmemb*size);
	membuff[ (nmemb*size) ] = '\0';

	fsXMLParser xmlp(membuff);
	xmlp.Open();

	//	read in the preferences
	//
	fsXMLData::fs_Node_Type node_type;
	std::string keyname, strvalue;
	while ((node_type = xmlp.ReadNode(keyname,strvalue)) != fsXMLData::e_EOF)
	{
		if ((node_type == fsXMLData::e_Text) && (keyname.length() > 0))
		{
			if (keyname == "version")
			{
				l_LatestVersion.SetFromString( strvalue );
			}
		}
	}

	xmlp.Close();

	delete membuff;
	return nmemb*size;
}
}



//----------------------------------------------------------------------------
//	IsLatestVersion - returns true if app is up to date. If not, it also
//	returns the version and the file to download.
//----------------------------------------------------------------------------
bool mainCheckAppVersion::IsLatestVersion(const envAppVersion& i_UserVersion, const std::string& i_VersionURL, envAppVersion& o_LatestVersion, itString& o_DownloadFile)
{
	l_UserVersion = i_UserVersion;

	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) 
	{
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_data);
		curl_easy_setopt(curl, CURLOPT_URL, i_VersionURL.c_str());
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);

		o_LatestVersion = l_LatestVersion;	//	set the parameter

		//	now check the version
		if (l_UserVersion  < l_LatestVersion)
		{
			DBG_WARNING("There is a newer version to download" << l_LatestVersion.GetString().c_str() );
			return false;
		}
		else if (l_UserVersion == l_LatestVersion)
		{
			DBG_LOG("Your version of the application is up to date");
		}
		else
		{
			// (user_version > latest_version)
			//	Note: This is possible when the user gets a pre-release version of MSPro.
			//
			DBG_TRACE("User version of application is newer than the latest" << l_UserVersion.GetString().c_str() << " vs " << l_LatestVersion.GetString().c_str());
			DBG_LOG("There is not a newer version.");	// for users
		}
	}

	return true;
}

//{
//	const int BUFSIZE = 64;
//	char	  m_version[BUFSIZ];				 // version number as text
//	CString m_sServer(L"www.studiogpu.com");						 // server name (www.mysite.com)
//	DWORD	  m_dwStatus;						 // status code
//	CString m_sStatus;						 // status text
//	DWORD dwMajorVersion;		// version number: most-sig 32 bits
//	DWORD dwMinorVersion;		// version number: least-sig 32 bits
//
//	LPCTSTR lpFileName(L"download/version.xml");
//	//assert(lpFileName);
//	BOOL bRet = FALSE;		// assume failure
//	m_dwStatus=0;
//	m_sStatus.Empty();
//
//	// Read version file using MFC Inet classes.
//	INTERNET_PORT nPort = INTERNET_DEFAULT_HTTP_PORT;
//	CInternetSession session(_T("SGPUSession"));
//	CHttpConnection* pConn = NULL;
//	CHttpFile* pFile = NULL;
//	try 
//	{
//		pConn = session.GetHttpConnection(m_sServer, nPort);
//		pFile = pConn->OpenRequest( CHttpConnection::HTTP_VERB_GET, lpFileName );
//		pFile->SendRequest();
//		pFile->QueryInfoStatusCode(m_dwStatus);
//		if (m_dwStatus==HTTP_STATUS_OK) 
//		{
//			const UINT BUFSIZE = 128;
//			UINT nRead = pFile->Read(m_version, BUFSIZE);
//			if (nRead>0) 
//			{
//				// read version number in the form Mhi,Mlo,mhi,mlo
//				m_version[nRead] = 0;
//				int Mhi,Mlo,mhi,mlo;
//				_stscanf(m_version, (_T(«%x,%x,%x,%x»)), 
//				&Mhi, &Mlo, &mhi, &mlo);
//				dwMajorVersion = MAKELONG(Mlo,Mhi);
//				dwMinorVersion = MAKELONG(mlo,mhi);
//				bRet = TRUE;                // success!
//			}
//		} 
//		else 
//		{
//			pFile->QueryInfo(HTTP_QUERY_STATUS_TEXT, m_sStatus);
//		}
//	} 
//	catch (CInternetException* pEx) 
//	{
//	}
//	session.Close();
//
//	delete pFile;
//	delete pConn;
//
//	return bRet;
//}
