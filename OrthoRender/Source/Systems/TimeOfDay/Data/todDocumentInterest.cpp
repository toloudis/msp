/*****************************************************************************
**	todDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "todDocumentInterest.hpp"

#include "todDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * todDocumentInterest::CreateDocumentChunk()
{
	return new todDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void todDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

