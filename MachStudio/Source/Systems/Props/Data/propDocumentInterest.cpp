/*****************************************************************************
**	propDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Props/Data/propDocumentInterest.hpp"

#include "Systems/Props/Data/propDocumentChunk.hpp"

#include "Systems/Props/GUI/propAnimList.hpp"
#include "Systems/Props/GUI/propGeomList.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * propDocumentInterest::CreateDocumentChunk()
{
	return new propDocumentChunk();
}

//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void propDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	propAnimList::SetAppDirectory( i_Locator );
	propGeomList::SetAppDirectory( i_Locator );

//OUT 	propDialogUtil::RebuildListDialog();
}
