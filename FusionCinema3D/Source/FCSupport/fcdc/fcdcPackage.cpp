/****************************************************************************\
**	fcdcPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcdc/fcdcPackage.hpp"

#include "FCSupport/fcdc/fcdcDataMgr.hpp"
#include "FCSupport/fcdc/data/fcdcTimelineItemDocumentInterest.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"


//============================================================================
//============================================================================
namespace fcdcPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		docSingleTypeMgr::AddDocumentInterest(new fcdcTimelineItemDocumentInterest());
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		
	}
}	

