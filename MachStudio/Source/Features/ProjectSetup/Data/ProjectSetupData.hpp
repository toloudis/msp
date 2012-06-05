/********************************************************************************************\
**  ProjectSetupData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef PROJECTSETUPDATA_HPP
#error ProjectSetupData.hpp multiply included
#endif
#define PROJECTSETUPDATA_HPP

//#ifndef FS_LOCATOR_HPP
//#include "Core/fs/fsLocator.hpp"
//#endif

//#include <map>
#include <string>
#include <vector>


struct ProjectSetupSceneData
{
	std::string m_SceneName;
	std::string m_SceneDesc;
};


//
//
struct ProjectSetupData
{
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	std::string	m_ProjectName;
	std::string m_ProjectDesc;
	std::string m_ProjectDirectory;

	std::vector<ProjectSetupSceneData> m_Scenes;

	//---------------------------------------------------------------------------
	//	not saved data
	//---------------------------------------------------------------------------
	std::string m_CurrentSceneName;

	bool	m_bDirty;
};

