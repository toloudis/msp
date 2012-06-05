/*****************************************************************************
**	mnmAppPackageDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/data/mnmAppPackageDocumentChunk.hpp"

#include "Support/mnm/data/mnmAppPackageParser.hpp"
#include "Support/mnm/mnmAppPackageMgr.hpp"

#include "Core/Fs/fsFileX.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_APPV = chDefs::MakeName('A', 'P', 'P', 'V');	// application version info
}


//============================================================================
// static variable
//============================================================================
mnmAppPackageDocumentChunk* mnmAppPackageDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
mnmAppPackageDocumentChunk* mnmAppPackageDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmAppPackageDocumentChunk::mnmAppPackageDocumentChunk()
:	docDocumentChunk(),
	m_bChunkRead(false)
{
	//m_CommentString = "Comment";
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::Clear()
{
	//m_CommentString = "Comment";
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  mnmAppPackageDocumentChunk::GetChunkName() const
{
	return c_APPV;
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* mnmAppPackageDocumentChunk::GetChunkDesc() const
{
	return "Application Version";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void mnmAppPackageDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void mnmAppPackageDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::Read( chReader &io_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size )
{
	mnmAppPackageParser::Read( io_Reader, i_Version, i_Size, m_Data );
	mnmAppPackageMgr::SetSceneData( m_Data );
	m_bChunkRead = true;

	if (mnmAppPackageMgr::IsRestricted())
	{
		mnmAppPackageData sdata = mnmAppPackageMgr::GetSceneData();
		mnmAppPackageData adata = mnmAppPackageMgr::GetAppData();

		//	If names don't match, don't allow file to be read in.
		//
		//	TODO - allow multiple apps, versions, etc.
		//
		//if (sdata.m_SGPUAppName != adata.m_SGPUAppName)
		if (!(sdata.m_SGPUAppName.HasSubString(adata.m_SGPUAppName)))
		{
			throw fsUnsupportedVersionX( io_Reader.GetLocator() );
		}
	}
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::Write(chWriter &io_Writer, bool i_bResetDirtyFlag)
{
	m_Data = mnmAppPackageMgr::GetAppData();
	mnmAppPackageParser::Write( io_Writer, m_Data );
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool mnmAppPackageDocumentChunk::IsDirty() const
{
	return false;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::SetActive()
{
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if (sm_pActiveChunk == this)
		sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
//  This virtual function is called on all chunks after all 
//	chunks have finished loading successfully
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::NotifyLoadFinished()
{
	if (sm_pActiveChunk == this)
	{
		//	if this chunk didn't get read, then this is file from
		//	an older version of one of our apps.
		//
		if (!m_bChunkRead)
		{
			if (mnmAppPackageMgr::IsRestricted())
			{
				// comment this out so we can convert scenes
				throw fsUnsupportedVersionX( fsLocator() );
			}
		}
		m_bChunkRead = false;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::SetupPaths()
{
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void mnmAppPackageDocumentChunk::DataChanged()
{
}
