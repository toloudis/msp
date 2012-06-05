/********************************************************************************************\
**  SceneSetupData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef SCENESETUPDATA_HPP
#error SceneSetupData.hpp multiply included
#endif
#define SCENESETUPDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <string>
#include <vector>


// TODO:	use IDs to keep track of the names, not string
//
struct SceneSetupProjectInfo
{
	std::string	m_ProjectName;
	std::string m_ProjectDirectory;
	std::string m_SceneName;

	bool	m_bFinished;	// do not write this value out.
};

struct ScenePropertiesData
{
	std::string	m_Notes;
};

//
//
struct SceneSetupData
{
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	SceneSetupProjectInfo	m_Data;
	ScenePropertiesData		m_PropertiesData;
};

