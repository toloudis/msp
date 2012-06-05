/*****************************************************************************
**	docDocumentWithChunks.hpp
**
**	The docDocumentWithChunks class holds the chunks of information
**	that represents a file
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_DOCUMENTWITHCHUNKS_HPP
#error docDocumentWithChunks.hpp multiply included
#endif
#define DOC_DOCUMENTWITHCHUNKS_HPP

#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class chReader;
class chWriter;
class docDocumentChunk;
class fsResourceTrackerData;


//============================================================================
//============================================================================
class docDocumentWithChunks : public docDocument
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	docDocumentWithChunks(int i_WriteFormat = docDocument::eDocBinary);
	docDocumentWithChunks(const std::vector<docDocumentChunk*> &i_DocChunks, int i_WriteFormat = docDocument::eDocBinary);

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~docDocumentWithChunks();

	//--------------------------------------------------------------------
	// Clear
	//--------------------------------------------------------------------
	virtual void Clear();

	//--------------------------------------------------------------------
	// Load
	//
	//	returns true if the file loaded is a newer version than the
	//	user's executable supports.  The file is still loaded as best
	//	it can be.
	//--------------------------------------------------------------------
	virtual bool Load(const fsLocator &i_Locator, LoadFlags i_LoadMethod = e_New_Doc );

	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	virtual void Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags = true );

	//--------------------------------------------------------------------
	// IsLoading - return true if this document is currently loading
	//--------------------------------------------------------------------
	virtual bool IsLoading();

	//--------------------------------------------------------------------
	// IsDirty
	//--------------------------------------------------------------------
	virtual bool IsDirty();

	//--------------------------------------------------------------------
	// SetActive
	//--------------------------------------------------------------------
	virtual void SetActive();

	//--------------------------------------------------------------------
	// SetInactive - set the chunks for this document inactive
	//
	//	if i_bUpdateData is true then the app will ask the appropriate
	//	"manager" to give the chunk the current data.  This shouldn't be
	//	done only if you need to load another document while the current
	//	one is active.
	//--------------------------------------------------------------------
	virtual void SetInactive( bool i_bUpdateData = true );

	//--------------------------------------------------------------------
	// SetWriteFormat - set the format for output
	//--------------------------------------------------------------------
	virtual void SetWriteFormat( int i_WriteFormat = docDocument::eDocBinary );

	//--------------------------------------------------------------------
	// Filename
	//--------------------------------------------------------------------
	virtual const fsLocator& GetFilename() const;
	virtual void SetFilename(const fsLocator &i_Filename);

	//--------------------------------------------------------------------
	// AddDocumentChunk - this document assumes ownership
	//--------------------------------------------------------------------
	void AddDocumentChunk(docDocumentChunk* i_pChunk);

	//--------------------------------------------------------------------
	// RemoveDocumentChunk - remove this chunk from the document
	//--------------------------------------------------------------------
	void RemoveDocumentChunk(docDocumentChunk* i_pChunk);

	//--------------------------------------------------------------------
	//	GetDocumentChunk() - get a document chunk
	//--------------------------------------------------------------------
	docDocumentChunk* GetDocumentChunk( const int i_Index );

	//--------------------------------------------------------------------
	//	GetNumDocumentChunks() - get the number of document chunks
	//--------------------------------------------------------------------
	int GetNumDocumentChunks();

	//--------------------------------------------------------------------
	//  Get a list of resources as unique full paths with no hierarchy.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& o_ResourceTrackerData );

private:
	//--------------------------------------------------------------------
	// Load
	//--------------------------------------------------------------------
	virtual void LoadFile(chReader& i_Reader, LoadFlags i_LoadMethod = e_New_Doc );

	//--------------------------------------------------------------------
	//	Load as binary format
	//--------------------------------------------------------------------
	virtual bool LoadFile_Binary( const fsLocator &i_Locator, LoadFlags i_LoadMethod = e_New_Doc );

	//--------------------------------------------------------------------
	//	Load as XML format
	//--------------------------------------------------------------------
	virtual bool LoadFile_XML( const fsLocator &i_Locator, LoadFlags i_LoadMethod = e_New_Doc );

	//--------------------------------------------------------------------
	//	Load as ASCII format
	//--------------------------------------------------------------------
	virtual bool LoadFile_ASCII( const fsLocator &i_Locator, LoadFlags i_LoadMethod = e_New_Doc );

	//--------------------------------------------------------------------
	// Save
	//--------------------------------------------------------------------
	virtual void SaveFile(chWriter& i_Writer, bool i_bResetDirtyFlags = true );

	//--------------------------------------------------------------------
	//	Save as binary format
	//--------------------------------------------------------------------
	virtual void SaveFile_Binary( const fsLocator &i_Locator, bool i_bResetDirtyFlags );

	//--------------------------------------------------------------------
	//	Save as XML format
	//--------------------------------------------------------------------
	virtual void SaveFile_XML( const fsLocator &i_Locator, bool i_bResetDirtyFlags );

	//--------------------------------------------------------------------
	//	Save as ASCII format
	//--------------------------------------------------------------------
	virtual void SaveFile_ASCII( const fsLocator &i_Locator, bool i_bResetDirtyFlags );

private:
	int			m_WriteFormat;
	fsLocator	m_Filename;
	bool		m_bIsLoading;

	std::vector<docDocumentChunk*> m_DocChunks;
};

