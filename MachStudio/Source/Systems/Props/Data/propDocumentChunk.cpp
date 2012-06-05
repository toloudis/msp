/*****************************************************************************
**	propDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004-6 - All Rights Reserved
\****************************************************************************/

#include "Systems/Props/Data/propDocumentChunk.hpp"

// Include this now to be able to turn all old files with props
// into the new "Objects" which is really just SystemCharacter.
// Prop information can't be saved to file anymore.
#include "Systems/Character/Data/chtrScriptData.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Props/Gui/propGeomList.hpp"

namespace
{
	// Convert prop driver codes to chtr driver codes:
	const chDefs::Name c_PATC = chDefs::MakeName('P', 'A', 'C', 'H');  // Prop Attachment
	const chDefs::Name c_PPOS = chDefs::MakeName('P', 'P', 'O', 'S');  // Prop static POSition
	const chDefs::Name c_PPSP = chDefs::MakeName('P', 'P', 'S', 'P');  // Prop Position SPline
	const chDefs::Name c_PORI = chDefs::MakeName('P', 'O', 'R', 'I');  // Prop static ORIentation
	const chDefs::Name c_POSP = chDefs::MakeName('P', 'O', 'S', 'P');  // Prop Orientation SPline
	const chDefs::Name c_PDAF = chDefs::MakeName('P', 'D', 'A', 'F');	// prop drive anim full

	const chDefs::Name c_CATC = chDefs::MakeName('C', 'A', 'C', 'H');  // Character Attachment
	const chDefs::Name c_CPOS = chDefs::MakeName('C', 'P', 'O', 'S');  // Character static POSition
	const chDefs::Name c_CPSP = chDefs::MakeName('C', 'P', 'S', 'P');  // Character Position SPline
	const chDefs::Name c_CORI = chDefs::MakeName('C', 'O', 'R', 'I');  // Character static ORIentation
	const chDefs::Name c_COSP = chDefs::MakeName('C', 'O', 'S', 'P');  // Character Orientation SPline
	const chDefs::Name c_CDAF = chDefs::MakeName('C', 'D', 'A', 'F');	// character driver anim full data
}

//--------------------------------------------------------------------
// If we are converting props to charaters, then we shouldn't
// add the props to the ObjectMgr, just hold that data.
// This flag needs to be coordinated with the one in 
// chtrDocumentChunk.cpp
//--------------------------------------------------------------------
const bool c_bConvertingProps = true;

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* propDocumentChunk::GetChunkDesc() const
{
	return "Props";
}
//--------------------------------------------------------------------
// Convert prop information in this chunk to chtr data
// and *append* the info to the given list.
// Returns true if there were props converted.
//--------------------------------------------------------------------
//static
bool propDocumentChunk::ConvertPropsToCharacters(chtrCharactersData &io_ChtrList)
{
	return (static_cast<propDocumentChunk*>(sm_pActiveChunk)->ConvertPropsToCharactersImpl(io_ChtrList));
}
bool propDocumentChunk::ConvertPropsToCharactersImpl(chtrCharactersData &io_ChtrList)
{
	propPropsData &props_data = this->m_DataList;

	// Convert to characters
	const int num_props = props_data.m_Items.size();
	if (num_props > 0)
	{
		chtrCharactersData char_list;
		char_list.m_Items.resize(num_props);

		for (int i=0; i<num_props; i++)
		{
			chtrScriptData chtr_data;
			propScriptData &prop_data = props_data.m_Items[i];
			//chtrData	m_BaseData;
			//std::vector<tmlnDriverInfo*>	m_Drivers;

			// Filename needs to be converted from single filename
			// to fullpath
			fsysFileList file_list;
			propGeomList::BuildFileList(file_list);
			itString prop_fname = prop_data.m_BaseData.m_Filename.GetValue();
			fsLocator geom_loc;
			file_list.FindFilePath(prop_fname, geom_loc);
			geom_loc.Push(prop_fname);
			chtr_data.m_BaseData.m_Filename = geom_loc;

			// Copy over base data:
			chtr_data.m_BaseData.m_bEditorVisible	= prop_data.m_BaseData.m_bEditorVisible;
			chtr_data.m_BaseData.m_bVisible			= prop_data.m_BaseData.m_bVisible;
			chtr_data.m_BaseData.m_Name				= prop_data.m_BaseData.m_Name;
			chtr_data.m_BaseData.m_Position			= prop_data.m_BaseData.m_Position;
			chtr_data.m_BaseData.m_Orientation		= prop_data.m_BaseData.m_Orientation;
			chtr_data.m_BaseData.m_Scale			= prop_data.m_BaseData.m_Scale;
			chtr_data.m_BaseData.m_PivotPoint		= prop_data.m_BaseData.m_PivotPoint;
			chtr_data.m_BaseData.m_PivotCompensation = prop_data.m_BaseData.m_PivotCompensation;
			chtr_data.m_BaseData.m_bLockedMaterials	= prop_data.m_BaseData.m_bLockedMaterials;

			// Copy over supporting data:
			chtr_data.m_Controls = prop_data.m_Controls;
			chtr_data.m_Materials = prop_data.m_Materials;
			chtr_data.m_ChannelInfo = prop_data.m_ChannelInfo;
			chtr_data.m_Fragments = prop_data.m_Fragments;
			chtr_data.m_AOData = prop_data.m_AOData;

			// Drivers are tricky. The item data "owns" the drivers and will 
			// delete them on destructor, so we need to transfer the pointers and
			// then clear them out from the old data.
			chtr_data.m_Drivers = prop_data.m_Drivers;  
			prop_data.m_Drivers.clear();
			// Also have to convert the chunk names of the drivers from prop codes to chtr codes
			const int num_drivers = chtr_data.m_Drivers.size();
			for (int d=0; d<num_drivers; d++)
			{
				tmlnDriverInfo *pDriverInfo = chtr_data.m_Drivers[d];
				if (pDriverInfo->GetBaseChunkName() == c_PATC)
					pDriverInfo->SetBaseChunkName(c_CATC);
				else if (pDriverInfo->GetBaseChunkName() == c_PPOS)
					pDriverInfo->SetBaseChunkName(c_CPOS);
				else if (pDriverInfo->GetBaseChunkName() == c_PPSP)
					pDriverInfo->SetBaseChunkName(c_CPSP);
				else if (pDriverInfo->GetBaseChunkName() == c_PORI)
					pDriverInfo->SetBaseChunkName(c_CORI);
				else if (pDriverInfo->GetBaseChunkName() == c_POSP)
					pDriverInfo->SetBaseChunkName(c_COSP);
				else if (pDriverInfo->GetBaseChunkName() == c_PDAF)
					pDriverInfo->SetBaseChunkName(c_CDAF);
			}
			io_ChtrList.m_Items.push_back(chtr_data);
		}

		// After converting, clear out the prop list so there is no more attempt to save them.
		props_data.Clear();
		return true;
	}

	return false;
}


//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  propDocumentChunk::Read(chReader &i_Reader,
				   chDefs::Version i_Version,
				   chDefs::Size i_Size)
{
	propDataParser::ReadData( i_Reader, i_Version, i_Size, m_DataList );

	if (this->m_bActive)
	{
		// If we are converting props to charaters, then we shouldn't
		// add the props to the ObjectMgr, just hold that data.
		if (!c_bConvertingProps)
		{
			DBG_ASSERT0(propObjectMgr::GetNumObjects() == 0, "Read() should be called after Clear(), use Import() to append items");
			propObjectMgr::SetData( m_DataList );
			propDialogUtil::UpdateListDialog();
		}
	}

	m_bDirty = false;
}

