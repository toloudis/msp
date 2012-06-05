/*****************************************************************************
**	docDocumentChunk.cpp
**
**	Base class for chunks of a document, each chunk is
**	handled by its own system
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Tool/doc/docDocumentChunk.hpp"


//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
docDocumentChunk::~docDocumentChunk()
{
}


//--------------------------------------------------------------------
//  Import chunk data and add it to existing data
//--------------------------------------------------------------------
void  docDocumentChunk::Import(	chReader &i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								bool i_bRenameDupes )
{
	this->Read(i_Reader, i_Version, i_Size);
}

