/*****************************************************************************
**	chnlMarkerOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"

#include "Tool/gui/guiPropertyDialog.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"


namespace chnlMarkerOperations
{
	//--------------------------------------------------------------------
	// Create a new marker for the the channels timeline
	//--------------------------------------------------------------------
	void AddMarker()
	{
		//TODO: Needs undo operation
		chnlMarkerMgr::AddMarker(tmlnTimeLine::GetValue());
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}
	void AddMarker(float i_Time, 
				   int &i_Type, 
				   const std::string &i_Note)
	{
		//TODO: Needs undo operation
		chnlMarkerMgr::AddMarker(i_Time, i_Type, i_Note);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
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
	void DeleteMarker(float i_Time)
	{
		//TODO: Needs undo operation
		chnlMarkerMgr::DeleteMarker(i_Time);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}

	//--------------------------------------------------------------------
	// Move current time to next or previous marker
	//--------------------------------------------------------------------
	void MoveToNextMarker()
	{
		float mtime = chnlMarkerMgr::GetNextMarkerTime(tmlnTimeLine::GetValue());
		if (mtime < 0)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMaximum());
		else
			tmlnTimeLine::SetValue(mtime);
	}
	void MoveToPrevMarker()
	{
		float mtime = chnlMarkerMgr::GetPrevMarkerTime(tmlnTimeLine::GetValue());
		if (mtime < 0)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMinimum());
		else
			tmlnTimeLine::SetValue(mtime);
	}

	//--------------------------------------------------------------------
	// Set data for given marker by index
	//--------------------------------------------------------------------
	void SetMarkerData(int i_Index, const chnlMarkerDataItem &i_Data)
	{
		//TODO: Needs undo operation
		chnlMarkerMgr::SetMarkerData(i_Index, i_Data);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}

	//--------------------------------------------------------------------
	// Show Dialog to edit properties of marker at given time
	//--------------------------------------------------------------------
	void ShowProperties(float i_Time)
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
			if (guiPropertyDialog::ShowModal("Marker Properties", markerInfo.GetList())
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

