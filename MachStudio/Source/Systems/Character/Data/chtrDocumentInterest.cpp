/*****************************************************************************
**	chtrDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/Data/chtrDocumentInterest.hpp"

#include "Systems/Character/Data/chtrDocumentChunk.hpp"

#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"

namespace
{
	// Function pointer to use to mark current character data chunk as dirty 
	// when material information changes. This function is safe to call even
	// when no character data chunk exists.
	void MaterialDataChanged()
	{
		chtrDocumentChunk::ActiveDataChanged();
	}
}

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * chtrDocumentInterest::CreateDocumentChunk()
{
	docDocumentChunk* chtrChunk = new chtrDocumentChunk();

	// Can't point the mtrlOperations at the chunk itself, because chunks are 
	// made and deleted in the import process. We need to route it to a safe function
	// that can then mark whatever chunk is currently active as dirty.
	//mtrlOperations::SetChunkImplementation(chtrChunk);
	mtrlOperations::SetChunkImplementation(&MaterialDataChanged);

	return chtrChunk;
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
