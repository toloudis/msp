/*****************************************************************************
**	rpnDocumentInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "MainApp/stdafx.h"
#include "Features/RenderPanels/rpnDocumentInterest.hpp"

#include "Features/RenderPanels/rpnDocumentChunk.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
rpnDocumentInterest::rpnDocumentInterest()
:	docDocumentInterest()
{
}

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * rpnDocumentInterest::CreateDocumentChunk()
{
	return new rpnDocumentChunk();
}


//--------------------------------------------------------------------
//	send the "root" directory for this application to each
//	doc interest.
//--------------------------------------------------------------------
void rpnDocumentInterest::SetAppDirectory( const fsLocator& i_Locator )
{
}
