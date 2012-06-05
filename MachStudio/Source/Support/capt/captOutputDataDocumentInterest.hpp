/*****************************************************************************
**	captOutputDataDocumentInterest.hpp
**
**	 Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#ifdef CAPT_OUTPUTDATA_DOCUMENTINTEREST_HPP
#error captOutputDataDocumentInterest.hpp multiply included
#endif
#define CAPT_OUTPUTDATA_DOCUMENTINTEREST_HPP

#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif


class captOutputDataDocumentInterest : public docDocumentInterest
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
