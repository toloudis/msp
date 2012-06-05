/********************************************************************************************\
**  cptrRenderBakeData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef CPTR_RENDERBAKEDATA_HPP
#error cptrRenderBakeData.hpp multiply included
#endif
#define CPTR_RENDERBAKEDATA_HPP

#ifndef PRTY_DIRECTORY_HPP
#include "Core/prty/prtyDirectory.hpp"
#endif

#ifndef RLYR_LAYERSDATA_HPP
#include "Support/rlyr/data/rlyrLayersData.hpp"
#endif

#include <vector>

//============================================================================
//	Data
//============================================================================

//============================================================================
//============================================================================
class cptrRenderBakeData
{
public:
	cptrRenderBakeData();

	/*enum BakeMode
	{
		bk_Color = 0,
		bk_Normals,
		bk_BakeTypeNum
	};*/

	enum BakeOutputFormat
	{
		bk_DDS = 0,
		bk_BMP,
		bk_JPG,
		bk_PNG,
		bk_TIF,
		bk_OutputFormatNum
	};

	enum BakeOutputResolution
	{
		bk_512 = 0,
		bk_1024,
		bk_2048,
		bk_4096
	};

	//output properties
	prtyEnum			m_BakeMode;
	prtyBoolean			m_bIsBakeNormal;
	prtyBoolean			m_bIsBakeColor;

	prtyEnum			m_OutputFormat;
	prtyEnum			m_OutputResolution;
	prtyDirectory		m_OutputDir;
	prtyBoolean			m_bIsSaveAndReplace;

	//bake properties
	prtyBoolean			m_bIsLit;
	prtyBoolean			m_bIsEnvironment;
	prtyBoolean			m_bIsShadows;
	prtyBoolean			m_bHDRAA;
	prtyBoolean			m_bIsAOVolume;
	prtyBoolean			m_bIsLPVGI;


	//data items
	//std::vector<rlyrObjectDataItem>		m_Objects;
};

