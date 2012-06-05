/****************************************************************************\
**	mainInitDocument.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainInitDocument.hpp"

#include "MainApp/mainConstants.hpp"
#include "MainApp/Data/mainDocumentInterest.hpp"
#include "MainApp/Data/mainCommentDocumentInterest.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/rstk/rstkDocumentInterest.hpp"


//--------------------------------------------------------------------
// Creates document
//--------------------------------------------------------------------
mainInitDocument::mainInitDocument()
{
	// Add ResourceTracker document interest
	docSingleTypeMgr::AddDocumentInterest(new rstkDocumentInterest());

	// Add the "version" document chunk LAST so it gets written at the end.
	std::string exestr = mainConstants::mc_ExecutableVersion;
	DBG_LOG("Version " << exestr);

	// Use "Prepend" here to make sure that we write these chunks first into the files
	//
	docSingleTypeMgr::PrependDocumentInterest(new mainCommentDocumentInterest());
	docSingleTypeMgr::PrependDocumentInterest(new mainDocumentInterest(exestr));	// FIRST chunk

	// Make NewDocument here after document interests have been registered
	guiSingleDocHandler::New();
}

//--------------------------------------------------------------------
// Deletes document, freeing memory when shutting down
//--------------------------------------------------------------------
mainInitDocument::~mainInitDocument()
{
	// By putting the clean up of the document here
	// it handles multiple ways of exiting, though there
	// might be a way to register this on "Closing()" or 
	// some event like that
	guiSingleDocHandler::Exit();
}
