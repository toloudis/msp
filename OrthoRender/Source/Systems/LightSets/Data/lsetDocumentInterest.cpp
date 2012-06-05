/*****************************************************************************
**	lsetDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/LightSets/Data/lsetDocumentInterest.hpp"

#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * lsetDocumentInterest::CreateDocumentChunk()
{
	return new lsetDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void lsetDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

