/********************************************************************************************\
**  cptrRenderStatsData.hpp
**
**		structures for the capture statistics.
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef CPTR_RENDERSTATSDATA_HPP
#error cptrRenderStatsData.hpp multiply included
#endif
#define CPTR_RENDERSTATSDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

#include <string>
#include <vector>


//
struct cptrRenderStatsForCameraRenderBlockData
{
	cptrRenderStatsForCameraRenderBlockData()
		: m_fStartTime(0), m_fEndTime(0) {}

	std::string m_CameraRenderBlockName;
	float		m_fStartTime;
	float		m_fEndTime;
};

//
struct cptrRenderStatsForSceneCameraData
{
	cptrRenderStatsForSceneCameraData()
		: m_fStartTime(0), m_fEndTime(0), 
		m_CurrentCameraRenderBlockIndex(0) {}

	std::string m_CameraName;
	std::string m_CameraDesc;
	float		m_fStartTime;
	float		m_fEndTime;
	int			m_CurrentCameraRenderBlockIndex;

	std::vector<cptrRenderStatsForCameraRenderBlockData> m_RenderBlocks;
};

//
struct cptrRenderStatsForSceneData
{
	cptrRenderStatsForSceneData()
		: m_fStartTime(0), m_fEndTime(0), 
		m_CurrentCameraIndex(0) {}

	std::string	m_SceneName;
	float		m_fStartTime;
	float		m_fEndTime;
	int			m_CurrentCameraIndex;

	std::vector<cptrRenderStatsForSceneCameraData> m_Cameras;
};

//
class cptrRenderStatsData
{
public:
	cptrRenderStatsData()
	:	m_fStartTime(0), 
		m_fEndTime(0), 
		m_CurrentSceneIndex(0)
	{
		m_bOpenOnRender.SetValue(false);
	}

public:
	std::string	m_BatchName;
	float		m_fStartTime;
	float		m_fEndTime;
	int			m_CurrentSceneIndex;
	prtyBoolean	m_bOpenOnRender;

	std::vector<cptrRenderStatsForSceneData> m_Scenes;
};

