/*****************************************************************************
**	captOutputDataDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Support/capt/captOutputDataDocumentInterest.hpp"

#include "Support/capt/captOutputDataDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * captOutputDataDocumentInterest::CreateDocumentChunk()
{
	return new captOutputDataDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void captOutputDataDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}