/*****************************************************************************
**	aoDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/AmbientOcclusion/Data/aoDocumentInterest.hpp"

#include "Systems/AmbientOcclusion/Data/aoDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * aoDocumentInterest::CreateDocumentChunk()
{
	return new aoDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void aoDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

