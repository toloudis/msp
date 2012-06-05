/*****************************************************************************
**	SceneSetupDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/SceneSetup/Data/SceneSetupDocumentInterest.hpp"

#include "Features/SceneSetup/Data/SceneSetupDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * SceneSetupDocumentInterest::CreateDocumentChunk()
{
	return new SceneSetupDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void SceneSetupDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}

