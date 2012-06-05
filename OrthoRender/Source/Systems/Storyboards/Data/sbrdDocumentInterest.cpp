/*****************************************************************************
**	sbrdDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Storyboards/Data/sbrdDocumentInterest.hpp"

#include "Systems/Storyboards/Data/sbrdDocumentChunk.hpp"
#include "Systems/Storyboards/GUI/sbrdGeomList.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * sbrdDocumentInterest::CreateDocumentChunk()
{
	return new sbrdDocumentChunk();
}

//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void sbrdDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	sbrdGeomList::SetAppDirectory( i_Locator );

//OUT	sbrdDialogUtil::RebuildListDialog();
}
