/*****************************************************************************
**	mainCommentDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Data/mainCommentDocumentInterest.hpp"

#include "MainApp/Data/mainCommentDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * mainCommentDocumentInterest::CreateDocumentChunk()
{
	return new mainCommentDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void mainCommentDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}
