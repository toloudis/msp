/*****************************************************************************
**	sbrdDocumentInterest.hpp
**
**	 Callback to create document chunk type
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/


#ifdef SBRD_DOCUMENTINTEREST_HPP
#error sbrdDocumentInterest.hpp multiply included
#endif
#define SBRD_DOCUMENTINTEREST_HPP


#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif


class sbrdDocumentInterest : public docDocumentInterest
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
