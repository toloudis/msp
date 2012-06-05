/****************************************************************************\
**	fbxMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fbx/fbxMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"

#include "Support/fbx/fbxExporter.hpp"
#include "Support/fbx/fbxExportInterest.hpp"
#include "ImportExport/fbx/export/fbxExportData.hpp"

#include <vector>

//============================================================================
//============================================================================
namespace
{
	std::vector<fbxExportInterest*> l_ExportInterestList;
	bool		l_bExportSelected;
}

//--------------------------------------------------------------------
// Gather up the list of the potential things to export and
// return in data structure
//--------------------------------------------------------------------
fbxExportData fbxMgr::GetPotentialExportData()
{
	fbxExportData data;

	const int num_interests = l_ExportInterestList.size();
	data.m_SceneData.resize(num_interests);

	for ( int i=0; i < num_interests; i++ )
	{
		//	Set the chunk export data
		data.m_SceneData[i].m_Desc = l_ExportInterestList[i]->GetChunkDesc();
		l_ExportInterestList[i]->GatherSceneData( data.m_SceneData[i] , data.m_bSceneHasBeenBaked );
	}

	return data;
}


//--------------------------------------------------------------------
// Export Maya Ascii file by calling Export functions on each
//	registered interest.
//--------------------------------------------------------------------
void fbxMgr::DoExport( fsLocator &i_Locator, const fbxExportData &i_Data )
{
	fbxExporter exporter( i_Locator );

	const int num_interests = l_ExportInterestList.size();

	// Parse potential data
	for ( int i=0; i < num_interests; i++ )
		if ( i_Data.m_SceneData[i].m_Desc == "Geometry" )
			l_ExportInterestList[i]->Export( exporter , i_Data.m_SceneData[i] );

	for ( int i=0; i < num_interests; i++ )
		if ( i_Data.m_SceneData[i].m_Desc == "Cameras" )
			l_ExportInterestList[i]->Export( exporter , i_Data.m_SceneData[i] );

	exporter.EndExport( i_Locator );
}

//--------------------------------------------------------------------
//	RegisterExportInterest() - add a Export interest to the system
//--------------------------------------------------------------------
void fbxMgr::RegisterExportInterest( fbxExportInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Export Interest" );
	l_ExportInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
// UnRegisterExportInterest
//--------------------------------------------------------------------
void fbxMgr::UnRegisterExportInterest( fbxExportInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_ExportInterestList, i_pInterest );
}

//--------------------------------------------------------------------
// Reset()
//--------------------------------------------------------------------
void fbxMgr::Reset()
{
}

//--------------------------------------------------------------------
// SetExportSelected()
//--------------------------------------------------------------------
void fbxMgr::SetExportSelected( bool i_Val )
{
	l_bExportSelected = i_Val;
}

//--------------------------------------------------------------------
// GetExportSelected()
//--------------------------------------------------------------------
bool fbxMgr::GetExportSelected()
{
	return l_bExportSelected;
}