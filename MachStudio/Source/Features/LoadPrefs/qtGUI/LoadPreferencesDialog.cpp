/*****************************************************************************
**	LoadPreferencesDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/LoadPrefs/qtGUI/LoadPreferencesDialog.hpp"

#include "Core/Fs/fsFileUtil.hpp"


#ifdef USE_QT

#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
LoadPreferencesDialog* LoadPreferencesDialog::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
LoadPreferencesDialog::LoadPreferencesDialog( QWidget* parent )
: QDialog( parent )
{
	this->setWindowTitle( "Load Preferences" );

	QVBoxLayout* bSizer1 = new QVBoxLayout( this );

	QTabWidget* pNotebook = new QTabWidget( this );

	// The properties are built into a scrolled page
	QScrollArea* pScroll = new QScrollArea( pNotebook );
	pScroll->setWidgetResizable( true );
	pScroll->setFrameShape( QFrame::NoFrame );
	m_panel_LoadPrefs = new QWidget();
	pScroll->setWidget( m_panel_LoadPrefs );
	pNotebook->addTab( pScroll, "Load Prefs" );

	QWidget* pMissingTextures = new QWidget( pNotebook );
	QVBoxLayout* bSizer2 = new QVBoxLayout( pMissingTextures );
	bSizer2->addWidget( new QLabel( "Textures skipped when loading:", pMissingTextures ) );
	m_listBox1 = new QListWidget( pMissingTextures );
	bSizer2->addWidget( m_listBox1, 1 );
	pNotebook->addTab( pMissingTextures, "Missing Textures" );

	bSizer1->addWidget( pNotebook, 1 );

	this->resize( 382, 417 );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
LoadPreferencesDialog::~LoadPreferencesDialog()
{
	if (LoadPreferencesDialog::Instance == this)
		LoadPreferencesDialog::Instance = NULL;
}

//--------------------------------------------------------------------
// Return access to tab page that will contain load prefs properties
//--------------------------------------------------------------------
QWidget* LoadPreferencesDialog::GetTabPageMain()
{
	return m_panel_LoadPrefs;
}

//--------------------------------------------------------------------
// Fill in list box with filenames from the textures
// that were skipped
//--------------------------------------------------------------------
void LoadPreferencesDialog::SetMissingTextureList(const std::set<fsLocator>& i_Set)
{
	m_listBox1->clear();
	std::set<fsLocator>::const_iterator it;
	for (it = i_Set.begin(); it != i_Set.end(); ++it)
	{
		itString filename;
		fsFileUtil::LocatorToUnicodeString(*it, filename);
		m_listBox1->addItem( QString::fromWCharArray(filename.GetString()) );
	}
}


#endif // USE_QT
