/*****************************************************************************
**	docSingleTypeMgr.hpp
**
**	 SingleTypeMgr handles the details of the single type
**	used by a single type/single document application
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_SINGLETYPEMGR_HPP
#error docSingleTypeMgr.hpp multiply included
#endif
#define DOC_SINGLETYPEMGR_HPP

#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
// forward declarations
//============================================================================
class docDocumentInterest;


//============================================================================
//============================================================================
namespace docSingleTypeMgr
{
	//--------------------------------------------------------------------
	// Gets MRU list
	//--------------------------------------------------------------------
	void Init( int i_MRUMaxEntries = 10 );

	//--------------------------------------------------------------------
	// Removes MRU list
	//--------------------------------------------------------------------
	void Close();

	//--------------------------------------------------------------------
	//  Set filter for filenames based on the extension and a
	// string description of the file type.  The extension should
	// include the period.
	//--------------------------------------------------------------------
	void  SetFilter(const std::string &i_Ext, const std::string &i_Description);

	//--------------------------------------------------------------------
	//  Get filter in format for Open and Save FileDialogs
	//--------------------------------------------------------------------
	std::string  GetFilter();

	//--------------------------------------------------------------------
	//  Add interest in the document type.
	// AddDocumentInterest - adds interest in order,
	// PrependDocumentInterest - puts document interest first in list
	//--------------------------------------------------------------------
	void  AddDocumentInterest(docDocumentInterest *i_Interest);
	void  PrependDocumentInterest(docDocumentInterest *i_Interest);

	//--------------------------------------------------------------------
	//  Create document holding instances of document chunks for
	// each interest registered
	//--------------------------------------------------------------------
	docDocument * CreateDocument(int i_WriteFormat = docDocument::eDocBinary);

	//--------------------------------------------------------------------
	//  Add the given filename to the MRU list
	//--------------------------------------------------------------------
	void  AddToMRU(const fsLocator &i_Locator);

	//--------------------------------------------------------------------
	//  Get vector of MRU filenames
	//--------------------------------------------------------------------
	void  GetMRUList(std::vector<fsLocator> &o_Locators);

	//--------------------------------------------------------------------
	//	send the "root" directory for this application to each
	//	doc interest.
	//--------------------------------------------------------------------
	void SetAppDirectory( const fsLocator& i_Locator );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetMRUFile(const fsLocator& i_File);

}	// end of namespace
