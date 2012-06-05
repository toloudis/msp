/********************************************************************************************\
**  tasTUVData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef TAS_TUVDATA_HPP
#error tasTUVData.hpp multiply included
#endif
#define TAS_TUVDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
struct tasTUVDataItem
{
	std::string	m_Filename;
};

//============================================================================
//============================================================================
struct tasTUVData
{
	//------------------------------------------------------------------------
	//	data - not saved in TUV file
	//------------------------------------------------------------------------
	fsLocator	m_TUVFilename;

	//------------------------------------------------------------------------
	//	data - saved in TUV file
	//------------------------------------------------------------------------
	std::vector<tasTUVDataItem> m_Filenames;

	int		m_WidthFrames;
	int		m_HeightFrames;
	int		m_NumFrames;
	float	m_StartTime;
	float	m_Rate;
	bool	m_bLooping;
	bool	m_bReversing;
};
