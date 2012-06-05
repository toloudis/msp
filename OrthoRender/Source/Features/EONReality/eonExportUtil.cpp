/*****************************************************************************
**	eonExportUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/EONReality/eonExportUtil.hpp"
#include "Features/EONReality/eonGeomExporter.hpp"

#include "Systems/Props/Object/propObjectMgr.hpp"

#include "Core/App/appSimTime.hpp"
#include "Core/Ch/chBinWriter.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyDirectory.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyFolderChooserUIInfo.hpp"
#include "Core/prty/prtyListBoxUIInfo.hpp"
#include "Core/prty/prtyListChecked.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Tool/api3d/api3dBakeScene.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"

#include <sstream>
#include <windows.h>

#undef CreateObject
#undef GetObject
#undef CreateDirectory
#undef CreateFile
#undef DeleteFile
#undef GetSaveFileName

#ifdef EON_REALITY

//============================================================================
//============================================================================
namespace eonExportUtil
{
	namespace
	{

		
		//------------------------------------------------------------------------
		// Get initial directory to use for the folder chooser property.
		//------------------------------------------------------------------------
		fsLocator get_init_directory()
		{
			//TODO: Should this use a registry or preference entry?

			// Get the directory from the first prop asset we find.
			// Go up one directory and then create a new "EON" directory there.
			fsLocator init_dir;
			if (propObjectMgr::GetNumObjects() > 0)
			{
				propObjectMgr::GetObject(0)->GetDirectory(init_dir);
				if (init_dir.GetNumNames() > 1)
				{
					init_dir.Pop();
					init_dir.Push("EON");
				}
			}

			return init_dir;
		}

		//------------------------------------------------------------------------
		// Return a list of names of objects that could be baked
		//------------------------------------------------------------------------
		void get_potential_bake_objects(std::vector<nameString>& o_ObjectNames)
		{
			// Directly access prop object manager here. Could add in sets
			// also to the list. But, characters are not static geometry, so
			// they should probably be left out.
			const int num_props = propObjectMgr::GetNumObjects();
			for (int i=0; i<num_props; i++)
			{
				// Note: should check geometry file type here also
				o_ObjectNames.push_back(propObjectMgr::GetObject(i)->GetName());
			}
		}

		//--------------------------------------------------------------------
		// confirm file exists and is writable
		//--------------------------------------------------------------------
		bool confirm_locator(const fsLocator& i_Locator)
		{
			//	if the file exists, delete it before creating it.
			if ( fsFileUtil::FileExists(i_Locator) )
			{
				if ( fsFileUtil::IsReadOnly(i_Locator) )
				{
					guiMessageBox::Show( "Conversion File is Read-Only -- cannot export geometry.", "File Read-only" );
					return false;
				}

				fsFileUtil::DeleteFile(i_Locator);
			}

			fsFileUtil::CreateFile(i_Locator);
			return true;
		}
		
		//------------------------------------------------------------------------
		// Export geometry files into EON file format
		//------------------------------------------------------------------------
		void do_geometry_export(const std::set<nameString>& i_ObjectsToBake,
							   const fsLocator &i_ExportFile,
							   std::map<const g3dFragment*, std::string> &i_TextureNameMap,
							   bool i_bRelativeTextureNames)
		{
			fsLocator conv_loc = i_ExportFile;
			conv_loc.Pop();
			//conv_loc.Push("EONConversion.ecv");
			// Create .ecv filename based on desired .eoz filename
			itString eoz_filename = i_ExportFile.GetLastName();
			eoz_filename.StripExtension();
			eoz_filename += itString(".ecv");
			conv_loc.Push(eoz_filename);
			if (!confirm_locator(conv_loc))
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(conv_loc, filename);
				std::string message("Cannot write to conversion file´: ");
				message += filename;
				guiMessageBox::Show(message.c_str(), "File locked");
				return;
			}

			// Parentheses to let the gfFileBin go out of scope and close file before launching process
			{
				// Create the conversion file
				gfFileBin file(conv_loc, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
				file.WriteHeader();
				chBinWriter writer(file);

				const int num_props = propObjectMgr::GetNumObjects();
				for (int i=0; i<num_props; i++)
				{
					if (i_ObjectsToBake.find(propObjectMgr::GetObject(i)->GetName()) != i_ObjectsToBake.end())
					{
						itString filename;
						propObjectMgr::GetObject(i)->GetFilename(filename);

						//std::string message("Should export: ");
						//message += itStringUtil::GetStdString( filename );
						//guiMessageBox::Show(message.c_str(), "Object for export");

						fsLocator orig_path;
						propObjectMgr::GetObject(i)->GetDirectory(orig_path);
						orig_path.Push(filename);

						// If we have overriden materials, then we need to export those
						// material textures instead.
						std::vector< shared_ptr<mdlMaterialInfo> > material_overrides;
						propObjectMgr::GetObject(i)->GetMaterialData(material_overrides);
						eonGeomExporter::ConvertToEON(orig_path, 
													  writer, 
													  material_overrides,
													  propObjectMgr::GetObject(i)->GetG3dFragments(),
													  i_TextureNameMap,
													  i_bRelativeTextureNames);
					}
				}
			}

			fsLocator exe_loc = gfPaths::GetPath(gfPaths::e_ExePath);
			exe_loc.Push("EONConverter.exe");
			std::string exe_filename, conv_filename;
			fsFileUtil::LocatorToANSIFilename(exe_loc, exe_filename);
			fsFileUtil::LocatorToANSIFilename(conv_loc, conv_filename);
			std::ostringstream cmd_str;
			cmd_str << "\"" << exe_filename << "\" \"" << conv_filename<< "\"";
			DBG_WARNING1("Executing command: %s", cmd_str.str().c_str());
			::WinExec(cmd_str.str().c_str(), SW_SHOW);
		}
	
		//------------------------------------------------------------------------
		// Bake textures and write them to texture files
		//------------------------------------------------------------------------
		void do_texture_baking(const std::set<nameString>& i_ObjectsToBake,
							   const fsLocator &i_ExportDirectory,
							   std::map<const g3dFragment*, std::string> &io_TextureNameMap,
							   api3dBakeScene::BakeFormat i_BakeFormat,
							   int i_TextureReduce)
		{
			//TODO: this code could be changed to avoid the "enable baking" flag in the
			// fgmtScriptObject by somehow passing the nodes from the objects we have here to 
			// the api3dBakeScene function directly.

			// Loop through the props and set the "enable baking" flag for each object.
			const int num_props = propObjectMgr::GetNumObjects();
			for (int i=0; i<num_props; i++)
			{
				if (i_ObjectsToBake.find(propObjectMgr::GetObject(i)->GetName()) != i_ObjectsToBake.end())
				{
					propObjectMgr::GetObject(i)->SetEnableBaking(true);
				}
				else
				{
					propObjectMgr::GetObject(i)->SetEnableBaking(false);
				}
			}

			// Do actual baking
			api3dBakeScene::BakeScene(i_BakeFormat, 
									  api3dScene::GetScene(), 
									  i_ExportDirectory,
									  appSimTime::GetTime(), 
									  cam3dMgr::GetCamera(),
									  io_TextureNameMap,
									  i_TextureReduce);
		}

		//------------------------------------------------------------------------
		// Gather up filenames for baked textures - incrementing 
		//	a counter as part of the filename to make sure each
		// filename is unique.
		//------------------------------------------------------------------------
		void gather_baked_names(const std::set<nameString>& i_ObjectsToBake,
							    std::map<const g3dFragment*, std::string> &o_NameMap)
		{
			// Keep track of counter per filename
			std::map<std::string, int> filename_counter;

			const int num_props = propObjectMgr::GetNumObjects();
			for (int pi=0; pi<num_props; pi++)
			{
				propScriptObject *pObject = propObjectMgr::GetObject(pi);
				if (i_ObjectsToBake.find(pObject->GetName()) != i_ObjectsToBake.end())
				{
					const int num_frags = pObject->GetG3dFragments().size();
					for (int fi=0; fi<num_frags; fi++)
					{
						g3dFragment *pFragment = pObject->GetG3dFragments()[fi];

						matMaterial *pMaterial = pFragment->GetMaterial();
						std::vector<std::string> texture_names;
						pMaterial->GetEffectData()->GetTextureNames(texture_names);

						std::string baseName("Texture");
						if (!texture_names.empty())
						{
							// strip off extension
							baseName = texture_names[0].substr(0, texture_names[0].length()-4);
						}

						std::ostringstream filename_str;
						filename_str << baseName;

						std::map<std::string, int>::iterator it = filename_counter.find(baseName);
						if (it == filename_counter.end())
						{
							// First use of this texture name, no counter needed yet
							filename_counter[baseName] = 1;
						}
						else
						{
							int counter = ++it->second;
							filename_str << "_" << counter;
						}
						
						// add on "_baked.dds"
						filename_str << "_baked.dds";
					
						// set the filename for this fragment pointer into map
						o_NameMap[pFragment] = filename_str.str();
					}
				}
			}
		}
	}


	//------------------------------------------------------------------------
	// DoExportDialog
	//------------------------------------------------------------------------
	void DoExportDialog()
	{
		std::string eoz_filter("EON Studio files (*.eoz)|*.eoz|All files (*.*)|*.*");
		fsLocator exportFile;
		if (guiFileDialogUtils::GetSaveFileName(eoz_filter, exportFile))
		{
			// Configuration
			prtyBoolean bakeTextures("Bake Textures", true);
			prtyEnum textureFormat("Texture Format", api3dBakeScene::e_RGBA16f);
			textureFormat.SetEnumTag(api3dBakeScene::e_RGBA16f, "RGBA16f");
			textureFormat.SetEnumTag(api3dBakeScene::e_RGBA8, "RGBA8");
			prtyBoolean exportGeometry("Export Geometry", true);
			prtyBoolean relativeTextures("Base in '..\\Textures\\'", true);
			prtyInt8 textureReduce("Reduce Baked Texture Size", 1); // start with slight reduction by default
			//prtyDirectory exportDirectory("Export Directory", get_init_directory());

			// List of objects to bake
			std::vector<nameString> object_names;
			get_potential_bake_objects(object_names);
			if (object_names.empty())
			{
				guiMessageBox::Show("No objects available to bake.", "No objects for export");
				return;
			}

			prtyListChecked objectList("Objects to export");
			prtyListBoxUIInfo *pListUIInfo = new prtyListBoxUIInfo(&objectList);
			pListUIInfo->m_bChecked = true;
			pListUIInfo->m_bOnlyOneSelected = false;
			objectList.SetNumberOfItems(object_names.size());
			for (int j = 0; j < object_names.size(); ++j)
			{
				objectList.SetValueText(j, object_names[j].GetString());
				objectList.SetValueFlag(j, true);
				pListUIInfo->AddItem(object_names[j].GetString(), true);
			}

			// Setup dialog
			prtyPropertyUIInfoContainer bakeInfo;
			bakeInfo.Add(new prtyCheckBoxUIInfo(&bakeTextures));
			bakeInfo.Add(new prtyComboBoxUIInfo(&textureFormat));
			bakeInfo.Add(new prtyCheckBoxUIInfo(&exportGeometry));
			bakeInfo.Add(new prtyCheckBoxUIInfo(&relativeTextures));
			//bakeInfo.Add(new prtyFolderChooserUIInfo(&exportDirectory));

			// Texture Reduction when baking
			prtyNumericUpDownUIInfo *pNUDUII = new prtyNumericUpDownUIInfo(&textureReduce);
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(0);
			pNUDUII->SetMaximum(8);
			bakeInfo.Add( pNUDUII );

			bakeInfo.Add(pListUIInfo);
			if (guiPropertyDialog::ShowModal("Bake Textures to EON", 
											 bakeInfo.GetList(), 
											 "Choose options for baking and export") == guiPropertyDialog::e_OK)
			{
				std::set<nameString> objects_to_bake;
				int num_objects = objectList.GetNumberOfItems();
				for (int j = 0; j < num_objects; ++j)
				{
					if (objectList.GetValueFlag(j))
					{
						objects_to_bake.insert( object_names[j] );
					}
				}

				if (objects_to_bake.empty())
				{
					guiMessageBox::Show("No objects chosen to bake.", "No objects for export");
				}
				else if (!exportGeometry.GetValue() && !bakeTextures.GetValue())
				{
					guiMessageBox::Show("Neither 'bake textures' nor 'export geometry' chosen.", "No action for export");
				}
				else
				{
					// Confirm the directory exists
					fsLocator export_dir = exportFile;
					export_dir.Pop();
					//fsLocator export_dir = exportDirectory.GetValue();
					if (!fsFileUtil::DirectoryExists(export_dir))
						fsFileUtil::CreateDirectory(export_dir);

					// Gather up filenames for baked textures
					std::map<const g3dFragment*, std::string> baked_texture_name_map;
					gather_baked_names(objects_to_bake, baked_texture_name_map);

					// Export geometry and/or bake textures as requested.
					if (bakeTextures.GetValue())
					{
						do_texture_baking(objects_to_bake, 
										  export_dir, 
										  baked_texture_name_map,
										  (api3dBakeScene::BakeFormat)textureFormat.GetValue(),
										  textureReduce.GetValue());
					}
					// Baking textures have to go first, the eoz file
					// will zip up the textures, so they have to be there already.
					if (exportGeometry.GetValue())
					{
						do_geometry_export(objects_to_bake, 
										   exportFile, 
										   baked_texture_name_map,
										   relativeTextures.GetValue());
					}
				}
			}
		}
	}

}

#endif	// EON_REALITY