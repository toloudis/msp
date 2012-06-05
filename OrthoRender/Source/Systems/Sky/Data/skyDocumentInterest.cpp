/*****************************************************************************
**	skyDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "skyDocumentInterest.hpp"

#include "skyDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * skyDocumentInterest::CreateDocumentChunk()
{
	return new skyDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void skyDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

