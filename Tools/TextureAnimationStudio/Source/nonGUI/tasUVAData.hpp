/********************************************************************************************\
**  tasUVAData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef TAS_UVADATA_HPP
#error tasUVAData.hpp multiply included
#endif
#define TAS_UVADATA_HPP

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
struct tasUVADataItem
{
	fsLocator			m_Filename;
	int					m_FramesPerPage;
	std::vector<float>	m_FrameLocs;		// UV offsets (m_FramesPerPage * 4)
};

//============================================================================
//============================================================================
struct tasUVAData
{
	//------------------------------------------------------------------------
	//	data - not saved in UVA file
	//------------------------------------------------------------------------
	fsLocator	m_UVAFilename;
	int	m_TexturePageWidth;	// width = height
	int m_CurTexPage;
	int	m_CurX;
	int m_CurY;
	int m_LastHeight;
	int m_ImagesInRow;
	int m_ImagesInCol;
	std::string m_ImageFormat;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int	m_NumberOfFrames;
	std::vector<fsLocator> m_Images;
	int m_NumberOfTexturePages;

	//------------------------------------------------------------------------
	//	data - saved in UVA file
	//------------------------------------------------------------------------
	std::vector<tasUVADataItem> m_TexturePages;
};
