/****************************************************************************\
**	mexpMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpMgr.hpp"

#include "Support/mexp/mexpExportData.hpp"
#include "Support/mexp/mexpExporter.hpp"
#include "Support/mexp/mexpExportInterest.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

//	library
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Tool/gui/guiProgressDialog.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	std::vector<mexpExportInterest*>	l_ExportInterestList;
}


//--------------------------------------------------------------------
// Gather up the list of the potential things to export and
// return in data structure
//--------------------------------------------------------------------
mexpExportData mexpMgr::GetPotentialExportData()
{
	mexpExportData data;

	const int num_interests = l_ExportInterestList.size();
	data.m_Chunks.resize(num_interests);

	for ( int i=0; i < num_interests; i++ )
	{
		//	Set the chunk export data
		data.m_Chunks[i].m_Desc = l_ExportInterestList[i]->GetChunkDesc();
		data.m_Chunks[i].m_bRemovableChunk = false;

		l_ExportInterestList[i]->GatherItemNames( data.m_Chunks[i].m_Items );
	}

	return data;
}

//--------------------------------------------------------------------
// Export Maya Ascii file by calling Export functions on each
//	registered interest.
//--------------------------------------------------------------------
void mexpMgr::DoExport( fsLocator &i_Locator, const mexpExportData &i_Data )
{
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_Locator, filename);

	mexpExporter exporter(filename.c_str(), 
						  i_Data.m_bExportAnimation, 
						  i_Data.m_bExportJoints);
	if (exporter.OpenFailed())
		throw fsReadOnlyX(i_Locator);

	// Export range of time for scene
	exporter.ExportTimeRange(tmlnTimeLine::GetMinimum(), tmlnTimeLine::GetMaximum());

	// Gather exporting data from interests
	const int num_interests = l_ExportInterestList.size();
	for ( int i=0; i < num_interests; i++ )
	{
		if (!i_Data.m_Chunks[i].m_Items.empty())
		{
			l_ExportInterestList[i]->Export( exporter, i_Data.m_Chunks[i].m_Items );
		}
	}

	// If we need to run a simulation in order to convert the drivers
	// into keys, then do it now.
	if (exporter.HasChannelRecorders())
	{
		guiProgressDialog::Show("Baking animation");

		const maTime original_time = tmlnTimeLine::GetValue();
		
		// Simulate at the requested frame rate
		//TIME - float frame rate passed to FromFrame
		maTime frame_time_delta = maTime::FromFrame(1, i_Data.m_SimulationFrameRate);

		const maTime end = tmlnTimeLine::GetMaximum();
		for (maTime time = tmlnTimeLine::GetMinimum(); time <= end; time += frame_time_delta)
		{
			guiProgressDialog::SetPercentage( (time - tmlnTimeLine::GetMinimum()) /
											  (end - tmlnTimeLine::GetMinimum()) );

			tmlnTimeLine::SetValue( time );

			// Execute drivers, sets values into channels
			tmlnTimelineMgr::Update( time );

			// Record values from channels into keys
			//TIME - should the recorder get a frame number? Was seconds before.
			exporter.RecordFrame( time.AsSeconds() );
		}
		exporter.FinishRecording();

		// Restore old current time
		tmlnTimeLine::SetValue( original_time );

		// Export keys to ascii file
		exporter.Export();
		
		guiProgressDialog::Hide();
	}
}

//--------------------------------------------------------------------
//	RegisterExportInterest() - add a Export interest to the system
//--------------------------------------------------------------------
void mexpMgr::RegisterExportInterest( mexpExportInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Export Interest" );
	l_ExportInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterExportInterest() - remove a Export interest from the system.
//
//	Note: this will NOT delete the Export interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void mexpMgr::UnRegisterExportInterest( mexpExportInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_ExportInterestList, i_pInterest );
}
