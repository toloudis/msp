/*****************************************************************************
**	prjltDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Data/prjltDocumentInterest.hpp"

#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"
#include "Systems/PrjLt/Data/prjltDocumentChunk.hpp"
#include "Systems/PrjLt/GUI/prjltTextureList.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * prjltDocumentInterest::CreateDocumentChunk()
{
	return new prjltDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void prjltDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	prjltTextureList::SetAppDirectory( i_Locator );
}
