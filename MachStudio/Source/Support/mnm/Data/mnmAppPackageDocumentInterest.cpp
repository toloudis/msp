/*****************************************************************************
**	mnmAppPackageDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/data/mnmAppPackageDocumentInterest.hpp"

#include "Support/mnm/data/mnmAppPackageDocumentChunk.hpp"


//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * mnmAppPackageDocumentInterest::CreateDocumentChunk()
{
	return new mnmAppPackageDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void mnmAppPackageDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}
