/*****************************************************************************
**	docDocumentInterest.hpp
**
**	 Callback to create document chunk type needed by
**	interested system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_DOCUMENTINTEREST_HPP
#error docDocumentInterest.hpp multiply included
#endif
#define DOC_DOCUMENTINTEREST_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//forward declarations
//============================================================================
class docDocumentChunk;


//============================================================================
//============================================================================
class docDocumentInterest
{
public:
	virtual ~docDocumentInterest() {}

	//--------------------------------------------------------------------
	//  virtual function to create document chunk to be held in
	// document.
	//--------------------------------------------------------------------
	virtual docDocumentChunk* CreateDocumentChunk() = 0;

	//--------------------------------------------------------------------
	//	send the "root" directory for this application to each
	//	doc interest.
	//--------------------------------------------------------------------
	virtual void SetAppDirectory( const fsLocator& i_Locator ) {}
};

//============================================================================
//============================================================================
template <class DocumentChunk>
class docSimpleDocumentInterest : public docDocumentInterest
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	docSimpleDocumentInterest() { }

	//--------------------------------------------------------------------
	//  template function just creates new chunk of our type
	//--------------------------------------------------------------------
	docDocumentChunk * CreateDocumentChunk()
	{
		return new DocumentChunk();
	}
};
