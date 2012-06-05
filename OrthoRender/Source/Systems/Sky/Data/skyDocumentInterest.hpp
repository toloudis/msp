/*****************************************************************************
**	skyDocumentInterest.hpp
**
**	Callback to create document chunk type
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef SKY_DOCUMENTINTEREST_HPP
#error skyDocumentInterest.hpp multiply included
#endif
#define SKY_DOCUMENTINTEREST_HPP

#ifndef DOC_DOCUMENTINTEREST_HPP
#include "docDocumentInterest.hpp"
#endif


class skyDocumentInterest : public docDocumentInterest
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
