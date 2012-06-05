/*****************************************************************************
**	dirltActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltActualOperations.hpp"

#include "dirltDialogUtil.hpp"
#include "dirltDocumentChunk.hpp"

#include "cmpsCompassMgr.hpp"


//============================================================================
//============================================================================
namespace
{

}	// end of namespace

//--------------------------------------------------------------------
//  Add new dir light to world
//--------------------------------------------------------------------
int  dirltActualOperations::AddObject(const dirltScriptData& i_Data)
{
	int index = dirltObjectMgr::AddObject(i_Data);
	dirltDialogUtil::UpdateListDialog();
	dirltDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in dirltOperations
	dirltObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete dir light with given index
//--------------------------------------------------------------------
void  dirltActualOperations::DeleteObject(int i_Index)
{
	dirltObjectMgr::DeleteObject(i_Index);
	dirltDialogUtil::UpdateListDialog();
	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual base properties
//--------------------------------------------------------------------
void dirltActualOperations::SetBaseData(int i_Index, const dirltData& i_Data)
{
	dirltObjectMgr::SetBaseData(i_Index, i_Data);
	dirltDialogUtil::UpdateListDialog();				// because of name
	dirltDialogUtil::UpdateLightData(i_Index, i_Data);

	cmpsCompassMgr::SetSelectedObjectChanged( true );

	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual dir light properties
//--------------------------------------------------------------------
void dirltActualOperations::SetData(int i_Index, const dirltScriptData& i_Data)
{
	dirltObjectMgr::SetScriptData(i_Index, i_Data);
	dirltDialogUtil::UpdateListDialog();				// because of name
	dirltDialogUtil::UpdateLightData(i_Index, i_Data);

	cmpsCompassMgr::SetSelectedObjectChanged( true );

	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void dirltActualOperations::ChangeDriverData(int i_Index, const dirltScriptData& i_Data)
{
	// no need to notify dirltObjectMgr if there is no undo, 
	// this message is coming from the object directly

	dirltDialogUtil::UpdateLightData(i_Index, i_Data);

	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Set Name
//--------------------------------------------------------------------
void dirltActualOperations::SetName(int i_Index, const nameString& i_Name)
{
	// first get the latest data, then set the position
	//
	dirltData data = dirltObjectMgr::GetBaseData(i_Index);
	data.m_Name = i_Name;

	dirltObjectMgr::SetBaseData(i_Index, data);

	// we don’t need to do the first call since the name isn’t changing.
	//
	dirltDialogUtil::UpdateListDialog();				// because of name
	dirltDialogUtil::UpdateLightData(i_Index, data);

	cmpsCompassMgr::SetSelectedObjectChanged( true );
	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// SetPosition
//--------------------------------------------------------------------
void dirltActualOperations::SetPosition(int i_Index, const maPoint3d& i_Position)
{
	// first get the latest data, then set the position
	//
	dirltData data = dirltObjectMgr::GetBaseData(i_Index);
	data.m_Position = i_Position;

	dirltObjectMgr::SetBaseData(i_Index, data);

	// we don’t need to do the first call since the name isn’t changing.
	//
	//dirltDialogUtil::UpdateListDialog();			// because of name
	dirltDialogUtil::UpdateLightData(i_Index, data);

	cmpsCompassMgr::SetSelectedObjectChanged( true );
	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// SetOrientation
//--------------------------------------------------------------------
void dirltActualOperations::SetOrientation(int i_Index, const maRotation& i_Orientation)
{
	// first get the latest data, then set the Orientation
	//
	dirltData data = dirltObjectMgr::GetBaseData(i_Index);

	maVector3d dir( 0.0f, 1.0f, 0.0f );
	i_Orientation.RotateVector( dir );
	
	data.m_Direction	= dir;

	dirltObjectMgr::SetBaseData(i_Index, data);

	// we don’t need to do the first call since the name isn’t changing.
	//
	//dirltDialogUtil::UpdateListDialog();			// because of name
	dirltDialogUtil::UpdateLightData(i_Index, data);

	cmpsCompassMgr::SetSelectedObjectChanged( true );
	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// SetColor
//--------------------------------------------------------------------
void dirltActualOperations::SetColor(int i_Index, const maFloatRGBA& i_Color)
{
	// first get the latest data, then set the Color
	//
	dirltData data = dirltObjectMgr::GetBaseData(i_Index);
	data.m_Color = i_Color;

	dirltObjectMgr::SetBaseData(i_Index, data);

	// we don’t need to do the first call since the name isn’t changing.
	//
	//dirltDialogUtil::UpdateListDialog();			// because of name
	dirltDialogUtil::UpdateLightData(i_Index, data);

	cmpsCompassMgr::SetSelectedObjectChanged( true );
	dirltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// SetEnabled
//--------------------------------------------------------------------
void dirltActualOperations::SetEnabled(int i_Index, const bool i_Enabled)
{
	// first get the latest data, then set the Enabled
	//
	dirltData data = dirltObjectMgr::GetBaseData(i_Index);
	data.m_Enabled = i_Enabled;

	dirltObjectMgr::SetBaseData(i_Index, data);

	// we don’t need to do the first call since the name isn’t changing.
	//
	//dirltDialogUtil::UpdateListDialog();			// because of name
	dirltDialogUtil::UpdateLightData(i_Index, data);

	cmpsCompassMgr::SetSelectedObjectChanged( true );
	dirltDocumentChunk::ActiveDataChanged();
}
