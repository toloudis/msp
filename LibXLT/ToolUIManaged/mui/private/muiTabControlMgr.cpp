/*****************************************************************************
**  muiTabControlMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "muiTabControlMgr.hpp"

#include "tmaTabControlMgr.hpp"


//===========================================================================
//	muiTabControlMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void muiTabControlMgr::Initialize()
{
	////todo assert if the init and deinit counts don't match
	//if ( tmaTabControlMgr::g_pMgr == 0 )
	//{
	//	tmaTabControlMgr::g_pMgr = new tmaTabControlMgr();
	//	tmaTabControlMgr::g_pMgr->Initialize();
	//}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void muiTabControlMgr::DeInitialize()
{
	//if ( tmaTabControlMgr::g_pMgr != 0 )
	//{
	//	tmaTabControlMgr::g_pMgr->DeInitialize();
	//	delete tmaTabControlMgr::g_pMgr;
	//	tmaTabControlMgr::g_pMgr = 0;
	//}
}
