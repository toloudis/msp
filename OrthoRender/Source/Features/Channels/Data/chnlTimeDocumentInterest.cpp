/*****************************************************************************
**	chnlTimeDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Data/chnlTimeDocumentInterest.hpp"

#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * chnlTimeDocumentInterest::CreateDocumentChunk()
{
	return new chnlTimeDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void chnlTimeDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

