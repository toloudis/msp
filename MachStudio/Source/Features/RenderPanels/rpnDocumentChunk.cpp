/*****************************************************************************
**	rpnDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnDocumentChunk.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <assert.h>
#include <string>
#include <time.h>

// Includes from old managed file
#define RPN_PANELLAYOUT_HPP
#ifndef RPN_RENDERPANE_HPP
#include "Features/RenderPanels/rpnRenderPane.hpp"
#endif

//============================================================================
// static variable
//============================================================================
const chDefs::Name c_RNPL = chDefs::MakeName('R', 'N', 'P', 'L');

rpnDocumentChunk* rpnDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
rpnDocumentChunk* rpnDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rpnDocumentChunk::rpnDocumentChunk()
: docDocumentChunk()
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  rpnDocumentChunk::Clear()
{
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  rpnDocumentChunk::GetChunkName() const
{
	return c_RNPL;
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* rpnDocumentChunk::GetChunkDesc() const
{
	return "Render Panels";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void rpnDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void rpnDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  rpnDocumentChunk::Read( chReader &i_Reader,
								 chDefs::Version i_Version,
								 chDefs::Size i_Size )
{
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void rpnDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag)
{
	// Last layout could go here, but for now do nothing
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  rpnDocumentChunk::IsDirty() const
{
	return false;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  rpnDocumentChunk::SetActive()
{
	sm_pActiveChunk = this;


#ifdef USE_WXWIDGETS
	if (rpnPanelGrid::Instance != NULL)
	{
		rpnPanelGrid::Instance->SetLayoutStyle( rpnPanelGrid::e_SinglePane );
		for (int i=0; i<4; ++i)
			rpnPanelGrid::Instance->SetCamera(i, cam3dMgr::GetEditorCameraProxy(), "Editor");
	}
#endif
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  rpnDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnDocumentChunk::SetupPaths()
{
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void rpnDocumentChunk::DataChanged()
{
}
