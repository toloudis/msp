/*****************************************************************************
**	cmraDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Cameras/Data/cmraDocumentInterest.hpp"

#include "Systems/Cameras/GUI/cmraAnimList.hpp"
#include "Systems/Cameras/Data/cmraDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * cmraDocumentInterest::CreateDocumentChunk()
{
	return new cmraDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void cmraDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	cmraAnimList::SetAppDirectory( i_Locator );
}

