/*****************************************************************************
**	giDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/


#include "Systems/GlobalIllumination/Data/giDocumentInterest.hpp"

#include "Systems/GlobalIllumination/Data/giDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * giDocumentInterest::CreateDocumentChunk()
{
	return new giDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void giDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

