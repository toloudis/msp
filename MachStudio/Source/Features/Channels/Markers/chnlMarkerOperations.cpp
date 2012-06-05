/*****************************************************************************
**	chnlMarkerOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"
#include "Features/Channels/Data/chnlTimeData.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Undo/chnlAddOperation.hpp"
#include "Features/Channels/Undo/chnlDeleteOperation.hpp"
#include "Features/Channels/Undo/chnlDataOperation.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"

#include "Tool/gui/guiPropertyDialog.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/undo/undoUndoMgr.hpp"

namespace chnlMarkerOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Add Marker";
		const char *c_DeleteOperationDisplayName = "Delete Marker";
		const char *c_DataOperationDisplayName = "Set Marker Data";
	}

	//--------------------------------------------------------------------
	// Create a new marker for the the channels timeline
	//--------------------------------------------------------------------
	void AddMarker()
	{
		if (chnlMarkerMgr::AddMarker(tmlnTimeLine::GetValue()))
		{
			chnlTimeDocumentChunk::ActiveDataChanged();
			chnlDialogUtil::UpdateMarkersAndNotes();
			undoUndoMgr::AddOperation(new chnlMarkerAddOperation(tmlnTimeLine::GetValue(), 
				0 /*Normal Type*/, 
				"" /*No Note*/, c_AddOperationDisplayName));
		}
	}
	void AddMarker(const maTime& i_Time, 
				   int &i_Type, 
				   const std::string &i_Note)
	{
		if (chnlMarkerMgr::AddMarker(i_Time, i_Type, i_Note))
		{
			chnlTimeDocumentChunk::ActiveDataChanged();
			chnlDialogUtil::UpdateMarkersAndNotes();
			undoUndoMgr::AddOperation(new chnlMarkerAddOperation(i_Time, i_Type, i_Note, c_AddOperationDisplayName));
		}
	}

	//--------------------------------------------------------------------
	// Delete a marker by time
	//--------------------------------------------------------------------
	void DeleteMarker()
	{
		chnlMarkerOperations::DeleteMarker(tmlnTimeLine::GetValue());
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}
	void DeleteMarker(const maTime& i_Time)
	{
		//undo needs to be set before the data is removed from the manager
		int marker_index = chnlMarkerMgr::GetMarkerIndexAtTime(i_Time);
		if (marker_index != -1)
		{
			chnlMarkerDataItem marker_data = chnlMarkerMgr::GetMarkerData(marker_index);

			undoUndoMgr::AddOperation(new chnlMarkerDeleteOperation(marker_data.m_Time.GetValue(), 
															marker_data.m_TimeMarkerType.GetValue(), 
															marker_data.m_Note.GetValue(), c_DeleteOperationDisplayName));
		}

		chnlMarkerMgr::DeleteMarker(i_Time);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}

	void DeleteMarkerAtFrame(int i_Frame)
	{
		std::vector<chnlMarkerDataItem> targetMarkers;
		chnlMarkerMgr::GetMarkersAtFrame(i_Frame, targetMarkers);

		for (int i = 0; i < targetMarkers.size(); i++)
		{
			DeleteMarker(targetMarkers[i].m_Time.GetValue());
		}
	}

	//--------------------------------------------------------------------
	// Move current time to next or previous marker
	//--------------------------------------------------------------------
	void MoveToNextMarker()
	{
		maTime mtime = chnlMarkerMgr::GetNextMarkerTime(tmlnTimeLine::GetValue());
		if (mtime < maTime::c_ZeroTime)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMaximum());
		else
			tmlnTimeLine::SetValue(mtime);
		chnlDialogUtil::UpdateChannels();
	}
	void MoveToPrevMarker()
	{
		maTime mtime = chnlMarkerMgr::GetPrevMarkerTime(tmlnTimeLine::GetValue());
		if (mtime < maTime::c_ZeroTime)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMinimum());
		else
			tmlnTimeLine::SetValue(mtime);
		chnlDialogUtil::UpdateChannels();
	}

	//--------------------------------------------------------------------
	// Set data for given marker by index
	//--------------------------------------------------------------------
	void SetMarkerData(int i_Index, const chnlMarkerDataItem &i_Data)
	{
		chnlMarkerDataItem old_data = chnlMarkerMgr::GetMarkerData(i_Index);
		chnlMarkerMgr::SetMarkerData(i_Index, i_Data);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
		undoUndoMgr::AddOperation(new chnlMarkerDataOperation(i_Index, old_data, i_Data, c_DataOperationDisplayName));
	}

	//--------------------------------------------------------------------
	// Show Dialog to edit properties of marker at given time
	//--------------------------------------------------------------------
	void ShowProperties(const maTime& i_Time)
	{
		int marker_index = 0;
		if (chnlMarkerMgr::SnapTimeToMarkers(i_Time, marker_index))
		{
			// Get a local copy of the marker data, only set the data if the
			// property dialog returns OK
			chnlMarkerDataItem marker_data = chnlMarkerMgr::GetMarkerData(marker_index);
			prtyPropertyUIInfoContainer markerInfo;

			// Create enum property for purposes of our interface.
			prtyEnum marker_type;
			marker_type.SetValue( marker_data.m_TimeMarkerType.GetValue() );
			marker_type.SetEnumTag(e_MarkerType_Normal, "Type Normal");
			marker_type.SetEnumTag(e_MarkerType_In, "Type In");
			marker_type.SetEnumTag(e_MarkerType_Out, "Type Out");
			markerInfo.Add(new prtyComboBoxUIInfo(&marker_type));

			prtyTextBoxUIInfo *pTBUII = new prtyTextBoxUIInfo(&marker_data.m_Note);
			pTBUII->SetMultiline(true);
			markerInfo.Add(pTBUII);
			if (guiPropertyDialog::ShowModal("Marker Properties", markerInfo)
									== guiPropertyDialog::e_OK)
			{
				// Set value of enum property into our local data
				marker_data.m_TimeMarkerType.SetValue( marker_type.GetValue() );

				// Set the new data for the marker
				chnlMarkerMgr::SetMarkerData(marker_index, marker_data);

				// Update the user interface
				chnlDialogUtil::UpdateMarkersAndNotes();
			}
		}

	}

}	// end of namespace

