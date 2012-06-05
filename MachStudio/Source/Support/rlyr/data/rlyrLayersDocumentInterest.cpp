/*****************************************************************************
**	rlyrLayersDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Support/rlyr/data/rlyrLayersDocumentInterest.hpp"

#include "Support/rlyr/data/rlyrLayersDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * rlyrLayersDocumentInterest::CreateDocumentChunk()
{
	return new rlyrLayersDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void rlyrLayersDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}