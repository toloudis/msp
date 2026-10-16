//****************************************************************************
//  dbgSystemInfoPACWin.cpp
//
//      see .hpp
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#include <string.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>
//#include <intrin.h>
#include <windows.h>

#include "dbgSystemInfoPACWin.hpp"
#include "Core/Env/envInitX.hpp"
#include "Core/Env/private/envSystemDataPACWin.hpp"
#include "Core/Dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace dbgSystemInfoPAC
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::wstring getUserName()
	{
		TCHAR userID[256]; 
		DWORD _buff = 256;
		GetUserName(LPWSTR(&userID), &_buff);
		return std::wstring(userID);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::wstring getComputerName()
	{
		TCHAR compID[256]; 
		DWORD _buff = 256;
		GetComputerName(LPWSTR(&compID), &_buff) ;
		return  std::wstring(compID);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int getMemory()
	{
		MEMORYSTATUS _memStatus;
		GlobalMemoryStatus(&_memStatus);
		return  _memStatus.dwTotalVirtual/(1024*1024);  //returned in MB
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::wstring getOS()
	{
		envSystemDataPAC::OSVersion _osInfo;
		if ( !envSystemDataPAC::GetOSVersion(_osInfo) )
			return L"Windows (unknown version)";

		// Windows 11 still reports itself as 10.0; it starts at build 22000.
		std::wstring _osVersion;
		if ( !_osInfo.m_bWorkstation )
			_osVersion = L"Windows Server";
		else if ( _osInfo.m_Major == 10 && _osInfo.m_Build >= 22000 )
			_osVersion = L"Windows 11";
		else
			_osVersion = L"Windows " + std::to_wstring(_osInfo.m_Major);

		_osVersion += L" (" + std::to_wstring(_osInfo.m_Major) + L"." +
			std::to_wstring(_osInfo.m_Minor) + L"." + std::to_wstring(_osInfo.m_Build) + L")";
		return  _osVersion;
	}
}