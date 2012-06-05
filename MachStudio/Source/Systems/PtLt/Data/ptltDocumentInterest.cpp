/*****************************************************************************
**	ptltDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/PtLt/Data/ptltDocumentInterest.hpp"

#include "Systems/PtLt/Data/ptltDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * ptltDocumentInterest::CreateDocumentChunk()
{
	return new ptltDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void ptltDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}
