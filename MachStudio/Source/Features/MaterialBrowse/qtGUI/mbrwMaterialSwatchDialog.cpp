/*****************************************************************************
**	mbrwMaterialSwatchDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/MaterialBrowse/qtGUI/mbrwMaterialSwatchDialog.hpp"

#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"
#include "Features/MaterialBrowse/mbrwPaintUtil.hpp"
#include "Support/mtrl/mtrlIconUtil.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Gf/gfDirectoryCategories.hpp"
#include "Core/Gf/gfFileEnum.hpp"
#include "Core/Gf/gfFileX.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mtr/mtrThumbnailData.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include <set>

#ifdef USE_QT

#include <QComboBox>
#include <QDrag>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMimeData>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

#include <functional>

//============================================================================
//============================================================================
namespace
{
	const int c_IconWidth = 128;  
	const int c_MaxHistory = 32;
	const char* c_MaterialDirCategory = "Materials";

	//--------------------------------------------------------------------
	// Icon directory, with a trailing separator
	//--------------------------------------------------------------------
	QString get_icon_directory()
	{
		itString wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return QString::fromWCharArray(wide_dir.GetString()) + "\\";
	}

	//--------------------------------------------------------------------
	// Image button with normal, disabled, hover and pressed images
	//--------------------------------------------------------------------
	QToolButton* create_image_button(QWidget* i_pParent, const QString& i_IconDir,
									 const char* i_pName, const char* i_pToolTip)
	{
		const QString base = i_IconDir + i_pName;
		QIcon icon(base + ".PNG");
		icon.addPixmap(QPixmap(base + "_disabled.PNG"), QIcon::Disabled);
		icon.addPixmap(QPixmap(base + "_highlight.PNG"), QIcon::Active);
		icon.addPixmap(QPixmap(base + "_pressed.PNG"), QIcon::Selected);

		QToolButton* pButton = new QToolButton(i_pParent);
		pButton->setIcon(icon);
		pButton->setIconSize(QSize(30, 30));
		pButton->setAutoRaise(true);
		pButton->setToolTip(i_pToolTip);
		return pButton;
	}

	//--------------------------------------------------------------------
	// List widget that starts a file drag of its own
	//--------------------------------------------------------------------
	class MaterialListWidget : public QListWidget
	{
	public:
		MaterialListWidget(QWidget* i_pParent) : QListWidget(i_pParent) {}
		std::function<void ()> m_StartDrag;
	protected:
		void startDrag(Qt::DropActions) override
		{
			if (m_StartDrag)
				m_StartDrag();
		}
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	fsLocator to_locator(const QString& i_Path)
	{
		fsLocator loc;
		fsFileUtil::UnicodeStringToLocator(itString(i_Path.toStdWString().c_str()), loc);
		return loc;
	}

	QString to_qstring(const itString& i_String)
	{
		return QString::fromWCharArray(i_String.GetString());
	}

		
	//--------------------------------------------------------------------
	// MaterialBrowseTarget searches for material library files.
	// It gathers all of the mtl filenames in the current directory
	// into a file list and also enumerates the subdirectories that
	// contain other mtl files.
	//--------------------------------------------------------------------
	class MaterialBrowseTarget : public fsFileEnum::EnumTarget
	{
		public:
			//
			MaterialBrowseTarget(const fsLocator& i_RootDir)
				: m_RootDir(i_RootDir) {}

			//
			virtual bool Notify(const fsLocator& i_Directory, const fsLocator& i_File)
			{
				std::string ext;
				itStringUtil::GetExtension( i_File.GetLastName(), ext );
				if (ext == "MTL" || ext == "mtl")
				{
					fsLocator Locator = i_Directory;

					// Files in root directories go into file list,
					// sub directories go into set to get unique sorted list
					if (i_Directory == m_RootDir)
					{
						Locator.Push(i_File);
						m_FileList.push_back(Locator);
					}
					else if (i_Directory.GetNumNames() > m_RootDir.GetNumNames()) // should be true, just making sure
					{
						// these three lines gather all subdirs that contain mtl files
						//fsLocator relative_dir(i_Directory);
						//relative_dir.RemoveBefore(m_RootDir.GetNumNames());
						//m_SubDirs.insert(relative_dir);
						
						// these lines gather only one level of sub directories and only
						// those directories that have mtl files somewhere under them
						itString subdir = i_Directory.GetName( m_RootDir.GetNumNames() );
						m_SubDirs.insert(fsLocator(subdir));
					}
				}
				return true;
			}

		public:
			fsLocator m_RootDir;
			fsFileEnum::fsFileList	m_FileList;
			std::set<fsLocator> m_SubDirs;
	};

	//--------------------------------------------------------------------
	// A target that determines if a subdirectory should be displayed
	// in the material browser with a folder icon. Its test
	// is whether a directory that does not begin with "."
	// exists. This target should be used with a directory enumeration,
	// the existence of a .mtl fil within the directory should be handled
	// with the MaterialBrowseTarget target.
	//--------------------------------------------------------------------
	class SubDirectoryTarget : public fsFileEnum::EnumTarget
	{
		public:
			//
			SubDirectoryTarget() : m_bHasSubdirs(false) {}

			//
			virtual bool Notify(const fsLocator& i_Directory, const fsLocator& i_File)
			{
				if (i_File.GetNumNames() > 0)
				{
					itString dir_name = i_File.GetLastName();
					if (dir_name[0] != '.')
					{
						m_bHasSubdirs = true;
						return false; // abort enumeration, we found what we wanted
					}
				}
				return true;
			}

		public:
			bool m_bHasSubdirs;
	};

}	// end of namespace

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
mbrwMaterialSwatchDialog* mbrwMaterialSwatchDialog::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mbrwMaterialSwatchDialog::mbrwMaterialSwatchDialog( QWidget* parent,
													const char* i_Title )
:	QWidget( parent )
{
	const QString icon_dir = get_icon_directory();

	QVBoxLayout* bSizer1 = new QVBoxLayout( this );

	QHBoxLayout* bSizer_MaterialBrowse = new QHBoxLayout();
	bSizer_MaterialBrowse->setSpacing( 0 );
	QToolButton* pBrowse = create_image_button( this, icon_dir, "matbrowse-open", "Browse to directory" );
	bSizer_MaterialBrowse->addWidget( pBrowse );
	QToolButton* pHome = create_image_button( this, icon_dir, "matbrowse-home", "Home directory" );
	bSizer_MaterialBrowse->addWidget( pHome );
	QToolButton* pRefresh = create_image_button( this, icon_dir, "matbrowse-refresh", "Refresh Icons" );
	bSizer_MaterialBrowse->addWidget( pRefresh );
	m_button_Back = create_image_button( this, icon_dir, "matbrowse-back", "Previous Directory" );
	bSizer_MaterialBrowse->addWidget( m_button_Back );
	m_button_Forward = create_image_button( this, icon_dir, "matbrowse-forward", "Next Directory" );
	bSizer_MaterialBrowse->addWidget( m_button_Forward );
	QToolButton* pUp = create_image_button( this, icon_dir, "matbrowse-up", "Parent Directory" );
	bSizer_MaterialBrowse->addWidget( pUp );
	m_button_Paint = create_image_button( this, icon_dir, "matbrowse-paint", "Apply to selected material" );
	bSizer_MaterialBrowse->addWidget( m_button_Paint );
	QToolButton* pNewFolder = create_image_button( this, icon_dir, "matbrowse-new", "Create a new folder" );
	bSizer_MaterialBrowse->addWidget( pNewFolder );
	bSizer_MaterialBrowse->addStretch( 1 );

	m_choice_view = new QComboBox( this );
	m_choice_view->addItem( "Thumbnails" );
	m_choice_view->addItem( "List" );
	bSizer_MaterialBrowse->addWidget( m_choice_view );
	bSizer1->addLayout( bSizer_MaterialBrowse );

	MaterialListWidget* pList = new MaterialListWidget( this );
	m_listCtrl_Icons = pList;
	m_listCtrl_Icons->setSelectionMode( QAbstractItemView::SingleSelection );
	m_listCtrl_Icons->setDragEnabled( true );
	m_listCtrl_Icons->setStyleSheet( "QListWidget { background: black; color: white; }" );
	bSizer1->addWidget( m_listCtrl_Icons, 1 );
	OnChoice();

	pList->m_StartDrag = [this]() { listCtrl_beginDrag(); };
	QObject::connect( pBrowse, &QToolButton::clicked, [this]() { buttonBrowse_Click(); } );
	QObject::connect( pHome, &QToolButton::clicked, [this]() { buttonHome_Click(); } );
	QObject::connect( pRefresh, &QToolButton::clicked, [this]() { buttonRefresh_Click(); } );
	QObject::connect( m_button_Back, &QToolButton::clicked, [this]() { buttonBack_Click(); } );
	QObject::connect( m_button_Forward, &QToolButton::clicked, [this]() { buttonForward_Click(); } );
	QObject::connect( pUp, &QToolButton::clicked, [this]() { buttonUp_Click(); } );
	QObject::connect( m_button_Paint, &QToolButton::clicked, [this]() { buttonPaint_Click(); } );
	QObject::connect( pNewFolder, &QToolButton::clicked, [this]() { buttonNewFolder_Click(); } );
	QObject::connect( m_listCtrl_Icons, &QListWidget::itemActivated, [this](QListWidgetItem* i_pItem) { listCtrl_doubleClick(i_pItem); } );
	QObject::connect( m_listCtrl_Icons, &QListWidget::itemSelectionChanged, [this]() { enable_buttons(); } );
	QObject::connect( m_choice_view, QOverload<int>::of(&QComboBox::activated), [this](int) { OnChoice(); } );

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(2).Bottom().BestSize(563, 350));

	fsLocator init_dir = mbrwDialogUtil::GetInitialDirectory();
	browse_directory( init_dir );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mbrwMaterialSwatchDialog::~mbrwMaterialSwatchDialog()
{
	if (mbrwMaterialSwatchDialog::Instance == this)
		mbrwMaterialSwatchDialog::Instance = NULL;
}

//--------------------------------------------------------------------
// Set directory for material browsing
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::SetInitialDirectory(const fsLocator& i_InitDir)
{
	browse_directory(i_InitDir);
}

//--------------------------------------------------------------------
// Refresh Icons from the current directory, called when
// new materials are exported.
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::RefreshIcons()
{
	if (m_CurrentMaterialDir.GetNumNames() > 0)
	{
		browse_directory( m_CurrentMaterialDir );
	}
}

//--------------------------------------------------------------------
// event handlers
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::buttonHome_Click()
{
	fsLocator home_dir = mbrwDialogUtil::GetInitialDirectory();
	browse_directory( home_dir );
}

void mbrwMaterialSwatchDialog::buttonRefresh_Click()
{
	// This regenerate thumbnail icons for the materials files that are outdated:
	regenerate_icons();

	// These lines reload the thumbnails into the browser:
	if (m_CurrentMaterialDir.GetNumNames() > 0)
		browse_directory( m_CurrentMaterialDir );
}

void mbrwMaterialSwatchDialog::buttonBack_Click()
{
	if (!m_PreviousDirectories.empty())
	{
		fsLocator prev_dir = m_PreviousDirectories.back();
		m_PreviousDirectories.pop_back();
		m_NextDirectories.push_back(m_CurrentMaterialDir);
		const bool bAddToHistory = false;
		browse_directory( prev_dir, bAddToHistory );
	}
}

void mbrwMaterialSwatchDialog::buttonForward_Click()
{
	if (!m_NextDirectories.empty())
	{
		fsLocator next_dir = m_NextDirectories.back();
		m_NextDirectories.pop_back();
		m_PreviousDirectories.push_back(m_CurrentMaterialDir);
		const bool bAddToHistory = false;
		browse_directory( next_dir, bAddToHistory );
	}
}

void mbrwMaterialSwatchDialog::buttonUp_Click()
{
	if (m_CurrentMaterialDir.GetNumNames() > 1)
	{
		fsLocator parent_dir = m_CurrentMaterialDir;
		parent_dir.Pop();
		browse_directory( parent_dir );
	}
}

void mbrwMaterialSwatchDialog::buttonPaint_Click()
{
	QListWidgetItem* pItem = m_listCtrl_Icons->currentItem();
	if (pItem && pItem->isSelected())
	{
		fsLocator mtl_loc = m_CurrentMaterialDir;
		mtl_loc.Push(itString(pItem->text().toStdWString().c_str()));
		mbrwPaintUtil::AssignMaterialFile(mtl_loc);
	}
}

void mbrwMaterialSwatchDialog::buttonNewFolder_Click()
{
	fsLocator init_dir;
	gfDirectoryCategories::GetCurDirectory(c_MaterialDirCategory, init_dir);
	itString defaultPath;
	fsFileUtil::LocatorToUnicodeString(init_dir, defaultPath);

	// The directory dialog has its own "new folder" button
	QString path = QFileDialog::getExistingDirectory(tqtSystem::g_pMainForm,
					   "Create a new directory", to_qstring(defaultPath));
	if (!path.isEmpty())
	{
		fsFileUtil::CreateDirectory(to_locator(path));
	}
}

void mbrwMaterialSwatchDialog::buttonBrowse_Click()
{
	fsLocator init_dir;
	gfDirectoryCategories::GetCurDirectory(c_MaterialDirCategory, init_dir);
	itString defaultPath;
	fsFileUtil::LocatorToUnicodeString(init_dir, defaultPath);

	QString path = QFileDialog::getExistingDirectory(tqtSystem::g_pMainForm,
					   "Browse material library directory", to_qstring(defaultPath));
	if (!path.isEmpty())
	{
		fsLocator material_dir = to_locator(path);
		browse_directory( material_dir );

		//bga - not sure about this, using the browse button to
		// define the "home" directory
		mbrwDialogUtil::SetInitialDirectory(material_dir);
	}
}

//--------------------------------------------------------------------
// Drag a material file to drop it on an object
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::listCtrl_beginDrag()
{
	QListWidgetItem* pItem = m_listCtrl_Icons->currentItem();
	if (pItem == NULL || m_CurrentMaterialDir.GetNumNames() == 0)
		return;

	// Check to see if this is really a mtl file or just a folder icon.
	const QString filename = pItem->text();
	if (filename.endsWith(".mtl", Qt::CaseInsensitive))
	{
		fsLocator fullpath = m_CurrentMaterialDir;
		fullpath.Push( itString(filename.toStdWString().c_str()) );
		itString itFilename;
		fsFileUtil::LocatorToUnicodeString(fullpath, itFilename);

		QMimeData* pData = new QMimeData();
		pData->setUrls( QList<QUrl>() << QUrl::fromLocalFile(to_qstring(itFilename)) );

		QDrag* pDrag = new QDrag(m_listCtrl_Icons);
		pDrag->setMimeData(pData);
		pDrag->setPixmap(pItem->icon().pixmap(64, 64));
		pDrag->exec(Qt::CopyAction);
	}
}

//--------------------------------------------------------------------
// double click on a folder item opens that folder
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::listCtrl_doubleClick( QListWidgetItem* i_pItem )
{
	const QString sub_dir_str = i_pItem->text();
	if ((m_CurrentMaterialDir.GetNumNames() > 0) &&
		(!sub_dir_str.isEmpty()))
	{
		// If it doesn't end with ".mtl" assume it is a folder icon
		if (!sub_dir_str.endsWith(".mtl", Qt::CaseInsensitive))
		{
			// Concatenate relative path to the root directory to get fullpath
			fsLocator new_dir = m_CurrentMaterialDir;
			new_dir.Push( itString(sub_dir_str.toStdWString().c_str()) );
			browse_directory( new_dir );
		}
	}
}

//--------------------------------------------------------------------
// Thumbnail or list view
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::OnChoice()
{
	if (m_choice_view->currentIndex() == 0)
	{
		m_listCtrl_Icons->setViewMode( QListView::IconMode );
		m_listCtrl_Icons->setIconSize( QSize(c_IconWidth, c_IconWidth) );
		m_listCtrl_Icons->setGridSize( QSize(c_IconWidth + 24, c_IconWidth + 32) );
	}
	else
	{
		m_listCtrl_Icons->setViewMode( QListView::ListMode );
		m_listCtrl_Icons->setIconSize( QSize(16, 16) );
		m_listCtrl_Icons->setGridSize( QSize() );
	}
	m_listCtrl_Icons->setResizeMode( QListView::Adjust );
	m_listCtrl_Icons->setMovement( QListView::Static );
	m_listCtrl_Icons->setWordWrap( true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::browse_directory(const fsLocator &i_Directory,
												bool i_bAddToHistory)
{
	if (i_bAddToHistory)
	{
		// Store old directory into history if it changed,
		// clear the forward list also since we are starting a
		// new branch to the history
		if ((m_CurrentMaterialDir.GetNumNames() > 0) &&
			(i_Directory != m_CurrentMaterialDir))
		{
			m_PreviousDirectories.push_back(m_CurrentMaterialDir);
			m_NextDirectories.clear();
			if (m_PreviousDirectories.size() > c_MaxHistory)
				m_PreviousDirectories.pop_front();
		}
	}

	m_listCtrl_Icons->clear();
	m_CurrentMaterialDir = i_Directory;
	gfDirectoryCategories::SetCurDirectory(c_MaterialDirCategory, i_Directory);

	MaterialBrowseTarget material_list(m_CurrentMaterialDir);

	// Enumerate material files in directory
	std::vector<std::string> substrings;
	const bool bSearchSubDirs = false;
	gfFileEnum::EnumerateFiles(i_Directory, material_list, substrings, bSearchSubDirs);

	// Enumerating all sub directories was too slow, so only check
	// if a sub directory has mtl files or other subdirectories.
	fsFileEnum::fsFileList dir_list;
	gfFileEnum::EnumerateDirectories(i_Directory, dir_list);
	for (int sdi=0; sdi<dir_list.size(); ++sdi)
	{
		fsLocator subdir = dir_list[sdi];
		itString subdir_name = subdir.GetName( i_Directory.GetNumNames() );
		if (subdir_name[0] != '.')
		{
			SubDirectoryTarget subdir_search;
			gfFileEnum::EnumerateDirectories(subdir, subdir_search);
			if (subdir_search.m_bHasSubdirs)
			{
				// This directory had more subdirectories, so add it to the list
				material_list.m_SubDirs.insert(fsLocator(subdir_name));
			}
			else
			{
				// No subdirectories in this directory, so look for .mtl files
				gfFileEnum::EnumerateFiles(dir_list[sdi], material_list);
				material_list.m_SubDirs.insert(fsLocator(subdir_name));
			}
		}
	}

	// Load icons for folders and materials without thumbnails
	const QString icon_dir = get_icon_directory();
	const QIcon noicon( QPixmap(icon_dir + "matbrowse-noicon.PNG") );
	const QIcon folder_icon( QPixmap(icon_dir + "matbrowse-folder.PNG") );

	// Put folder icons first
	std::set<fsLocator>::const_iterator it;
	for (it = material_list.m_SubDirs.begin(); it != material_list.m_SubDirs.end(); ++it)
	{
		itString subpath;
		fsFileUtil::LocatorToUnicodeString(*it, subpath);
		new QListWidgetItem( folder_icon, to_qstring(subpath), m_listCtrl_Icons );
	}

	const int num_materials = material_list.m_FileList.size();
	for (int i=0; i<num_materials; ++i)
	{
		const QString filename = to_qstring(material_list.m_FileList[i].GetLastName());
		try
		{
			// Reading material info doesn't read thumbnail, using different function
			// that reads out the thumbnail data separately.
			mtrThumbnailData thumbnail;
			mtrMaterialSaver::ReadThumbnail(material_list.m_FileList[i], thumbnail);

			QIcon icon = noicon;
			if (thumbnail.GetHasThumb() && thumbnail.GetBitmapSize() == c_IconWidth)
			{
				// The thumbnail is stored bottom-up in BGR order
				const int size = thumbnail.GetBitmapSize();
				QImage image( thumbnail.GetThumbParams(), size, size, size * 3, QImage::Format_BGR888 );
				icon = QIcon( QPixmap::fromImage( image.flipped(Qt::Vertical) ) );
			}
			new QListWidgetItem( icon, filename, m_listCtrl_Icons );
		}
		catch ( gfInvalidFileBinX&  )
		{
			// No need to handle this, just wasn't really one of our material files.
			// Don't create an icon for it.
		}
	}

	// update button states for this directory
	enable_buttons();
}

//--------------------------------------------------------------------
// update enabled state of bitmap buttons
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::enable_buttons()
{
	m_button_Back->setEnabled( !m_PreviousDirectories.empty() );
	m_button_Forward->setEnabled( !m_NextDirectories.empty() );

	bool bSelectedMaterialObject = mbrwPaintUtil::CanAssignMaterial();
	bool bSelectedMtlFile = (m_listCtrl_Icons->selectedItems().size() == 1);
	m_button_Paint->setEnabled( bSelectedMaterialObject && bSelectedMtlFile );
}

//--------------------------------------------------------------------
// Check mtl files and see if their thumbnail icon is out of date.
// If so, rewrite the mtl file, if possible.
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::regenerate_icons()
{
	// These lines regenerate icons for the materials that it can change:
	MaterialBrowseTarget material_list(m_CurrentMaterialDir);
	gfFileEnum::EnumerateFiles(m_CurrentMaterialDir, material_list);
	
	bool bWarnedReadOnly = false;
	const int num_materials = material_list.m_FileList.size();
	for (int i=0; i<num_materials; ++i)
	{
		bool bReadMaterial = false, bReadIcon = false;
		mtrThumbnailData thumbnail;
		try
		{
			// Reading material info doesn't read thumbnail, using different function
			// that reads out the thumbnail data separately.
			bReadIcon = mtrMaterialSaver::ReadThumbnail(material_list.m_FileList[i],
												thumbnail);
			bReadMaterial = true;
		}
		catch ( gfInvalidFileBinX&  )
		{
			// No need to handle this, just wasn't really one of our material files.
			// Don't create an icon for it.
		}

		// If no icon was read, try to create one right now
		if (bReadMaterial && !bReadIcon)
		{
			mdlMaterialInfo material_info;
			if (mtrMaterialSaver::ReadSingleMaterial(material_list.m_FileList[i], material_info))
			{
				try
				{
					mtrlIconUtil::createMtrlIcon(material_info, thumbnail);
					mtrMaterialSaver::WriteSingleMaterial(material_list.m_FileList[i], material_info, thumbnail);
				}
				catch( const fsReadOnlyX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

					std::string msg = "Cannot write to file " + filename;
					DBG_ERROR( msg );

					// Only display read-only warning once
					if (!bWarnedReadOnly)
					{
						guiMessageBox::Show(msg.c_str(), "Read only", guiMessageBox::e_OKOnly);
						bWarnedReadOnly = true;
					}
				}
			}
		}
	}
}

#endif // USE_QT
