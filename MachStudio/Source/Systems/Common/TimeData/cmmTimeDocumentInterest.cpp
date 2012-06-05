/*****************************************************************************
**	cmmTimeDocumentInterest.cpp
**
**	Callback to create document chunk type
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Common/TimeData/cmmTimeDocumentInterest.hpp"

#include "Systems/Common/TimeData/cmmTimeDocumentChunk.hpp"

//--------------------------------------------------------------------
//  virtual function to create document chunk to be held in
// document
//--------------------------------------------------------------------
docDocumentChunk * cmmTimeDocumentInterest::CreateDocumentChunk()
{
	return new cmmTimeDocumentChunk();
}

