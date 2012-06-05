/*****************************************************************************
**	grupDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Groups/Data/grupDocumentInterest.hpp"

#include "Systems/Groups/Data/grupDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * grupDocumentInterest::CreateDocumentChunk()
{
	return new grupDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void grupDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

