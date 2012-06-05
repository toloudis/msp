/*****************************************************************************
**  rmpPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/rmpPackage.hpp"

#include "Support/rmp/rmpDialogMgr.hpp"
#include "Support/rmp/rmpDialogUtil.hpp"
#include "Support/rmp/rmpTextureMgr.hpp"

namespace
{
	int l_RefCount = 0;
}


//--------------------------------------------------------------------
// Init 
//--------------------------------------------------------------------
// static
void rmpPackage::Init()
{
	if ( 0 == l_RefCount )
	{
		// initialize packages we depend on
		//

		// initialize our internal stuff
		rmpTextureMgr::Initialize();
		rmpDialogMgr::Initialize();
		rmpDialogUtil::Initialize();
	}
	++l_RefCount;
}

//--------------------------------------------------------------------
// CleanUp 
//--------------------------------------------------------------------
// static
void rmpPackage::CleanUp()
{
	--l_RefCount;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( 0 == l_RefCount )
	{
		rmpDialogMgr::CleanUp();
		rmpTextureMgr::DeInitialize();
		rmpDialogUtil::DeInitialize();
	}
}

