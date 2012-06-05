/*****************************************************************************
**	setsDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/Sets/Data/setsDocumentInterest.hpp"

#include "Systems/Sets/GUI/setsDialogUtil.hpp"
#include "Systems/Sets/Data/setsDocumentChunk.hpp"
#include "Systems/Sets/GUI/setsGeomList.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * setsDocumentInterest::CreateDocumentChunk()
{
	return new setsDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void setsDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	setsGeomList::SetAppDirectory( i_Locator );

	setsDialogUtil::RebuildListDialog();
}

