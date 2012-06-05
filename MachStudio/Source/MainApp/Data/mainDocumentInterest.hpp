/*****************************************************************************
**	mainDocumentInterest.hpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_DOCUMENTINTEREST_HPP
#error mainDocumentInterest.hpp multiply included
#endif
#define MAIN_DOCUMENTINTEREST_HPP

#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif

#include <string.h>


//============================================================================
//============================================================================
class mainDocumentInterest : public docDocumentInterest
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mainDocumentInterest(const std::string& i_ExecutableVersion);

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
	std::string m_ExecutableVersion;
};
