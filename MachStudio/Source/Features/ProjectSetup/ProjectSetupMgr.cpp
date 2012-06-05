/*****************************************************************************
**	ProjectSetupMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004-5 - All Rights Reserved
\****************************************************************************/
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"

#include "Features/ProjectSetup/Data/ProjectSetupDataParser.hpp"
#include "Features/ProjectSetup/prjPython.hpp"
#include "Support/mnm/mnmPaths.hpp"

//	library
#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/Gf/gfFileTxt.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>


//============================================================================
//============================================================================
namespace ProjectSetupMgr
{
	namespace
	{
		ProjectSetupData l_Data;

		const char* lc_ProjectFileExt = ".mpj";

		// Case-independent comparison of names within scene data
		bool scene_name_compare(const ProjectSetupSceneData& i_A, 
								const ProjectSetupSceneData& i_B)
		{
			return boost::ilexicographical_compare(i_A.m_SceneName, i_B.m_SceneName);
		}

		// Function operator for finding scene data by name
		struct find_scene_name
		{
			std::string m_SceneName;
			find_scene_name(const std::string& i_SceneName)
				: m_SceneName(i_SceneName) {}
			bool operator()(const ProjectSetupSceneData& i_Data) const
			{
				return boost::iequals(m_SceneName, i_Data.m_SceneName);
			}
		};
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		//docSingleTypeMgr::AddDocumentInterest(new ProjectSetupDocumentInterest());

		l_Data.m_bDirty = false;

		prjPython::AddCommands("mach");
	}

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	void  CleanUp()
	{
	}

