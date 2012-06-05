/*****************************************************************************
**	dirltDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "dirltDocumentInterest.hpp"

#include "dirltDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * dirltDocumentInterest::CreateDocumentChunk()
{
	return new dirltDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void dirltDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}
