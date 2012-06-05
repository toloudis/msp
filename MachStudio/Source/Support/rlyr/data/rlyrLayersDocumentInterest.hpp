/*****************************************************************************
**	rlyrLayersDocumentInterest.hpp
**
**	 Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#ifdef RLYR_LAYERS_DOCUMENTINTEREST_HPP
#error rlyrLayersDocumentInterest.hpp multiply included
#endif
#define RLYR_LAYERS_DOCUMENTINTEREST_HPP

#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif


class rlyrLayersDocumentInterest : public docDocumentInterest
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