	//------------------------------------------------------------------------
	//	get the current project data
	//------------------------------------------------------------------------
	ProjectSetupData& Data()
	{
		return l_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const ProjectSetupData GetData()
	{
		return l_Data;
	}

	//------------------------------------------------------------------------
	//	add a scene to the project data.
	//------------------------------------------------------------------------
	void AddSceneToProject( const std::string& i_SceneName,
							const std::string& i_SceneDescription)
	{
		//	if the scene name is empty don't add it.
		//
		if (i_SceneName.empty())
			return;

		std::vector<ProjectSetupSceneData>::iterator it = 
			std::find_if(l_Data.m_Scenes.begin(), l_Data.m_Scenes.end(), find_scene_name(i_SceneName));
		
		if (it == l_Data.m_Scenes.end())
		{
			// Scene name not found, add it
			//DBG_LOG( "adding scene (" << i_SceneName.c_str() << ")" );

			ProjectSetupSceneData scene_data;
			scene_data.m_SceneName = i_SceneName;
			scene_data.m_SceneDesc = i_SceneDescription;
			l_Data.m_Scenes.push_back( scene_data );
			l_Data.m_CurrentSceneName = i_SceneName;	// scene becomes current when adding?
			l_Data.m_bDirty = true;
		}
	}
	//------------------------------------------------------------------------
	//	remove scene with given index from the project data.
	//------------------------------------------------------------------------
	void RemoveSceneFromProject(int i_Index)
	{
		DBG_ASSERT(i_Index >= 0 && i_Index < l_Data.m_Scenes.size(), "Scene index out of range. " << i_Index << " < " << l_Data.m_Scenes.size());
		l_Data.m_Scenes.erase( l_Data.m_Scenes.begin() + i_Index );
		l_Data.m_bDirty = true;
	}

	//------------------------------------------------------------------------
	//	set the current project data
	//------------------------------------------------------------------------
	void SetData(const ProjectSetupData& i_Data)
	{
		fsLocator apppath;

		//	set the project data from this chunks data
		//
		ProjectSetupData& PData		= ProjectSetupMgr::Data();

		CopyData( i_Data, PData );

		// set the app directory and scene name
		if ( i_Data.m_ProjectDirectory.size() > 0 )
		{
			std::string dir = i_Data.m_ProjectDirectory;
			fsFileUtil::ANSIFilenameToLocator( dir, apppath );
		}
		else
		{
			apppath = gfPaths::GetPath( gfPaths::e_ExePath );
		}

		itString scene_name( i_Data.m_CurrentSceneName.c_str() );

		//	setup all the paths
		mnmPaths::SetupPaths( apppath, scene_name );

		//DBG_LOG2("(%s)(%s)", i_Data.m_ProjectDirectory.c_str(), i_Data.m_ProjectName.c_str() );
	}
	//------------------------------------------------------------------------
	//	set the current project data
	//------------------------------------------------------------------------
	void SetProjectSceneDir(const std::string& i_ProjectName,
						 const std::string& i_ProjectDirectory,
						 const std::string& i_SceneName)
	{
		ProjectSetupData& PData		= ProjectSetupMgr::Data();

		// Store the data
		PData.m_ProjectName			= i_ProjectName;
		PData.m_ProjectDirectory	= i_ProjectDirectory;
		PData.m_CurrentSceneName	= i_SceneName;

		// set the app directory and scene name
		fsLocator apppath;
		if ( i_ProjectDirectory.size() > 0 )
		{
			std::string dir = i_ProjectDirectory;
			fsFileUtil::ANSIFilenameToLocator( dir, apppath );
		}
		else
		{
			apppath = gfPaths::GetPath( gfPaths::e_ExePath );
		}

		itString scene_name( i_SceneName.c_str() );

		//	setup all the paths
		mnmPaths::SetupPaths( apppath, scene_name );

		//DBG_LOG2("(%s)(%s)", i_Data.m_ProjectDirectory.c_str(), i_Data.m_ProjectName.c_str() );
	}

	//------------------------------------------------------------------------
	//	copy project data
	//------------------------------------------------------------------------
	void CopyData(const ProjectSetupData& i_FromData, ProjectSetupData& o_ToData)
	{
		o_ToData.m_ProjectName		= i_FromData.m_ProjectName;
		o_ToData.m_ProjectDesc		= i_FromData.m_ProjectDesc;
		o_ToData.m_ProjectDirectory	= i_FromData.m_ProjectDirectory;
		o_ToData.m_CurrentSceneName	= i_FromData.m_CurrentSceneName;
		o_ToData.m_ProjectName		= i_FromData.m_ProjectName;

		o_ToData.m_Scenes.resize( i_FromData.m_Scenes.size() );
		for (int i = 0; i < i_FromData.m_Scenes.size(); ++i)
		{
			o_ToData.m_Scenes[i].m_SceneName = i_FromData.m_Scenes[i].m_SceneName;
			o_ToData.m_Scenes[i].m_SceneDesc = i_FromData.m_Scenes[i].m_SceneDesc;
		}
	}


	//------------------------------------------------------------------------
	//	sort the data
	//------------------------------------------------------------------------
	void SortData(ProjectSetupData& io_Data)
	{
		std::sort(io_Data.m_Scenes.begin(), io_Data.m_Scenes.end(), scene_name_compare);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void make_projectfilename( itString& i_Filename )
	{
		if ( !i_Filename.HasSubString( itString(lc_ProjectFileExt) ) )
		{
			i_Filename += itString(lc_ProjectFileExt);
		}
	}

	//------------------------------------------------------------------------
	//	WriteProject() - write the current project data
	//------------------------------------------------------------------------
	void WriteProject( const itString& i_Filename )
	{
		fsLocator locator = gfPaths::GetPath(mnmPaths::e_SaveProjectFiles);

		//	test for the directory first
		if ( !fsFileUtil::DirectoryExists( locator ) )
		{
			fsFileUtil::CreateDirectory( locator );
		}

		//	add the filename
		itString fullFilename;
		fullFilename = i_Filename;
		make_projectfilename( fullFilename );
		locator.Push( fullFilename );

		//	if the file exists, delete it before creating it.
		if ( fsFileUtil::FileExists(locator) )
		{
			if ( fsFileUtil::IsReadOnly(locator) )
			{
				if ( l_Data.m_bDirty )
				{
					guiMessageBox::Show( "Project File is Read-Only -- Configuration NOT SAVED.", "Project Configuration Save" , guiMessageBox::e_OKOnly);
					return;
				}
			}

			fsFileUtil::DeleteFile(locator);
		}

		fsFileUtil::CreateFile(locator);

		gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		file.WriteHeader();

		chBinWriter writer(file);

		ProjectSetupDataParser::WriteData( writer, l_Data );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void clear_data()
	{
		l_Data.m_ProjectName.clear();
		l_Data.m_ProjectDesc.clear();
		l_Data.m_ProjectDirectory.clear();
		l_Data.m_Scenes.clear();
	}

	//------------------------------------------------------------------------
	//	ReadProject() - read the current project data
	//------------------------------------------------------------------------
	void ReadProject( const itString& i_Filename )
	{
		fsLocator locator = gfPaths::GetPath(mnmPaths::e_SaveProjectFiles);

		//	test for the directory first
		if ( !fsFileUtil::DirectoryExists( locator ) )
		{
			fsFileUtil::CreateDirectory( locator );
		}

		//	add the filename
		itString fullFilename;
		fullFilename = i_Filename;
		make_projectfilename( fullFilename );
		locator.Push( fullFilename );

		clear_data();

		if ( !fsFileUtil::FileExists( locator ) )
		{
			return;
		}

		gfFileBin file(locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
		file.ReadHeader();

		chBinReader reader(file);
		ProjectSetupDataParser::ReadData( reader, l_Data );
		SortData(l_Data);

		l_Data.m_bDirty = false;
	}

	//------------------------------------------------------------------------
	//	CreateDirectories() - create directories based on the current
	//		project settings.
	//------------------------------------------------------------------------
	void CreateDirectories()
	{
		fsLocator dir;
		fsFileUtil::ANSIFilenameToLocator(l_Data.m_ProjectDirectory.c_str(), dir);

		//	set-up the main + stock project directories (if necessary)
		//
		mnmPaths::CreateProjectDirectories( dir );

		//	loop through each scene in the list and check if it has been created.
		//	if not, create it.
		//
		int i;
		int size = l_Data.m_Scenes.size();
		for ( i = 0 ; i < size ; i++ )
		{
			try
			{
				//DBG_LOG("Creating (" << l_Data.m_Scenes[i].m_SceneName.c_str() << ")" );
				mnmPaths::CreateSceneDirectories( dir, 
												itString(l_Data.m_Scenes[i].m_SceneName.c_str()) );
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem creating scene directories, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Scene Setup Error", guiMessageBox::e_OKOnly);
			}	
		}
	}

	//------------------------------------------------------------------------
	//	ImportSceneList() - ask user for a file and then import
	//		scenes from that file, one scene name per line.
	//------------------------------------------------------------------------
	void ImportSceneList()
	{
		 //	let the user browse for the file and then open/parse it.
		 //
		std::string filter("Scene List files (*.txt)|*.txt|All files (*.*)|*.*");
		fsLocator init_dir = gfPaths::GetAppPath();
		fsLocator full_path;
		if (guiFileDialogUtils::GetOpenFileName(filter, init_dir, full_path))
		{
			//	open the file and parse it
			//
			try
			{
				std::string one_line;
				gfFileTxt txt_file( full_path, fsFileStream::e_ReadOnly);
				while ( txt_file.ReadLine( one_line ) )
				{
					//DBG_LOG( "reading scene line (" << one_line.c_str() << ")" );

					//	add the scene if it doesn't already exist
					//
					boost::trim(one_line);
					if (!one_line.empty())
						AddSceneToProject( one_line, "" );
				}
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem importing scene list, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Scene Setup Error", guiMessageBox::e_OKOnly);
			}	
		}

	}

}
