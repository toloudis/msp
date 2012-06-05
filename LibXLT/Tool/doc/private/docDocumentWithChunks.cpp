/*****************************************************************************
**	docDocumentWithChunks.cpp
**
**		The docDocumentWithChunks class holds the chunks of information
**	that represents a file
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/doc/docDocumentWithChunks.hpp"

#include "Core/Ch/chBinReader.hpp"
#include "Core/Ch/chBinWriter.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/Ch/chExceptionX.hpp"
#include "Core/Ch/chTxtReader.hpp"
#include "Core/Ch/chTxtWriter.hpp"
#include "Core/Ch/chXMLReader.hpp"
#include "Core/Ch/chXMLWriter.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
//#include "Core/Fs/fsResourceTracker.hpp"	// DEBUG ONLY
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Gf/gfFileTxt.hpp"
#include "Core/Gf/gfFileXML.hpp"
#include "Tool/doc/docDocumentChunk.hpp"


//============================================================================
//============================================================================
namespace
{
	//============================================================================
	//============================================================================
	const chDefs::Name c_SGPU = chDefs::MakeName('S', 'G', 'P', 'U');	// root tag

	
	// structure for exception safety to make sure the
	// boolean flag l_bIsSaving is restored to false when exception is thrown
	struct safe_flag_set
	{
		safe_flag_set(bool &i_bFlag) : m_bFlag(i_bFlag)
		{
			m_bFlag = true;
		}
		~safe_flag_set()
		{
			m_bFlag = false;
		}
		bool &m_bFlag;
	};
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
docDocumentWithChunks::docDocumentWithChunks(int i_WriteFormat)
:	m_WriteFormat(i_WriteFormat), m_bIsLoading(false)
{
}

docDocumentWithChunks::docDocumentWithChunks(const std::vector<docDocumentChunk*> &i_DocChunks, int i_WriteFormat)
:	m_WriteFormat(i_WriteFormat), m_bIsLoading(false)
{
	for (int i=0; i<i_DocChunks.size(); i++)
	{
		m_DocChunks.push_back(i_DocChunks[i]);
	}
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
docDocumentWithChunks::~docDocumentWithChunks()
{
	Clear();	// needed?

	envSTLHelpers::DeleteContainer(m_DocChunks);
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  docDocumentWithChunks::Clear()
{
	m_Filename.Clear();

	//	reverse the document chunks so they clear LIFO
	//
	//for (int i=0; i<m_DocChunks.size(); i++)
	for (int i = m_DocChunks.size() - 1; i >= 0; --i)
	{
		m_DocChunks[i]->Clear();
	}
}

//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
bool docDocumentWithChunks::Load(const fsLocator &i_Locator, LoadFlags i_LoadMethod)
{
	bool bNewerVersion = false;

	//	handle reading from a binary file and writing to ASCII or the other way around.
	//
	itString extension;
	i_Locator.GetLastName().GetExtension( extension );

	//bga - These extensions shouldn't be here. This class docDocumentWithChunks needs to
	// handle many types of files, not just MachStudio scene files. The extensions need to be
	// something that is configurable by the application.

	// Exception safe structure sets flag to true and then restores it to false on destructor
	{
		safe_flag_set load_flag(m_bIsLoading);
		//m_bIsLoading = true;
		if (extension == itString("mab"))
		{
			bNewerVersion = LoadFile_Binary( i_Locator, i_LoadMethod );
		}
		else if (extension == itString("xml"))
		{
			bNewerVersion = LoadFile_XML( i_Locator, i_LoadMethod );
		}
		else if (extension == itString("maa"))
		{
			bNewerVersion = LoadFile_ASCII( i_Locator, i_LoadMethod );
		}	
		//m_bIsLoading = false;
	}

	// Notify all chunks that the load finished successfully
	std::for_each(m_DocChunks.begin(), m_DocChunks.end(), std::mem_fun(&docDocumentChunk::NotifyLoadFinished));

	return bNewerVersion;
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
void  docDocumentWithChunks::Save( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
//#define DOC_NOSAVING
#ifndef DOC_NOSAVING

	//
	switch (m_WriteFormat)
	{
		case docDocument::eDocBinary:
		{
			SaveFile_Binary( i_Locator, i_bResetDirtyFlags );
			break;
		}
		case docDocument::eDocXML:
		{
			SaveFile_XML( i_Locator, i_bResetDirtyFlags );
			break;
		}
		case docDocument::eDocASCII:
		{
			SaveFile_ASCII( i_Locator, i_bResetDirtyFlags );
			break;
		}
		case docDocument::eDocAll:
		{
			SaveFile_Binary( i_Locator, i_bResetDirtyFlags );
			SaveFile_XML( i_Locator, i_bResetDirtyFlags );
			//SaveFile_ASCII( i_Locator, i_bResetDirtyFlags );
			break;
		}
	}
#endif
}

//--------------------------------------------------------------------
// IsLoading - return true if this document is currently loading
//--------------------------------------------------------------------
bool docDocumentWithChunks::IsLoading()
{
	return m_bIsLoading;
}

//--------------------------------------------------------------------
// IsDirty
//--------------------------------------------------------------------
bool  docDocumentWithChunks::IsDirty()
{
	for (int i=0; i<m_DocChunks.size(); i++)
		if (m_DocChunks[i]->IsDirty())
			return true;
	return false;
}

//--------------------------------------------------------------------
// SetActive
//--------------------------------------------------------------------
void  docDocumentWithChunks::SetActive()
{
	for (int i=0; i<m_DocChunks.size(); i++)
	{
		m_DocChunks[i]->SetActive();
	}
}

//--------------------------------------------------------------------
// SetInactive
//--------------------------------------------------------------------
void  docDocumentWithChunks::SetInactive( bool i_bUpdateData )
{
	for (int i=0; i<m_DocChunks.size(); i++)
	{
		m_DocChunks[i]->SetInactive( i_bUpdateData );
	}
}

//--------------------------------------------------------------------
// SetWriteFormat - set the format for output
//--------------------------------------------------------------------
//virtual 
void docDocumentWithChunks::SetWriteFormat( int i_WriteFormat )
{
	m_WriteFormat = i_WriteFormat;
}

//--------------------------------------------------------------------
// Filename
//--------------------------------------------------------------------
const fsLocator& docDocumentWithChunks::GetFilename() const
{
	return m_Filename;
}
void  docDocumentWithChunks::SetFilename(const fsLocator &i_Filename)
{
	m_Filename = i_Filename;
}

//--------------------------------------------------------------------
// AddDocumentChunk - this document assumes ownership
//--------------------------------------------------------------------
void docDocumentWithChunks::AddDocumentChunk(docDocumentChunk* i_pChunk)
{
	m_DocChunks.push_back(i_pChunk);
}

//--------------------------------------------------------------------
// RemoveDocumentChunk - remove this chunk from the document
//--------------------------------------------------------------------
void docDocumentWithChunks::RemoveDocumentChunk(docDocumentChunk* i_pChunk)
{
	DBG_ASSERT( i_pChunk != 0, "cannot remove a NULL chunk" );

	std::vector<docDocumentChunk*>::iterator it, end = m_DocChunks.end();
	for (it = m_DocChunks.begin(); it != end; ++it)
	{
		if ((*it) == i_pChunk)
		{
			m_DocChunks.erase( it );
			return;
		}
	}
}

//--------------------------------------------------------------------
//	GetDocumentChunk() - get a document chunk
//--------------------------------------------------------------------
docDocumentChunk* docDocumentWithChunks::GetDocumentChunk( const int i_Index )
{
	DBG_ASSERT( i_Index < m_DocChunks.size(), "chunk index out of range" );

	return m_DocChunks[ i_Index ];
}

//--------------------------------------------------------------------
//	GetNumDocumentChunks() - get the number of document chunks
//--------------------------------------------------------------------
int docDocumentWithChunks::GetNumDocumentChunks()
{
	return m_DocChunks.size();
}

//--------------------------------------------------------------------
//	Get a list of resources as unique full paths with no hierarchy.
//--------------------------------------------------------------------
void docDocumentWithChunks::GetResourceList( fsResourceTrackerData& o_ResourceTrackerData )
{
	o_ResourceTrackerData.m_Resources.clear();
	o_ResourceTrackerData.SetCurrentDepth(0);
	std::vector<docDocumentChunk*>::iterator it, end = m_DocChunks.end();
	for (it = m_DocChunks.begin(); it != end; ++it)
	{
		(*it)->GetResourceList( o_ResourceTrackerData );
	}

	//fsResourceTracker::Debug_OutputList(o_ResourceTrackerData);
}


//--------------------------------------------------------------------
// Load
//--------------------------------------------------------------------
//virtual 
void docDocumentWithChunks::LoadFile(chReader& i_Reader, LoadFlags i_LoadMethod)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	//DBG_WARNING("Loading document-----------------------------start");
	try
	{
		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			// could hash from name to chunk if there gets
			// to be too many. But in the grand scheme of loading
			// a file, how much time is spent reading top-level chunks?
			//
			for (int i=0; i<m_DocChunks.size(); i++)
			{
				//	read the chunk if it matches
				//
				if (m_DocChunks[i]->GetChunkName() == name)
				{
					// DEBUG ONLY
					//char *name_str = (char*)(&name);
					//DBG_WARNING4("Chunk: %c%c%c%c", name_str[0], name_str[1], name_str[2], name_str[3]);
					//chChunkParserUtil::DebugDisplayChunkName(name);

					switch(i_LoadMethod)
					{
						case e_Append_NoDupes:
							m_DocChunks[i]->Import(i_Reader, version, size);
							break;
						case e_Append_RenameDupes:
						{
							const bool c_bRENAME_DUPES = true;
							m_DocChunks[i]->Import(i_Reader, version, size, c_bRENAME_DUPES);
							break;
						}
						case e_New_Doc:
						default:
							m_DocChunks[i]->Read(i_Reader, version, size);
							break;
					}

					// if the chunk is finished, jump out of the for loop so we don't
					// waste checking the rest of the document chunk names.
					break;
				}
			}

			i_Reader.FinishChunk();
		}
	}
	catch( const chInvalidChunkX& )
	{
		DBG_WARNING("Invalid Chunk while reading docDocumentWithChunks file");
		throw;
	}
	//catch( const fsNewerVersionAbortX& )
	//{
	//	throw;
	//}

	//DBG_WARNING("Loading document-----------------------------end");
}

//--------------------------------------------------------------------
//	Load as binary format
//--------------------------------------------------------------------
//virtual 
bool docDocumentWithChunks::LoadFile_Binary( const fsLocator &i_Locator, LoadFlags i_LoadMethod )
{
	gfFileBin doc_file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	gfFileBin::Header header;
	doc_file.ReadHeader(&header);
	// could check version here?

	chBinReader reader(doc_file);

	try
	{
		LoadFile( reader, i_LoadMethod );

		if ( i_LoadMethod == e_New_Doc )
		{
			m_Filename = i_Locator;
		}
	}
	catch( const chInvalidChunkX& )
	{
		throw;
	}
	catch( const fsNewerVersionAbortX& )
	{
		throw;
	}

	return reader.IsNewerVersion();
}

//--------------------------------------------------------------------
//	Load as XML format
//--------------------------------------------------------------------
//virtual 
bool docDocumentWithChunks::LoadFile_XML( const fsLocator &i_Locator, LoadFlags i_LoadMethod )
{
	gfFileXML doc_file(i_Locator, fsFileStream::e_ReadOnly);
	gfFileXML::Header header;
	doc_file.ReadHeader(&header);

	chXMLReader reader(doc_file);

	//	read the root tag
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;
	reader.ReadChunkHeader(name, version, size);
	if (name != c_SGPU)
	{
		DBG_ASSERT( false, "Invalid XML file" );
		throw chInvalidChunkX();
	}

	//	load the meat of the file
	try
	{
		LoadFile( reader, i_LoadMethod );

		if ( i_LoadMethod == e_New_Doc )
		{
			m_Filename = i_Locator;
		}
	}
	catch( const chInvalidChunkX& )
	{
		throw;
	}
	catch( const fsNewerVersionAbortX& )
	{
		throw;
	}

	return reader.IsNewerVersion();
}

//--------------------------------------------------------------------
//	Load as ASCII format
//--------------------------------------------------------------------
//virtual 
bool docDocumentWithChunks::LoadFile_ASCII( const fsLocator &i_Locator, LoadFlags i_LoadMethod )
{
	gfFileTxt doc_file(i_Locator, fsFileStream::e_ReadOnly);

	try
	{
		chTxtReader reader(doc_file);
		LoadFile( reader, i_LoadMethod );

		if ( i_LoadMethod == e_New_Doc )
		{
			m_Filename = i_Locator;
		}

		return reader.IsNewerVersion();
	}
	catch( const chInvalidChunkX& )
	{
		throw;
	}
	catch( const fsNewerVersionAbortX& )
	{
		throw;
	}
}

//--------------------------------------------------------------------
// Save
//--------------------------------------------------------------------
//virtual 
void  docDocumentWithChunks::SaveFile(chWriter& i_Writer, bool i_bResetDirtyFlags)
{
#ifndef DOC_NOSAVING
	for (int i=0; i<m_DocChunks.size(); i++)
	{
		m_DocChunks[i]->Write( i_Writer, i_bResetDirtyFlags );
	}
#endif
}

//--------------------------------------------------------------------
//	Save as binary format
//--------------------------------------------------------------------
//virtual 
void docDocumentWithChunks::SaveFile_Binary( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	// Create document file
	//
	fsLocator save_file = i_Locator;
	save_file.ReplaceExtension("mab");
	if (fsFileUtil::FileExists(save_file))
	{
		fsFileUtil::DeleteFile(save_file);
	}

	fsFileUtil::CreateFile(save_file);

	gfFileBin doc_file(save_file, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	doc_file.WriteHeader(0, 0, 0);

	chBinWriter writer(doc_file);

	SaveFile( writer, i_bResetDirtyFlags );

	m_Filename = save_file;
}

//--------------------------------------------------------------------
//	Save as XML format
//--------------------------------------------------------------------
//virtual 
void docDocumentWithChunks::SaveFile_XML( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	// Create document file
	//
	fsLocator save_file = i_Locator;
	save_file.ReplaceExtension("xml");
	if (fsFileUtil::FileExists(save_file))
	{
		fsFileUtil::DeleteFile(save_file);
	}

	fsFileUtil::CreateFile(save_file);

	gfFileXML doc_file(save_file, fsFileStream::e_WriteOnly);

	doc_file.WriteHeader();

	chXMLWriter writer(doc_file);

	SaveFile( writer, i_bResetDirtyFlags );

	doc_file.WriteFooter();

	m_Filename = save_file;
}

//--------------------------------------------------------------------
//	Save as ASCII format
//--------------------------------------------------------------------
//virtual 
void docDocumentWithChunks::SaveFile_ASCII( const fsLocator &i_Locator, bool i_bResetDirtyFlags )
{
	DBG_ASSERT(false, "Need to implement ASCII writing of a document with chunks");
}

