/*****************************************************************************
**	lyrsDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Layers/Data/lyrsDocumentInterest.hpp"

#include "Systems/Layers/Data/lyrsDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * lyrsDocumentInterest::CreateDocumentChunk()
{
	return new lyrsDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void lyrsDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

