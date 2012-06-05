/*****************************************************************************
**	rpnDocumentInterest.hpp
**
**	Callback to create document chunk type
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_DOCUMENTINTEREST_HPP
#error rpnDocumentInterest.hpp multiply included
#endif
#define RPN_DOCUMENTINTEREST_HPP

#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif

#include <string.h>


//============================================================================
//============================================================================
class rpnDocumentInterest : public docDocumentInterest
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rpnDocumentInterest();

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

private:
};
