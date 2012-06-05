/*****************************************************************************
**	mnmPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmPackage.hpp"

#include "Support/mnm/data/mnmAppPackageDocumentInterest.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"


//============================================================================
//============================================================================
namespace mnmPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		docSingleTypeMgr::AddDocumentInterest(new mnmAppPackageDocumentInterest());
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}
}
