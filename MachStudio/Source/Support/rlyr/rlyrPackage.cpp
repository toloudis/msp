/*****************************************************************************
**  mtrlPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Support/rlyr/rlyrPackage.hpp"

#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rlyr/data/rlyrLayersDocumentInterest.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"

//============================================================================
//============================================================================
namespace rlyrPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		rlyrRenderLayerMgr::Initialize();
		docSingleTypeMgr::AddDocumentInterest(new rlyrLayersDocumentInterest());
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		rlyrRenderLayerMgr::DeInitialize();
	}
}