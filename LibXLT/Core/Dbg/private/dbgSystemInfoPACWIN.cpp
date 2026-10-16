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
		std::wstring _osVersion;
		envSystemDataPAC::OSVersion _osInfo;
		if ( !envSystemDataPAC::GetOSVersion(_osInfo) )
			return L"Windows (unknown version)";

		switch(_osInfo.m_Major)
		{
			case 6:
				if( _osInfo.m_Minor == 1 )
				{
					if(	_osInfo.m_bWorkstation)
					{
						_osVersion = L"Windows 7, ";
						_osVersion += _osInfo.m_ServicePack;
					}
					else
					{
						_osVersion = L"Windows Server 2008 R2, ";
						_osVersion += _osInfo.m_ServicePack;
					}
				}
				else if( _osInfo.m_Minor == 0 )
				{
					if(	_osInfo.m_bWorkstation)
					{
						_osVersion = L"Windows Vista, ";
						_osVersion += _osInfo.m_ServicePack;
					}
					else
					{
						_osVersion = L"Windows Server 2008, ";
						_osVersion += _osInfo.m_ServicePack;
					}
				}
				break;
			case 5:
				if( _osInfo.m_Minor == 2 )
				{
					_osVersion = L"Windows Server 2003, ";
					_osVersion += _osInfo.m_ServicePack;
				}
				if( _osInfo.m_Minor == 1 )
				{
					_osVersion = L"Windows XP, ";
					_osVersion += _osInfo.m_ServicePack;
				}
				if( _osInfo.m_Minor == 0 )
				{
					_osVersion = L"Windows 2000, ";
					_osVersion += _osInfo.m_ServicePack;
				}
				break;
			default:
				_osVersion = L"Windows " + std::to_wstring(_osInfo.m_Major) + L"." +
					std::to_wstring(_osInfo.m_Minor) + L"." + std::to_wstring(_osInfo.m_Build);
				break;
		}
		return  _osVersion;
	}
}