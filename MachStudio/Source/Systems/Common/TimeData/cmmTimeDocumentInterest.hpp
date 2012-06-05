/*****************************************************************************
**	cmmTimeDocumentInterest.hpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef CMM_TIMEDOCUMENTINTEREST_HPP
#error cmmTimeDocumentInterest.hpp multiply included
#endif
#define CMM_TIMEDOCUMENTINTEREST_HPP


#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif


class cmmTimeDocumentInterest : public docDocumentInterest
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
	virtual void SetAppDirectory( const fsLocator& i_Locator ) {};
};
