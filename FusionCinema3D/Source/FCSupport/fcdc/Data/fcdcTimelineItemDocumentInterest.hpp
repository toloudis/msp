/*****************************************************************************
**	fcdcDocumentInterest.hpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCDC_DOCUMENTINTEREST_HPP
#error fcdcTimelineItemDocumentInterest.hpp multiply included
#endif
#define FCDC_DOCUMENTINTEREST_HPP

#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif

#include <string.h>


//============================================================================
//============================================================================
class fcdcTimelineItemDocumentInterest : public docDocumentInterest
{
public:
	//--------------------------------------------------------------------
	//  virtual function to create document chunk to be held in
	// document
	//--------------------------------------------------------------------
	docDocumentChunk * CreateDocumentChunk();

	//--------------------------------------------------------------------
	//	send the "root" directory for this application to each
	//	doc interest.
	//--------------------------------------------------------------------
	void SetAppDirectory( const fsLocator& i_Locator );
};
