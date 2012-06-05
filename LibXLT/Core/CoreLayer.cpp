/*****************************************************************************
**  CoreLayer.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/CoreLayer.hpp"

#include "Core/app/appPackage.hpp"
#include "Core/ch/chPackage.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsPackage.hpp"
#include "Core/geo/geoPackage.hpp"
#include "Core/gf/gfPackage.hpp"
#include "Core/it/itPackage.hpp"
#include "Core/ma/maPackage.hpp"
#include "Core/prty/prtyPackage.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//----------------------------------------------------------------------------
//	Init 
//----------------------------------------------------------------------------
void CoreLayer::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages in the layer
		dbgPackage::Init();
		envPackage::Init();
		maPackage::Init();
		geoPackage::Init();
		fsPackage::Init();
		itPackage::Init();
		appPackage::Init();
		gfPackage::Init();
		chPackage::Init();
		prtyPackage::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp 
//----------------------------------------------------------------------------
void CoreLayer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		prtyPackage::CleanUp();
		chPackage::CleanUp();
		gfPackage::CleanUp();
		appPackage::CleanUp();
		itPackage::CleanUp();
		fsPackage::CleanUp();
		geoPackage::CleanUp();
		maPackage::CleanUp();
		envPackage::CleanUp();
		dbgPackage::CleanUp();
	}
}
