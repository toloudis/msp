/*****************************************************************************
**	ImportUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Import/ImportUtil.hpp"

#include "Features/Import/ImportData.hpp"
#include "Features/Import/ImportDialogUtil.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Features/SceneSetup/Data/SceneSetupDocumentChunk.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/doc/docDocumentWithChunks.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include <string>


//============================================================================
//============================================================================
namespace ImportUtil
{
	//------------------------------------------------------------------------
	//  AddToMenu() - add Import actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Import
		pCmd = new cmaCommandSimple("Import", 
									"File", 
									"Import objects from one scene into current scene",
									
									&ImportDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "File", "Import", pCmd );
	}

	//------------------------------------------------------------------------
	//	Build a data list from the passed in doc.
	//------------------------------------------------------------------------
	void BuildDataList( docDocument* i_pDoc, ImportData& o_Data )
	{
		docDocumentWithChunks *pDoc = dynamic_cast<docDocumentWithChunks*>(i_pDoc);
		DBG_ASSERT( pDoc != NULL, "Need a doc to build a datalist" );

		docDocumentChunk* pChunk;
		int numchunks;

		numchunks = pDoc->GetNumDocumentChunks();
		o_Data.m_Chunks.resize( numchunks );

		//DBG_LOG( "BuildDataList with " << numchunks << " chunks" );

		int i;
		for ( i = 0; i < numchunks ; ++i )
		{
			pChunk = pDoc->GetDocumentChunk( i );

			//DBG_LOG2( " chunk #%02d [%s]", i, pChunk->GetChunkDesc() );

			//	Set the chunk import data
			o_Data.m_Chunks[i].m_Desc = pChunk->GetChunkDesc();
			o_Data.m_Chunks[i].m_bRemovableChunk = false;

			//	now, have the chunk fill in it's item list however
			//	it needs to.
			//
			pChunk->BuildDataList( o_Data.m_Chunks[i].m_Items );
		}
	}

	//------------------------------------------------------------------------
	//	Remove items in the list from the doc
	//------------------------------------------------------------------------
	void remove_items( docDocument* io_pDoc, const ImportData& i_Data )
	{
		docDocumentWithChunks *pDoc = dynamic_cast<docDocumentWithChunks*>(io_pDoc);
		DBG_ASSERT( pDoc != NULL, "Need a doc to remove items" );

		//DBG_TRACE( "IMPORT UTIL: remove_items" );

		docDocumentChunk* pChunk;
		int i,j;
		int numdatachunks;
		int numdocchunks;

		numdocchunks	= pDoc->GetNumDocumentChunks();
		numdatachunks	= i_Data.m_Chunks.size();
		for ( j=numdocchunks-1 ; j >=0 ; --j )
		{
			pChunk = pDoc->GetDocumentChunk(j);

			//DBG_TRACE("    doc chunks" << j << " of " << numdocchunks << " - " << pChunk->GetChunkDesc());

			bool bFoundMatch = false;
			bool bRemoveChunk = false;
			for ( i=0 ; i < numdatachunks ; ++i )
			{
				//	find if the document chunk exists in the import list
				//
				if ( strcmp( i_Data.m_Chunks[i].m_Desc.c_str(), pChunk->GetChunkDesc()) == 0 )
				{
					bFoundMatch = true;

					//	if this chunk is removable, jump out
					if (i_Data.m_Chunks[i].m_bRemovableChunk)
					{
						bRemoveChunk = true;
						//DBG_TRACE("        removable chunk: " << j << " - " << pChunk->GetChunkDesc());
						break;
					}

					//	if there are items in the list, remove them.
					//
					if ( i_Data.m_Chunks[i].m_Items.size() > 0 )
					{
						//DBG_TRACE( "      removing items" );
						//for ( int q = 0 ; q < i_Data.m_Chunks[i].m_Items.size() ; ++q )
						//{
						//	DBG_TRACE("        " << q << " - " << i_Data.m_Chunks[i].m_Items[q].c_str() );
						//}

						pChunk->RemoveItems( i_Data.m_Chunks[i].m_Items );
					}
					break;
				}
			}

			//	if it didn't find a match, then it wasn't in the list (i.e. no items so chunk 
			//	not originally written) so just remove it.
			//
			if ( !bFoundMatch )
			{
				bRemoveChunk = true;
			}

			//	remove the chunk if the chunk isn't in the import list or it is empty
			//
			if ( bRemoveChunk )
			{
				//DBG_TRACE( "            REMOVING CHUNK" );

				// don't need this chunk anymore
				//
				pDoc->RemoveDocumentChunk( pChunk );
				delete pChunk;	// delete the chunk itself
			}
		}
	}

	//------------------------------------------------------------------------
	//	Import one Document into another
	//		The i_SourceData list are items to REMOVE from the source document
	//	(i.e. not import into the destination).  An empty list will merge
	//	everything.
	//------------------------------------------------------------------------
	void ImportDoc( docDocument* io_pDestDoc, docDocument* io_pSourceDoc, const ImportData& i_SourceData )
	{
		ProjectSetupData psdata;

		//std::string name;
		//fsFileUtil::LocatorToANSIFilename(io_pSourceDoc->GetFilename(), name);
		//DBG_LOG("src  file: (" << name.c_str() << ")" );
		//fsFileUtil::LocatorToANSIFilename(io_pDestDoc->GetFilename(), name);
		//DBG_LOG("dest file: (" << name.c_str() << ")" );

		//	remove the items from the doc before saving it
		//
		remove_items( io_pSourceDoc, i_SourceData );

		//	create the temporary name
		//
		fsLocator temp_locator;
		temp_locator = i_SourceData.m_LastFile;
		itString filename;
		filename = itString("~temp_");
		filename += temp_locator.GetLastName();
		temp_locator.Pop();
		temp_locator.Push( filename );
		//fsFileUtil::LocatorToANSIFilename(temp_locator, name);
		//DBG_LOG("temporary file: (" << name.c_str() << ")" );

		//	save the doc to a temporary file
		//
		io_pSourceDoc->Save( temp_locator, false );

		//	store the timeline max time since the load resets it
		//
		maTime mintime = tmlnTimeLine::GetMinimum();
		maTime maxtime = tmlnTimeLine::GetMaximum();

		//	since the source doc changed the paths, 
		//	reset the paths correctly.
		//
		docDocumentChunk* pChunk;
		SceneSetupDocumentChunk* pSSChunk;
		docDocumentWithChunks* pDoc = dynamic_cast<docDocumentWithChunks*>(io_pDestDoc);
		int numchunks = pDoc->GetNumDocumentChunks();
		//DBG_TRACE("Importing " << numchunks);
		int i;
		for ( i = 0; i < numchunks ; ++i )
		{
			pChunk		= pDoc->GetDocumentChunk( i );
			pSSChunk	= dynamic_cast<SceneSetupDocumentChunk*>(pChunk);
			if ( pSSChunk != 0 )
			{
				// save the data for after the load
				//
				//	Note: the ProjectSetupMgr data is already changed before it gets into
				//	this function, so get cannot just get the PSMgr data.
				//
				pSSChunk->GetData(psdata);

				////	DEBUG info
				//DBG_LOG3("Import: save project \n\t(%s)\n\t(%s)\n\t(scene=%s)", psdata.m_ProjectName.c_str(), psdata.m_ProjectDirectory.c_str(), psdata.m_CurrentSceneName.c_str());
				//for (int j=0;j<psdata.m_Scenes.size();++j)
				//{
				//	DBG_LOG2("\t\t%03d %s", j, psdata.m_Scenes[j].m_SceneName.c_str());
				//}
				break;
			}
		}

		//	load the temp doc into the current one
		//
		io_pDestDoc->Load( temp_locator, i_SourceData.m_LoadMethod );

		//	reset the project paths
		//
		if (pSSChunk != 0)
		{
			ProjectSetupMgr::SetData(psdata);

			//	set the chunk path also, so subsequent imports will still work correctly.
			//
			pSSChunk->SetPathsInChunk(psdata);
		}

		//	reset the timeline maximum
		//
		tmlnTimeLine::SetTimeRange( mintime, maxtime );

		//	delete the temporary file
		//
		fsFileUtil::DeleteFile( temp_locator );
	}
}
