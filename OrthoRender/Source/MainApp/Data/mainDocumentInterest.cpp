/*****************************************************************************
**	mainDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Data/mainDocumentInterest.hpp"

#include "MainApp/Data/mainDocumentChunk.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mainDocumentInterest::mainDocumentInterest(std::string& i_ExecutableVersion)
:	docDocumentInterest(),
	m_ExecutableVersion(i_ExecutableVersion)
{
}

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * mainDocumentInterest::CreateDocumentChunk()
{
	return new mainDocumentChunk(m_ExecutableVersion);
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void mainDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}
