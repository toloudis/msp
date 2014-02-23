/*****************************************************************************
**	mbrwMaterialSwatchDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/MaterialBrowse/wxGUI/mbrwMaterialSwatchDialog.hpp"

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
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include <set>

#ifdef USE_WXWIDGETS

#include <wx/dnd.h>	// for drag and drop


//============================================================================
//============================================================================
namespace
{
	const int c_IconWidth = 128;  
	const int c_MaxHistory = 32;
	const char* c_MaterialDirCategory = "Materials";

	//--------------------------------------------------------------------
	// Icon directory as wxString
	//--------------------------------------------------------------------
	wxString get_icon_directory()
	{
		itString wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return wxString(wide_dir.GetString());
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
				itString ext;
				i_File.GetLastName().GetExtension( ext );
				if (ext == itString(L"MTL") || ext == itString(L"mtl"))
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
mbrwMaterialSwatchDialog::mbrwMaterialSwatchDialog( wxWindow* parent, 
													const wxString& i_Title )
:	mbrwMaterialSwatchDialogBase( parent, get_icon_directory() )
{

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(2).Bottom());

	//fsLocator init_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
	fsLocator init_dir = mbrwDialogUtil::GetInitialDirectory();
	browse_directory( init_dir );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mbrwMaterialSwatchDialog::~mbrwMaterialSwatchDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing mbrwMaterialSwatchDialog()");
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
void mbrwMaterialSwatchDialog::buttonHome_Click( wxCommandEvent& i_Event )
{
	//fsLocator home_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
	fsLocator home_dir = mbrwDialogUtil::GetInitialDirectory();
	browse_directory( home_dir );

}
void mbrwMaterialSwatchDialog::buttonRefresh_Click( wxCommandEvent& i_Event )
{
	// This regenerate thumbnail icons for the materials files that are outdated:
	regenerate_icons();

	// These lines reload the thumbnails into the browser:
	if (m_CurrentMaterialDir.GetNumNames() > 0)
		browse_directory( m_CurrentMaterialDir );
}
void mbrwMaterialSwatchDialog::buttonBack_Click( wxCommandEvent& i_Event )
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
void mbrwMaterialSwatchDialog::buttonForward_Click( wxCommandEvent& i_Event )
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
void mbrwMaterialSwatchDialog::buttonUp_Click( wxCommandEvent& i_Event )
{
	if (m_CurrentMaterialDir.GetNumNames() > 1)
	{
		fsLocator parent_dir = m_CurrentMaterialDir;
		parent_dir.Pop();
		browse_directory( parent_dir );
	}

}
void mbrwMaterialSwatchDialog::buttonPaint_Click( wxCommandEvent& i_Event )
{
	if (m_listCtrl_Icons->GetSelectedItemCount() == 1)
	{
		// Strange way to get the selected index
		long item = m_listCtrl_Icons->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
        if ( item > -1 )
		{
			wxString file_str = m_listCtrl_Icons->GetItemText(item);

			fsLocator mtl_loc = m_CurrentMaterialDir;
			mtl_loc.Push(itString((const char*)file_str.c_str()));
			mbrwPaintUtil::AssignMaterialFile(mtl_loc);
		}
    }
}

void mbrwMaterialSwatchDialog::buttonNewFolder_Click(wxCommandEvent& i_Event)
{
	fsLocator init_dir;
	gfDirectoryCategories::GetCurDirectory(c_MaterialDirCategory, init_dir);
	itString defaultPath;
	fsFileUtil::LocatorToUnicodeString(init_dir, defaultPath);
	wxWindow *pParent = twxSystem::g_pMainForm;
	wxDirDialog dialog(pParent, 
					   L"Create a new directory",
					   defaultPath.GetString(), wxDD_NEW_DIR_BUTTON);
	if(dialog.ShowModal() == wxID_OK)
	{
		fsLocator material_dir;
        fsFileUtil::UnicodeStringToLocator(itString(dialog.GetPath().wc_str()), material_dir);
		fsFileUtil::CreateDirectory(material_dir);
	}
}
					   

void mbrwMaterialSwatchDialog::buttonBrowse_Click( wxCommandEvent& i_Event )
{
	fsLocator init_dir;
	gfDirectoryCategories::GetCurDirectory(c_MaterialDirCategory, init_dir);
	itString defaultPath;
	fsFileUtil::LocatorToUnicodeString(init_dir, defaultPath);
	
	wxWindow *pParent = twxSystem::g_pMainForm;
	wxDirDialog dialog(pParent, 
					   L"Browse material library directory", 
					   defaultPath.GetString());
	if (dialog.ShowModal() == wxID_OK)
	{
		fsLocator material_dir;
        fsFileUtil::UnicodeStringToLocator(itString(dialog.GetPath().wc_str()), material_dir);
		browse_directory( material_dir );

		//bga - not sure about this, using the browse button to 
		// define the "home" directory
		mbrwDialogUtil::SetInitialDirectory(material_dir);
	}
}
//void mbrwMaterialSwatchDialog::choiceSubDirs_Change( wxCommandEvent& i_Event )
//{
//	int sel_id = this->m_choice_SubDirs->GetSelection();
//	if (sel_id >= 1) // Choice 0 is the current directory
//	{
//		wxString sub_dir_str = m_choice_SubDirs->GetStringSelection();
//
//		// Concatenate relative path to the root directory to get fullpath
//		itString fullpath;
//		fsFileUtil::LocatorToUnicodeString(m_CurrentMaterialDir, fullpath);
//		fullpath += L"\\";
//		fullpath += sub_dir_str.c_str();
//
//		fsLocator new_dir;
//		fsFileUtil::UnicodeStringToLocator(fullpath, new_dir);
//		browse_directory( new_dir );
//	}
//}
void mbrwMaterialSwatchDialog::listCtrl_beginDrag( wxListEvent& i_Event )
{
#if wxUSE_DRAG_AND_DROP
	int item_index = i_Event.GetIndex();
	wxString filename = m_listCtrl_Icons->GetItemText(item_index);
	if ((m_CurrentMaterialDir.GetNumNames() > 0) &&
		(!filename.empty()))
	{	
		// Check to see if this is really a mtl file or just a folder icon.
		wxString lower_case = filename.Lower();
		if (lower_case.EndsWith(L".mtl"))
		{
			// start drag operation
			wxFileDataObject matFileObject;

			fsLocator fullpath = m_CurrentMaterialDir;
			fullpath.Push( itString((const char*)filename.c_str()) );

			itString itFilename;
			fsFileUtil::LocatorToUnicodeString(fullpath, itFilename);
			matFileObject.AddFile(itFilename.GetString());

			wxDropSource source(matFileObject, this);
								//wxDROP_ICON(dnd_copy),
								//wxDROP_ICON(dnd_move),
								//wxDROP_ICON(dnd_none));

			// While dragging, auto highlight material
			//mbrwPaintUtil::SetDoAutoPickMaterial(true);
			source.DoDragDrop();
			//mbrwPaintUtil::SetDoAutoPickMaterial(false);
		}
	}

#endif // wxUSE_DRAG_AND_DROP
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::listCtrl_doubleClick( wxListEvent& i_Event )
{
	// double click on a folder item opens that folder
	int item_index = i_Event.GetIndex();
	wxString sub_dir_str = m_listCtrl_Icons->GetItemText(item_index);
	if ((m_CurrentMaterialDir.GetNumNames() > 0) &&
		(!sub_dir_str.empty()))
	{	
		// If it doesn't end with ".mtl" assume it is a folder icon
		wxString lower_case = sub_dir_str.Lower();
		if (!lower_case.EndsWith(L".mtl"))
		{
			// Concatenate relative path to the root directory to get fullpath
			fsLocator new_dir = m_CurrentMaterialDir;
			new_dir.Push( itString((const char*)sub_dir_str.c_str()) );
			browse_directory( new_dir );
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::listCtrl_selectionChange( wxListEvent& i_Event )
{
	enable_buttons();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::OnChoice( wxCommandEvent& i_Event )
{
	const bool l_ADDSTYLE = true;
	const bool l_REMOVESTYLE = false;

	int sel = this->m_choice_view->GetSelection();
	if (sel == 0)
	{
		m_listCtrl_Icons->SetSingleStyle( wxLC_AUTOARRANGE|wxLC_ICON|wxLC_SINGLE_SEL, l_ADDSTYLE );
	}
	else if (sel == 1)
	{
		m_listCtrl_Icons->SetSingleStyle( wxLC_AUTOARRANGE|wxLC_LIST|wxLC_SINGLE_SEL, l_ADDSTYLE );
	}
	else
	{
		m_listCtrl_Icons->SetSingleStyle( wxLC_AUTOARRANGE|wxLC_SMALL_ICON|wxLC_SINGLE_SEL, l_ADDSTYLE );
	}
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

	m_listCtrl_Icons->ClearAll();
	m_CurrentMaterialDir = i_Directory;
	gfDirectoryCategories::SetCurDirectory(c_MaterialDirCategory, i_Directory);

	//itString fullpath;
	//fsFileUtil::LocatorToUnicodeString(m_CurrentMaterialDir, fullpath);
	//m_choice_SubDirs->Clear();
	//m_choice_SubDirs->Append(fullpath.GetString());
	//m_choice_SubDirs->SetSelection( 0 );

	MaterialBrowseTarget material_list(m_CurrentMaterialDir);
	// Enumerate material files in directory, extract thumbnails from the files
	// and add those bitmaps to the grid as drag sources.
	std::vector<itString> substrings;
	// need to check for extension=="mtl", not just substring
	//substrings.push_back( itString(".mtl") ); 
	//substrings.push_back( itString(".MTL") );
	const bool bSearchSubDirs = false;
	gfFileEnum::EnumerateFiles(i_Directory, material_list, substrings, bSearchSubDirs);

	// Enumerating all sub directories was too slow, so I am only going to check for 
	// if a sub directory has mtl files or other subdirectories. This might point us down
	// some paths that won't have material files, but it was too expensive to check further
	// if the user just chose "C:\"
	//
	fsFileEnum::fsFileList dir_list;
	// Need to enumerate directories without using anything that starts with a "."
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
				// to display in the browser. Maybe there are mtl files underneath and maybe not.
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
	

	// Fill subdirectories into combo box
	//std::set<fsLocator>::const_iterator it;
	//for (it = material_list.m_SubDirs.begin(); it != material_list.m_SubDirs.end(); ++it)
	//{
	//	itString subpath;
	//	fsFileUtil::LocatorToUnicodeString(*it, subpath);
	//	m_choice_SubDirs->Append(subpath.GetString());
	//}

	struct sThumbnailInfo
	{
		itString m_Filename;
		int m_ImageIndex;
	};

	std::vector<sThumbnailInfo> thumbnail_items;
	std::auto_ptr<wxImageList> image_list(new wxImageList(c_IconWidth,c_IconWidth));
	
	// Load an icon for folders
	wxString icon_dir = get_icon_directory();
	wxBitmap noicon_bmp(icon_dir + wxT("\\matbrowse-noicon.PNG"), wxBITMAP_TYPE_ANY); 
	int noicon_index = image_list->Add( noicon_bmp );
	wxBitmap folder_bmp(icon_dir + wxT("\\matbrowse-folder.PNG"), wxBITMAP_TYPE_ANY); 
	int folder_index = image_list->Add( folder_bmp );

	const int num_materials = material_list.m_FileList.size();
	for (int i=0; i<num_materials; ++i)
	{
		sThumbnailInfo list_item;
		list_item.m_Filename = material_list.m_FileList[i].GetLastName();

		//std::string filename;
		//fsFileUtil::LocatorToANSIFilename(material_list[i], filename);
		//filename = itStringUtil::GetStdString(material_list[i].GetLastName());
		//DBG_LOG("Creating browse icon for material file: " << filename);

		try
		{
			// Reading material info doesn't read thumbnail, using different function
			// that reads out the thumbnail data separately.
			//envType::UInt8* pixelBuffer = NULL;
			//int bufferSize = 0;
			//int bitmapSize = 64;
			//mbrwSwatchItem *pSwatch = NULL;
			mtrThumbnailData thumbnail;
			bool bReadIcon = mtrMaterialSaver::ReadThumbnail(material_list.m_FileList[i],
												thumbnail);
												//pixelBuffer,bufferSize, bitmapSize);

			// If no icon was read, try to create one right now
			//bga - This code takes a while and it requires that we handle absolute paths
			// with the material library better.
			//if (!bReadIcon)
			//{
			//	mdlMaterialInfo material_info;
			//	if (mtrMaterialSaver::ReadSingleMaterial(material_list.m_FileList[i], material_info))
			//	{
			//		mtrlIconUtil::createMtrlIcon(material_info, thumbnail);
			//	}
			//}

			// If we have a thumbnail now, create wxWidgets image for it
			if (thumbnail.GetHasThumb())
			{
				// wxWidgets wants the image data in opposite order,
				// so create a new buffer and reverse order of bytes
				envType::UInt8* reverseBuffer = new envType::UInt8[thumbnail.GetBufferSize()];
				const envType::UInt8 *pSrcPtr = thumbnail.GetThumbParams();
				envType::UInt8 *pDestPtr = reverseBuffer+(thumbnail.GetBufferSize()-1);
				while (pDestPtr >= reverseBuffer)
				{
					*pDestPtr-- = *pSrcPtr++;
				}

				// first create a wxImage to hold the image data
				bool isStaticData = true;
				wxImage image( thumbnail.GetBitmapSize(), 
							thumbnail.GetBitmapSize(), 
							reverseBuffer, 
							isStaticData);

				// Confirm we have the correct size thumbnail
				if (thumbnail.GetBitmapSize() == c_IconWidth)
				{
					// now create a bitmap from the image
					const int c_ImageDepth = 24; //24 bit depth
					//wxBitmap thumb_bmp(image, c_ImageDepth); 
					wxBitmap thumb_bmp(image.Mirror(), c_ImageDepth);  //switch left and right
					//wxBitmap thumb_bmp(pixelBuffer, wxBITMAP_TYPE_BMP, bitmapSize, bitmapSize, c_ImageDepth);
					
					// Add bitmap to image list and set image index for list item to use this image
					list_item.m_ImageIndex = image_list->Add( thumb_bmp );

					// doing list items now, no more swatch items
					//pSwatch = new mbrwSwatchItem( m_scrolledWindow1, material_list[i], thumb_bmp );
				}
				else
				{
					// Images all have to be the same size
					list_item.m_ImageIndex = -1;
				}
				
				// clean up the reverse buffer
				delete reverseBuffer;
			}
			else
			{
				// No thumbnail, so display just filename
				//list_item.m_ImageIndex = -1;
				list_item.m_ImageIndex = noicon_index;
			}
		
			thumbnail_items.push_back( list_item );
		}
		catch ( gfInvalidFileBinX&  )
		{
			// No need to handle this, just wasn't really one of our material files.
			// Don't create an icon for it.
		}
	}

	// assign the image list to the list control, passing ownership
	m_listCtrl_Icons->AssignImageList( image_list.release(), wxIMAGE_LIST_NORMAL );
	
	// Put folder icons first
	int index = 0;
	std::set<fsLocator>::const_iterator it;
	for (it = material_list.m_SubDirs.begin(); it != material_list.m_SubDirs.end(); ++it)
	{
		itString subpath;
		fsFileUtil::LocatorToUnicodeString(*it, subpath);
		m_listCtrl_Icons->InsertItem(index++, subpath.GetString(), folder_index);
	}

	const int num_items = thumbnail_items.size();
	for (int i=0; i<num_items; ++i)
	{
		m_listCtrl_Icons->InsertItem(index++, thumbnail_items[i].m_Filename.GetString(), 
								   thumbnail_items[i].m_ImageIndex);
	}

	// update button states for this directory
	enable_buttons();
}

//--------------------------------------------------------------------
// update enabled state of bitmap buttons
//--------------------------------------------------------------------
void mbrwMaterialSwatchDialog::enable_buttons()
{
	m_button_Back->Enable( !m_PreviousDirectories.empty() );
	m_button_Forward->Enable( !m_NextDirectories.empty() );

	bool bSelectedMaterialObject = mbrwPaintUtil::CanAssignMaterial();
	bool bSelectedMtlFile = (m_listCtrl_Icons->GetSelectedItemCount() == 1);
	m_button_Paint->Enable( bSelectedMaterialObject && bSelectedMtlFile );
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

#endif // USE_WXWIDGETS
