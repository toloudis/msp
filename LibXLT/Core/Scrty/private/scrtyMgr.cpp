/*****************************************************************************
**	scrtyMgr.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Core/scrty/scrtyMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <assert.h>
//#include <iomanip>
//#include <algorithm>
#include <objbase.h>		// for BSTR


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtyMgr::Init()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Init();
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtyMgr::CleanUp()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->CleanUp();
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
long scrtyMgr::ValidateLicense(std::string& i_LicenseStr)
{
	if (sm_pImplementation)
	{
		//	convert the std string to BSTR
		std::wstring wstr( i_LicenseStr.length()+1, 0 );
		MultiByteToWideChar( ::GetACP(),
							0,
							i_LicenseStr.c_str(),
							i_LicenseStr.length(),
							&wstr[0],
							i_LicenseStr.length() );
		BSTR b_str = SysAllocString( wstr.c_str() );

		//	validate the license
		return sm_pImplementation->ValidateLicense( b_str );

		SysFreeString( b_str );
	}

	return -1;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtyMgr::CheckSecurity1()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{ 
		return sm_pImplementation->CheckSecurity1();
	}
	return true;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtyMgr::CheckSecurity2()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{ 
		return sm_pImplementation->CheckSecurity2();
	}
	return true;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtyMgr::CheckSecurity3()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{ 
		return sm_pImplementation->CheckSecurity3();
	}
	return true;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtyMgr::CheckSecurity4()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{ 
		return sm_pImplementation->CheckSecurity4();
	}
	return true;
}

//------------------------------------------------------------------------
//	check if the security is valid
//------------------------------------------------------------------------
bool scrtyMgr::CheckSecurity5()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{ 
		return sm_pImplementation->CheckSecurity5();
	}
	return true;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//bool scrtyMgr::ReadSecurity(int o_Data[])
//{
//	DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
//	if (sm_pImplementation)
//	{
//		return sm_pImplementation->ReadSecurity(o_Data);
//	}
//	return true;
//}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//bool scrtyMgr::WriteSecurity(int i_Data[])
//{
//	DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
//	if (sm_pImplementation)
//	{
//		return sm_pImplementation->WriteSecurity(i_Data);
//	}
//	return true;
//}

//------------------------------------------------------------------------
//	Call ONCE on launch of program
//------------------------------------------------------------------------
bool scrtyMgr::OpenSecurity()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->OpenSecurity();
	}
	else
	{
		// crash out in an ugly way -- since this should never happen
		//	unless someone is hacking the executable
		//
		assert("");
	}
	return false;
}

//------------------------------------------------------------------------
//	Call ONCE on exit of program
//------------------------------------------------------------------------
void scrtyMgr::CloseSecurity()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->CloseSecurity();
	}
	else
	{
		// crash out in an ugly way -- since this should never happen
		//	unless someone is hacking the executable
		//
		assert("");
	}
}

//------------------------------------------------------------------------
//	for testing only
//------------------------------------------------------------------------
void scrtyMgr::TestSecurity()
{
	//DBG_ASSERT(sm_pImplementation, "scrtyMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->TestSecurity();
	}
}

