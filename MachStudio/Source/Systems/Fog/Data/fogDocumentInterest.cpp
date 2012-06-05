/*****************************************************************************
**	fogDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Fog/Data/fogDocumentInterest.hpp"

#include "Systems/Fog/Data/fogDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * fogDocumentInterest::CreateDocumentChunk()
{
	return new fogDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void fogDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

