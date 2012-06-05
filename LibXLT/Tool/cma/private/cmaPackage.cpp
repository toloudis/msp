/*****************************************************************************
**	cmaPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/cma/cmaPackage.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//--------------------------------------------------------------------
// Init -- i_Parser, the user defined level parser.  NULL indicates
//			to the package that the default parsing system be used.
//--------------------------------------------------------------------
// static
//void cmaPackage::Init(cmaPackageParser* i_Parser/*=NULL*/)
void cmaPackage::Init()
{
	if ( 0 == l_RefCount )
	{
		// initialize packages we depend on
		//
		// chPackage initializes all our other dependencies for us
		//lvlPackage::Init();  // used by cmaParserUtil

		// initialize our internal stuff
		//
		//if ( i_Parser )
		//{
		//	lvlParserMgr::AddParser(i_Parser);
		//}
		//else
		//{
		//	lvlParserMgr::AddParser(new cmaPackageParser);
		//}

		//	set the error handler file for this package
		//
		//gfErrorHandler::SetErrorFilename( envPackageErrorIndices::e_cma,itString("cmaErrors.tsf") );

	}
	++l_RefCount;
}

//--------------------------------------------------------------------
// CleanUp -- cleans up all relevent package data.  It also destroys
// all waypoint lists that may have been filled when the data was read
// from the level file and/or added by the user.
// Something the user may do manually if desired.
//--------------------------------------------------------------------
// static
void cmaPackage::CleanUp()
{
	--l_RefCount;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( 0 == l_RefCount )
	{
		//delete cmaCommandManagementInfo::l_pCmdMgr;

		// clean up packages we depend on, in reverse order
		//
		//lvlPackage::CleanUp();
	}
}

