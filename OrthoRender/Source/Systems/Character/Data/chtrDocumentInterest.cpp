/*****************************************************************************
**	chtrDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/Data/chtrDocumentInterest.hpp"

#include "Systems/Character/Data/chtrDocumentChunk.hpp"

#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * chtrDocumentInterest::CreateDocumentChunk()
{
	return new chtrDocumentChunk();
}

//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void chtrDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	chtrAnimList::SetAppDirectory( i_Locator );
	chtrGeomList::SetAppDirectory( i_Locator );

//OUT	chtrDialogUtil::RebuildListDialog();
}
