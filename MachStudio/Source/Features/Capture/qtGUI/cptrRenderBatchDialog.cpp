/*****************************************************************************
**	cptrRenderBatchDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/qtGUI/cptrRenderBatchDialog.hpp"

#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrRenderBatchDialogUtil.hpp"
#include "Features/Capture/cptrRenderBatchDataParser.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#undef CreateFile
#undef DeleteFile
#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#ifdef USE_QT

#include <QCheckBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
	QString to_qstring(const std::string& i_String)
	{
		return QString::fromStdString(i_String);
	}

	QPushButton* add_button(QWidget* i_pParent, QBoxLayout* i_pLayout, const char* i_pText)
	{
		QPushButton* pButton = new QPushButton(i_pText, i_pParent);
		pButton->setAutoDefault(false);
		i_pLayout->addWidget(pButton);
		return pButton;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderBatchDialog::cptrRenderBatchDialog(cptrRenderBatchData& i_Data, QWidget* parent)
:	QDialog(parent),
	m_Data(i_Data)
{
	this->setWindowTitle( "Render Batch" );

	QVBoxLayout* bSizer_Main = new QVBoxLayout( this );
	bSizer_Main->addWidget( new QLabel( "Add scenes and save as a batch.  Saved batches can be loaded later.", this ) );

	m_checkList_Scenes = new QListWidget( this );
	m_checkList_Scenes->setSelectionMode( QAbstractItemView::ExtendedSelection );
	bSizer_Main->addWidget( m_checkList_Scenes, 1 );

	QHBoxLayout* bSizer_SceneButtons = new QHBoxLayout();
	bSizer_SceneButtons->addStretch( 1 );
	QPushButton* pAddScene = add_button( this, bSizer_SceneButtons, "Add Scene" );
	m_button_AddCurrent = add_button( this, bSizer_SceneButtons, "Add Current" );
	QPushButton* pDeleteScene = add_button( this, bSizer_SceneButtons, "Del Scene" );
	QPushButton* pMoveUp = add_button( this, bSizer_SceneButtons, "Move Up" );
	QPushButton* pMoveDown = add_button( this, bSizer_SceneButtons, "Move Down" );
	bSizer_SceneButtons->addStretch( 1 );
	bSizer_Main->addLayout( bSizer_SceneButtons );

	m_checkBox_SkipScene = new QCheckBox( "Skip Scene When Error Occurs", this );
	bSizer_Main->addWidget( m_checkBox_SkipScene, 0, Qt::AlignHCenter );

	QGroupBox* pBatchFile = new QGroupBox( "Batch File", this );
	QVBoxLayout* sbSizer_BatchFile = new QVBoxLayout( pBatchFile );
	m_textCtrl_BatchFile = new QLineEdit( pBatchFile );
	sbSizer_BatchFile->addWidget( m_textCtrl_BatchFile );
	QHBoxLayout* gSizer_BatchFileButtons = new QHBoxLayout();
	gSizer_BatchFileButtons->addStretch( 1 );
	QPushButton* pLoadBatch = add_button( pBatchFile, gSizer_BatchFileButtons, "Load Batch" );
	QPushButton* pSaveBatch = add_button( pBatchFile, gSizer_BatchFileButtons, "Save Batch" );
	gSizer_BatchFileButtons->addStretch( 1 );
	sbSizer_BatchFile->addLayout( gSizer_BatchFileButtons );
	bSizer_Main->addWidget( pBatchFile );

	m_button_Continue = new QPushButton( "Render", this );
	m_button_Continue->setAutoDefault( false );
	bSizer_Main->addWidget( m_button_Continue, 0, Qt::AlignHCenter );

	QObject::connect( m_checkList_Scenes, &QListWidget::itemChanged, [this](QListWidgetItem*) { set_continue_button(); } );
	QObject::connect( m_checkList_Scenes, &QListWidget::itemDoubleClicked, [this](QListWidgetItem*) { set_continue_button(); } );
	QObject::connect( pAddScene, &QPushButton::clicked, [this]() { button_AddScene_OnButtonClick(); } );
	QObject::connect( m_button_AddCurrent, &QPushButton::clicked, [this]() { button_AddCurrent_OnButtonClick(); } );
	QObject::connect( pDeleteScene, &QPushButton::clicked, [this]() { button_DeleteScene_OnButtonClick(); } );
	QObject::connect( pMoveUp, &QPushButton::clicked, [this]() { button_MoveUp_OnButtonClick(); } );
	QObject::connect( pMoveDown, &QPushButton::clicked, [this]() { button_MoveDown_OnButtonClick(); } );
	QObject::connect( pLoadBatch, &QPushButton::clicked, [this]() { button_LoadBatch_OnButtonClick(); } );
	QObject::connect( pSaveBatch, &QPushButton::clicked, [this]() { button_SaveBatch_OnButtonClick(); } );
	QObject::connect( m_button_Continue, &QPushButton::clicked, [this]() { button_Continue_OnButtonClick(); } );

	m_Data.m_BatchAborted = true;
	m_checkBox_SkipScene->setChecked(true);
	set_continue_button();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::set_continue_button()
{
	m_button_Continue->setEnabled(   (m_Data.m_BatchFilename.GetValue().GetNumNames() > 0)
								  || (m_checkList_Scenes->count() > 0) );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::add_scene(const std::string& i_Filename, bool i_bChecked)
{
	QListWidgetItem* pItem = new QListWidgetItem( to_qstring(i_Filename), m_checkList_Scenes );
	pItem->setFlags( pItem->flags() | Qt::ItemIsUserCheckable );
	pItem->setCheckState( i_bChecked ? Qt::Checked : Qt::Unchecked );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::ReadFile(const fsLocator &i_Loc)
{
	try
	{
		cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
		cptrRenderBatchDataUtil::ReadData( i_Loc, data );
		m_Data = data;
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Error reading batch data, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Batch Error", guiMessageBox::e_OKOnly);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::SaveFile(const fsLocator &i_Loc)
{
	SetDataValues();

	if (fsFileUtil::FileExists(i_Loc))
	{
		fsFileUtil::DeleteFile(i_Loc);
	}
	fsFileUtil::CreateFile(i_Loc);
	try
	{
		cptrRenderBatchDataUtil::WriteData(i_Loc,m_Data);
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Error writing batch data, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Batch Error", guiMessageBox::e_OKOnly);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::UpdateFileDirectory( fsLocator& i_dir )
{
	std::string sdir;
	fsFileUtil::LocatorToUnicodeString(i_dir, sdir);
	m_textCtrl_BatchFile->setText( to_qstring(sdir) );

	cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
	data.m_BatchFilename.SetValue(i_dir);
	m_Data.m_BatchFilename.SetValue(i_dir);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::SetDataValues()
{
	int numitems = m_checkList_Scenes->count();
	m_Data.m_Scenes.clear();
	m_Data.m_Scenes.resize( numitems );
	for ( int i = 0 ; i < numitems ; ++i )
	{
		QListWidgetItem* pItem = m_checkList_Scenes->item(i);
		fsLocator dir;
		fsFileUtil::UnicodeStringToLocator( pItem->text().toStdString(), dir);
		m_Data.m_Scenes[i].m_Filename.SetValue(dir);
		m_Data.m_Scenes[i].m_bChecked.SetValue(pItem->checkState() == Qt::Checked);
	}
	m_Data.m_SkipDialog = m_checkBox_SkipScene->isChecked();
	cptrRenderBatchDataUtil::UpdateData(m_Data);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::SetComponentValues()
{
	m_textCtrl_BatchFile->clear();
	m_checkList_Scenes->clear();
	for ( int i = 0 ; i < m_Data.m_Scenes.size() ; ++i )
	{
		std::string filename;
		fsFileUtil::LocatorToUnicodeString(m_Data.m_Scenes[i].m_Filename.GetValue(), filename);
		add_scene(filename, m_Data.m_Scenes[i].m_bChecked.GetValue());
	}
	m_checkBox_SkipScene->setChecked(m_Data.m_SkipDialog.GetValue());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::closeEvent( QCloseEvent* i_pEvent )
{
	SetDataValues();
	cptrRenderBatchDialogUtil::Hide();
	cptrRenderBatchDialogUtil::SetDialogClosing( true );
	if (m_Data.m_BatchAborted.GetValue())
		cptrRenderBatchDialogUtil::SetDialogExitting(true);
	QDialog::closeEvent( i_pEvent );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_AddScene_OnButtonClick()
{
	fsLocator initial_dir;
	if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
	{
		initial_dir = docSingleDocumentMgr::GetFilename();
		initial_dir.Pop();
	}
	std::string init_dir;
	fsFileUtil::LocatorToUnicodeString( initial_dir, init_dir );

	const QStringList paths = QFileDialog::getOpenFileNames(this, "Open File", to_qstring(init_dir),
		"scene files (*.mab);;All files (*.*)");
	for (const QString& path : paths)
	{
		fsLocator file_loc;
		fsFileUtil::UnicodeStringToLocator(QDir::toNativeSeparators(path).toStdString(), file_loc);
		std::string filename;
		fsFileUtil::LocatorToUnicodeString(file_loc, filename);
		add_scene(filename, true);
	}

	set_continue_button();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_MoveUp_OnButtonClick()
{
	int selection = m_checkList_Scenes->currentRow();
	if (selection <= 0)
		return;

	QListWidgetItem* pItem = m_checkList_Scenes->takeItem(selection);
	m_checkList_Scenes->insertItem(selection - 1, pItem);
	m_checkList_Scenes->clearSelection();
	m_checkList_Scenes->setCurrentRow(selection - 1);

	set_continue_button();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_MoveDown_OnButtonClick()
{
	int selection = m_checkList_Scenes->currentRow();
	if (selection < 0 || selection >= m_checkList_Scenes->count() - 1)
		return;

	QListWidgetItem* pItem = m_checkList_Scenes->takeItem(selection);
	m_checkList_Scenes->insertItem(selection + 1, pItem);
	m_checkList_Scenes->clearSelection();
	m_checkList_Scenes->setCurrentRow(selection + 1);

	set_continue_button();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_AddCurrent_OnButtonClick()
{
	if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
	{
		std::string filename;
		fsFileUtil::LocatorToUnicodeString(docSingleDocumentMgr::GetFilename(), filename);
		add_scene(filename, true);
	}
	set_continue_button();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_DeleteScene_OnButtonClick()
{
	qDeleteAll( m_checkList_Scenes->selectedItems() );
	set_continue_button();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_LoadBatch_OnButtonClick()
{
	std::string filter = "scene batch files (*.scb)|*.scb|All files (*.*)|*.*";
	fsLocator file_loc;
	fsLocator initial_dir;
	if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		ReadFile(file_loc);
		SetComponentValues();
		UpdateFileDirectory( file_loc );
		set_continue_button();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_SaveBatch_OnButtonClick()
{
	std::string filter = "scene batch files (*.scb)|*.scb|All files (*.*)|*.*";
	fsLocator file_loc;
	if (guiFileDialogUtils::GetSaveFileName(filter, file_loc))
	{
		DBG_LOG( "Saving batch file " << file_loc.GetLastName() );
		SaveFile(file_loc);
		UpdateFileDirectory( file_loc );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBatchDialog::button_Continue_OnButtonClick()
{
	cptrRenderBatchDialogUtil::Hide();
	m_Data.m_BatchAborted = false;
	this->close();
}

#endif
