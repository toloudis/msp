/*****************************************************************************
**	billDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Billboard/Data/billDocumentInterest.hpp"

#include "Systems/Billboard/Data/billDocumentChunk.hpp"
#include "Systems/Billboard/GUI/billGeomList.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * billDocumentInterest::CreateDocumentChunk()
{
	return new billDocumentChunk();
}

//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void billDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	billGeomList::SetAppDirectory( i_Locator );

//OUT	billDialogUtil::RebuildListDialog();
}
