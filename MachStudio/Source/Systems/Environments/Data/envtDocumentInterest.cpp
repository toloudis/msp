/*****************************************************************************
**	envtDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Environments/Data/envtDocumentInterest.hpp"

#include "Systems/Environments/Data/envtDocumentChunk.hpp"
#include "Systems/Environments/GUI/envtTextureList.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * envtDocumentInterest::CreateDocumentChunk()
{
	return new envtDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void envtDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
	envtTextureList::SetAppDirectory( i_Locator );
}

