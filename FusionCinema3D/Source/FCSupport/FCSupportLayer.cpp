/*****************************************************************************
**	FCSupportLayer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/FCSupportLayer.hpp"

#include "FCSupport/actn/actnPackage.hpp"
#include "FCSupport/cast/castPackage.hpp"
#include "FCSupport/fcmd/fcmdPackage.hpp"
#include "FCSupport/fcdc/fcdcPackage.hpp"
#include "FCSupport/fcui/fcuiPackage.hpp"
#include "FCSupport/fio/fioPackage.hpp"
#include "FCSupport/loc/locPackage.hpp"
#include "FCSupport/ovrly/ovrlyPackage.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//----------------------------------------------------------------------------
//	Init
//----------------------------------------------------------------------------
void FCSupportLayer::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		actnPackage::Init();
		castPackage::Init();
		fcmdPackage::Init();
		fcuiPackage::Init();
		fioPackage::Init();
		locPackage::Init();
		ovrlyPackage::Init();

		fcdcPackage::Init();

	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp
//----------------------------------------------------------------------------
void FCSupportLayer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
#ifdef WRITECHUNK
		fcdcPackage::CleanUp();
#endif
		ovrlyPackage::CleanUp();
		locPackage::CleanUp();
		fioPackage::CleanUp();
		fcuiPackage::CleanUp();
		fcmdPackage::CleanUp();
		castPackage::CleanUp();
		actnPackage::CleanUp();
	}
}

