/*****************************************************************************
**	rptReportMem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Reports/ReportMem/rptReportMem.hpp"

#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"

#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/gf/gfFileTxt.hpp"

//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
rptReportMem::rptReportMem()
{
}

//--------------------------------------------------------------------
//	Destructor
//--------------------------------------------------------------------
rptReportMem::~rptReportMem()
{
}

//--------------------------------------------------------------------
//	Generate() - generate the report
//--------------------------------------------------------------------
//static
void rptReportMem::Generate()
{
	//fsLocator dir = gfPaths::GetPath( mnmPaths::e_SaveShots );
	docDocument* doc = docSingleDocumentMgr::GetDocument();
	fsLocator sceneFile = docSingleDocumentMgr::GetFilename();
	if (sceneFile.GetNumNames() == 0)
		return;
	
	// filepath should come from the current document.
	// we will automatically drop this doc in the same folder as the 
	// scene file.

	std::string filename;
	fsFileUtil::LocatorToANSIFilename(sceneFile, filename);

	itString fname = sceneFile.GetLastName();
	fname.StripExtension();
	fname += itString(".mem");

	sceneFile.Pop();
	sceneFile.Push(fname);

	// open file and write whatever header is desired:
	// gfFileTxt or std::ostream??
	if (fsFileUtil::FileExists(sceneFile))
		fsFileUtil::DeleteFile(sceneFile);
	fsFileUtil::CreateFile(sceneFile);
	gfFileTxt theFile(sceneFile, fsFileStream::e_WriteOnly);
	//std::ostream theFile(filename);

	theFile.WriteLine("Memory Usage Report\r\n");
	theFile.WriteLine("===================\r\n");


	rptReportMemMgr::DoReportMem(theFile);

	// close file and write whatever footer is desired:
}

