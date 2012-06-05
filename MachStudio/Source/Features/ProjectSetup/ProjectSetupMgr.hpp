/*****************************************************************************
**	ProjectSetupMgr.hpp
**
**		Manage the project set-up
**
**	StudioGPU
**	Copyright(C) 2004-5 - All Rights Reserved
\****************************************************************************/

#ifdef PROJECTSETUPMGR_HPP
#error ProjectSetupMgr.hpp multiply included
#endif
#define PROJECTSETUPMGR_HPP

#ifndef PROJECTSETUPDATA_HPP
#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
namespace ProjectSetupMgr
{
	//------------------------------------------------------------------------
	// Init
	//------------------------------------------------------------------------
	void  Init();

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp();

	//------------------------------------------------------------------------
	//	get the current project data
	//------------------------------------------------------------------------
	ProjectSetupData& Data();
	const ProjectSetupData GetData();

	//------------------------------------------------------------------------
	//	add a scene to the project data.
	//------------------------------------------------------------------------
	void AddSceneToProject( const std::string& i_SceneName,
							const std::string& i_SceneDescription);

	//------------------------------------------------------------------------
	//	remove scene with given index from the project data.
	//------------------------------------------------------------------------
	void RemoveSceneFromProject(int i_Index);

	//------------------------------------------------------------------------
	//	set the current project data
	//------------------------------------------------------------------------
	void SetData(const ProjectSetupData& i_Data);

	//------------------------------------------------------------------------
	//	set the current project data
	//------------------------------------------------------------------------
	void SetProjectSceneDir(const std::string& i_ProjectName,
						    const std::string& i_ProjectDirectory,
						    const std::string& i_SceneName);

	//------------------------------------------------------------------------
	//	copy project data
	//------------------------------------------------------------------------
	void CopyData(const ProjectSetupData& i_FromData, ProjectSetupData& o_ToData);

	//------------------------------------------------------------------------
	//	sort the data
	//------------------------------------------------------------------------
	void SortData(ProjectSetupData& io_Data);

	//------------------------------------------------------------------------
	//	WriteProject() - write the current project data
	//------------------------------------------------------------------------
	void WriteProject( const itString& i_Filename );

	//------------------------------------------------------------------------
	//	ReadProject() - read the current project data
	//------------------------------------------------------------------------
	void ReadProject( const itString& i_Filename );

	//------------------------------------------------------------------------
	//	CreateDirectories() - create directories based on the current
	//		project settings.
	//------------------------------------------------------------------------
	void CreateDirectories();

	//------------------------------------------------------------------------
	//	ImportSceneList() - ask user for a file and then import
	//		scenes from that file, one scene name per line.
	//------------------------------------------------------------------------
	void ImportSceneList();
}
