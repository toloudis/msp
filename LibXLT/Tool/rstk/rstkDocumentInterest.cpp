/*****************************************************************************
**	rstkDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Tool/rstk/rstkDocumentInterest.hpp"

#include "Tool/rstk/rstkDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * rstkDocumentInterest::CreateDocumentChunk()
{
	return new rstkDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void rstkDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

