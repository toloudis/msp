/*****************************************************************************
**	rptReportMem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Reports/ReportMem/rptReportMem.hpp"

#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"

#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Graphics/g2d/g2dResourceCounter.hpp"

#include <sstream>
#include <iomanip>

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

	theFile.WriteLine("\r\n");

	std::ostringstream stm;
	float tvm = g2dResourceCounter::GetTotalVideoMemory();
	float tx = g2dResourceCounter::GetTotalSceneTextureMemory();
	float fb = g2dResourceCounter::GetTotalFramebufferMemory();
	float sh = g2dResourceCounter::GetTotalShadowMapMemory();
	float vb = g2dResourceCounter::GetTotalVertexBufferMemory();
	float ib = g2dResourceCounter::GetTotalIndexBufferMemory();

	stm << std::setiosflags( std::ios::fixed );
	stm << std::setprecision(3);
	stm << "Total Video Mem : " << tvm << "KB\r\n";
	stm << " Textures       : " << std::setw(4) << g2dResourceCounter::GetNumSceneTextures() << " : " << std::setw(10) << tx << "KB\r\n";
	stm << " Framebuffers   : " << std::setw(4) << g2dResourceCounter::GetNumFramebufferTextures() << " : " << std::setw(10) << fb << "KB\r\n";
	stm << " Shadow Maps    : " << std::setw(4) << g2dResourceCounter::GetNumShadowMapTextures() << " : " << std::setw(10) << sh << "KB\r\n";
	stm << " Vertex Buffers : " << std::setw(4) << g2dResourceCounter::GetNumVertexBuffers() << " : " << std::setw(10) << vb << "KB\r\n";
	stm << " Index Buffers  : " << std::setw(4) << g2dResourceCounter::GetNumIndexBuffers() << " : " << std::setw(10) << ib << "KB\r\n";
	// remainder should be zero!
//	stm << "Remainder (Non-texture buffers) : " << tvm-(tx+fb+sh+vb+ib) << "KB\r\n";
	theFile.WriteLine(stm.str());

	theFile.WriteLine("\r\n");


	rptReportMemMgr::DoReportMem(theFile);

	// close file and write whatever footer is desired:
}

