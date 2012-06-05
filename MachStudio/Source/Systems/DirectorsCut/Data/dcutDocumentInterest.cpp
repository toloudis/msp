/*****************************************************************************
**	dcutDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/


#include "Systems/DirectorsCut/Data/dcutDocumentInterest.hpp"

#include "Systems/DirectorsCut/Data/dcutDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * dcutDocumentInterest::CreateDocumentChunk()
{
	return new dcutDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void dcutDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

