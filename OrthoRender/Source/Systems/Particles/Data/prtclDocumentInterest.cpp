/*****************************************************************************
**	prtclDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Particles/Data/prtclDocumentInterest.hpp"

#include "Systems/Particles/GUI/prtclAnimList.hpp"
#include "Systems/Particles/Data/prtclDocumentChunk.hpp"
#include "Systems/Particles/GUI/prtclGeomList.hpp"
#include "Systems/Particles/GUI/prtclTextureList.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * prtclDocumentInterest::CreateDocumentChunk()
{
	return new prtclDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void prtclDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	prtclAnimList::SetAppDirectory( i_Locator );
	prtclGeomList::SetAppDirectory( i_Locator );
	prtclTextureList::SetAppDirectory( i_Locator );

	//OUT prtclDialogUtil::RebuildListDialog();
}

