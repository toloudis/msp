/*****************************************************************************
**	trfnDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Transforms/Data/trfnDocumentInterest.hpp"

#include "Systems/Transforms/Data/trfnDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * trfnDocumentInterest::CreateDocumentChunk()
{
	return new trfnDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void trfnDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

