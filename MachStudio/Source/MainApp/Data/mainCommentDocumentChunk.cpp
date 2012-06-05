/*****************************************************************************
**	mainCommentDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Data/mainCommentDocumentChunk.hpp"

#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_CMNT = chDefs::MakeName('C', 'M', 'N', 'T');	// executable version info
}


//============================================================================
// static variable
//============================================================================
mainCommentDocumentChunk* mainCommentDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
mainCommentDocumentChunk* mainCommentDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mainCommentDocumentChunk::mainCommentDocumentChunk()
: docDocumentChunk()
{
	m_CommentString = "Comment";
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  mainCommentDocumentChunk::Clear()
{
	m_CommentString = "Comment";
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  mainCommentDocumentChunk::GetChunkName() const
{
	return c_CMNT;
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* mainCommentDocumentChunk::GetChunkDesc() const
{
	return "Comment";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void mainCommentDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void mainCommentDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  mainCommentDocumentChunk::Read( chReader &i_Reader,
									  chDefs::Version i_Version,
									  chDefs::Size i_Size )
{
	if (i_Version == 0)
	{
		std::string comment;
		comment.resize(i_Size);
		i_Reader.Read( comment );
		m_CommentString = comment.c_str();
	}
	else
	{
		chChunkParserUtil::Read(i_Reader, m_CommentString);
	}
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void mainCommentDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag)
{
	const int c_CMNT_VERSION = 1;
	i_Writer.WriteChunkHeader(c_CMNT, c_CMNT_VERSION, false);

	chChunkParserUtil::Write(i_Writer, m_CommentString);

	i_Writer.FinishChunk();
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool mainCommentDocumentChunk::IsDirty() const
{
	return false;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  mainCommentDocumentChunk::SetActive()
{
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  mainCommentDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
//  This virtual function is called on all chunks after all 
//	chunks have finished loading successfully
//--------------------------------------------------------------------
void  mainCommentDocumentChunk::NotifyLoadFinished()
{
	if (sm_pActiveChunk == this)
	{
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mainCommentDocumentChunk::SetupPaths()
{
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void mainCommentDocumentChunk::DataChanged()
{
}
