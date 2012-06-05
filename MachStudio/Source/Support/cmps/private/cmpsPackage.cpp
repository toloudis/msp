/*****************************************************************************
**  cmpsPackage.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/cmpsPackage.hpp"

#include "Support/cmps/cmpsSelectMgr.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
//#include "Support/cmps/cmpsPickInterest.hpp"

//	library

//	tool
//#include "Tool/pick3d/pick3dMgr.hpp"


namespace
{
	//cmpsPickInterest* l_pCmpsPI = 0;
}


//====================================================================
//	Init
//====================================================================
void cmpsPackage::Init()
{
	cmpsCompassMgr::Initialize();
	cmpsSelectMgr::Initialize();

	//	register a pick interest
	//l_pCmpsPI = new cmpsPickInterest();
	//pick3dMgr::RegisterPickInterest( l_pCmpsPI );
}

//====================================================================
//	CleanUp
//====================================================================
void cmpsPackage::CleanUp()
{
	cmpsSelectMgr::DeInitialize();
	cmpsCompassMgr::DeInitialize();

	//	unregister a pick interest
	//pick3dMgr::UnRegisterPickInterest( l_pCmpsPI );
}


