#include "StdAfx.h"
#include "LODDialog.h"

#include "lodLevel.hpp"

//	tool includes
#include "cam3dMgr.hpp"
#include "docCustomDocumentMgr.hpp"
#include "muiFileDialogUtils.hpp"
#include "tmaCustomDocHandler.hpp"
#include "tmaDialogMemory.hpp"


using namespace LODStudio;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
LODDialog::LODDialog(lod3dData& i_Data)
:	m_Data( i_Data ),
	m_bDataChanged( false )
{
	m_bDisableNotify = true;

	InitializeComponent();

	SetupControls();

	// Dialog memory remembers size, location, visiblity of dialog
	m_pMemory = new tmaDialogMemory( this );

	m_bDisableNotify = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::button_open_Click(System::Object *  sender, System::EventArgs *  e)
{
	tmaCustomDocHandler::Open();

	SetupControls();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::button_save_Click(System::Object *  sender, System::EventArgs *  e)
{
	SetupData();

	tmaCustomDocHandler::Save();

	const fsLocator& filename = docCustomDocumentMgr::GetFilename();
	m_Data.m_LODFilename = filename;

	m_Data.m_bReloadObject = true;

	if ( m_bDataChanged )
	{
		ResetControlDataChanged();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::button_saveas_Click(System::Object *  sender, System::EventArgs *  e)
{
	if ( m_bDataChanged )
	{
		SetupData();
		ResetControlDataChanged();
	}

	tmaCustomDocHandler::SaveAs();

	const fsLocator& filename = docCustomDocumentMgr::GetFilename();
	m_Data.m_LODFilename = filename;
	//m_Data.m_bReloadData = true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::button_LOD1setcurrent_Click(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	float newvalue;
	newvalue = (cam3dMgr::GetCamera().GetTarget() - cam3dMgr::GetCamera().GetPosition()).Length();
	m_Data.m_LODLevels[1].m_fStartDistance  = newvalue;
	floatEdit_LOD1->Value = newvalue;

	SetControlDataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::button_LOD2setcurrent_Click(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	float newvalue;
	newvalue = (cam3dMgr::GetCamera().GetTarget() - cam3dMgr::GetCamera().GetPosition()).Length();
	m_Data.m_LODLevels[2].m_fStartDistance  = newvalue;
	floatEdit_LOD2->Value = newvalue;

	SetControlDataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::radioButton_LOD0default_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlDataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::radioButton_LOD1default_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlDataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::radioButton_LOD2default_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlDataChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::fileChooser_LOD0_ValueChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlFilenameChanged();
	SetupData();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::fileChooser_LOD1_ValueChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlFilenameChanged();
	SetupData();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::fileChooser_LOD2_ValueChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlFilenameChanged();
	SetupData();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::floatEdit_LOD1_ValueChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlDataChanged();
	SetupData();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void LODDialog::floatEdit_LOD2_ValueChanged(System::Object *  sender, System::EventArgs *  e)
{
	if (m_bDisableNotify)
		return;

	SetControlDataChanged();
	SetupData();
}

//----------------------------------------------------------------------------
//	set that a control component changed
//----------------------------------------------------------------------------
void LODDialog::SetControlDataChanged()
{
	m_bDataChanged = true;
	button_save->Enabled = true;

	m_Data.m_bReloadDataOnly = true;
}

//----------------------------------------------------------------------------
//	reset that a control component changed
//----------------------------------------------------------------------------
void LODDialog::ResetControlDataChanged()
{
	m_bDataChanged = false;
	button_save->Enabled = false;
}

//----------------------------------------------------------------------------
//	set that a control component changed
//----------------------------------------------------------------------------
void LODDialog::SetControlFilenameChanged()
{
	m_bDataChanged = true;
	button_save->Enabled = true;
}

//----------------------------------------------------------------------------
//	reset that a control component changed
//----------------------------------------------------------------------------
void LODDialog::ResetControlFilenameChanged()
{
	m_bDataChanged = false;
	button_save->Enabled = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void LODDialog::SetupControls()
{
	m_bDisableNotify = true;

	radioButton_LOD0default->set_Checked( m_Data.m_LODLevels[0].m_bEditorDefault );
	fileChooser_LOD0->set_Fullpath( m_Data.m_LODLevels[0].m_ModelFile.c_str() );

	radioButton_LOD1default->set_Checked( m_Data.m_LODLevels[1].m_bEditorDefault );
	floatEdit_LOD1->Value = m_Data.m_LODLevels[1].m_fStartDistance;
	fileChooser_LOD1->set_Fullpath( m_Data.m_LODLevels[1].m_ModelFile.c_str() );

	radioButton_LOD2default->set_Checked( m_Data.m_LODLevels[2].m_bEditorDefault );
	floatEdit_LOD2->Value = m_Data.m_LODLevels[2].m_fStartDistance;
	fileChooser_LOD2->set_Fullpath( m_Data.m_LODLevels[2].m_ModelFile.c_str() );

	//fileChooser_LOD0->Filter = "Object Files (*.edf)|*.edf|All files (*.*)|*.*"

	//DBG_LOG1( "LODDialog path 0 (%s)", m_Data.m_LODLevels[0].m_ModelFile.c_str() );
	//DBG_LOG1( "LODDialog path 1 (%s)", m_Data.m_LODLevels[1].m_ModelFile.c_str() );
	//DBG_LOG1( "LODDialog path 2 (%s)", m_Data.m_LODLevels[2].m_ModelFile.c_str() );

	m_bDisableNotify = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void LODDialog::SetupData()
{
	m_bDisableNotify = true;

	m_Data.m_NumberOfLevels = 3;
	m_Data.m_LODLevels.resize( m_Data.m_NumberOfLevels );

	m_Data.m_LODLevels[0].m_bEditorDefault	= radioButton_LOD0default->get_Checked();
	m_Data.m_LODLevels[0].m_fStartDistance  = 0.0f;
	tmaManagedStringUtils::ManagedStringToStdString( fileChooser_LOD0->get_Fullpath(), m_Data.m_LODLevels[0].m_ModelFile );

	m_Data.m_LODLevels[1].m_bEditorDefault	= radioButton_LOD1default->get_Checked();
	m_Data.m_LODLevels[1].m_fStartDistance  = (float)floatEdit_LOD1->Value;
	tmaManagedStringUtils::ManagedStringToStdString( fileChooser_LOD1->get_Fullpath(), m_Data.m_LODLevels[1].m_ModelFile );

	m_Data.m_LODLevels[2].m_bEditorDefault	= radioButton_LOD2default->get_Checked();
	m_Data.m_LODLevels[2].m_fStartDistance  = (float)floatEdit_LOD2->Value;
	tmaManagedStringUtils::ManagedStringToStdString( fileChooser_LOD2->get_Fullpath(), m_Data.m_LODLevels[2].m_ModelFile );

	//DBG_LOG1( "LODDialog path 0 (%s)", m_Data.m_LODLevels[0].m_ModelFile.c_str() );
	//DBG_LOG1( "LODDialog path 1 (%s)", m_Data.m_LODLevels[1].m_ModelFile.c_str() );
	//DBG_LOG1( "LODDialog path 2 (%s)", m_Data.m_LODLevels[2].m_ModelFile.c_str() );

	//m_Data.m_bReloadDataOnly = true;

	m_bDisableNotify = false;
}


