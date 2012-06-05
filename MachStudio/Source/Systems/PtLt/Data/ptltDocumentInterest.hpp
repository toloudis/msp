/*****************************************************************************
**	ptltDocumentInterest.hpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef PTLT_DOCUMENTINTEREST_HPP
#error ptltDocumentInterest.hpp multiply included
#endif
#define PTLT_DOCUMENTINTEREST_HPP

#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif


class ptltDocumentInterest : public docDocumentInterest
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
