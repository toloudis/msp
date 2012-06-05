/*****************************************************************************
**	SceneSetupDocumentChunk.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/SceneSetup/Data/SceneSetupDocumentChunk.hpp"

#include "Features/SceneSetup/Data/SceneSetupDataParser.hpp"
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <assert.h>
#include <string>


//============================================================================
// static variable
//============================================================================
SceneSetupDocumentChunk* SceneSetupDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
SceneSetupDocumentChunk* SceneSetupDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
SceneSetupDocumentChunk::SceneSetupDocumentChunk() 
:	m_bDirty(false), 
	m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  SceneSetupDocumentChunk::Clear()
{
	m_bDirty = false;

	if (this->m_bActive)
	{
		//SceneSetupDialogUtil::UpdateSceneSetupListDialog();
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  SceneSetupDocumentChunk::GetChunkName() const
{
	return SceneSetupDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* SceneSetupDocumentChunk::GetChunkDesc() const
{
	return "SceneSetup";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void SceneSetupDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void SceneSetupDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  SceneSetupDocumentChunk::Read( chReader &i_Reader,
									 chDefs::Version i_Version,
									 chDefs::Size i_Size )
{
	SceneSetupDialogUtil::CleanSceneProperties();
	SceneSetupData& data = SceneSetupDialogUtil::Data();
	SceneSetupDataParser::ReadData( i_Reader, i_Version, i_Size, data );

	// immediately after reading, set the appropriate items
	//
	m_PData.m_ProjectName		= data.m_Data.m_ProjectName;
	m_PData.m_ProjectDirectory	= data.m_Data.m_ProjectDirectory;
	m_PData.m_CurrentSceneName	= data.m_Data.m_SceneName;

	// set the directory properly because it may be different on each
	// user's machine
	//
	fsLocator proj_dir( i_Reader.GetLocator() );
	bool bRemoved = proj_dir.RemoveAfter( itString("Shots") );
	if (!bRemoved) 
		DBG_ERROR0("ERROR-Locator directory removal UNSUCCESSFUL");
	proj_dir.Pop();

	std::string pdir;
	fsFileUtil::LocatorToANSIFilename(proj_dir, pdir);
	m_PData.m_ProjectDirectory = pdir;

	//
	DBG_LOG0("-----------PROJECT SETTINGS-----------");
	DBG_LOG1("Scene Project Directory (%s)", m_PData.m_ProjectDirectory.c_str() );
	DBG_LOG1("Scene Project Name      (%s)", m_PData.m_ProjectName.c_str() );
	DBG_LOG1("Scene Current           (%s)", m_PData.m_CurrentSceneName.c_str() );

	//
	SetupPaths();

	//SceneSetupDialogUtil::UpdateSceneProperties();

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  SceneSetupDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	SceneSetupData& data = SceneSetupDialogUtil::Data();

	SceneSetupDataParser::WriteData( i_Writer, data );

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  SceneSetupDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  SceneSetupDocumentChunk::SetActive()
{
	m_bActive = true;
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  SceneSetupDocumentChunk::SetInactive( bool i_bUpdateData )
{
	m_bActive = false;
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void SceneSetupDocumentChunk::SetupPaths()
{
	ProjectSetupMgr::SetData( m_PData );
}

//--------------------------------------------------------------------
//	Set the chunks data
//--------------------------------------------------------------------
void SceneSetupDocumentChunk::SetPathsInChunk(ProjectSetupData& i_Data)
{
	m_PData = i_Data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void SceneSetupDocumentChunk::GetData(ProjectSetupData& o_Data)
{
	ProjectSetupMgr::CopyData(m_PData, o_Data);
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void SceneSetupDocumentChunk::DataChanged()
{
	m_bDirty = true;
}

