/*****************************************************************************
**	fcdcDocumentInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcdc/data/fcdcTimelineItemDocumentInterest.hpp"
#include "FCSupport/fcdc/data/fcdcTimelineItemDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * fcdcTimelineItemDocumentInterest::CreateDocumentChunk()
{
	return new fcdcTimelineItemDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void fcdcTimelineItemDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}
