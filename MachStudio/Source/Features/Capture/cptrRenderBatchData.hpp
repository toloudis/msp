/********************************************************************************************\
**  cptrRenderBatchData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef CPTR_RENDERBATCHDATA_HPP
#error cptrRenderBatchData.hpp multiply included
#endif
#define CPTR_RENDERBATCHDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct cptrRenderBatchDataItem
{
	prtyBoolean		m_bChecked;
	prtyFilePath	m_Filename;
};

//
//
class cptrRenderBatchData
{
public:
	//------------------------------------------------------------------------
	//	data - not saved in batch file
	//------------------------------------------------------------------------
	prtyFilePath	m_BatchFilename;
	prtyBoolean		m_BatchAborted;

	//------------------------------------------------------------------------
	//	data - saved in batch file
	//------------------------------------------------------------------------
	std::vector<cptrRenderBatchDataItem> m_Scenes;
	prtyBoolean		m_SkipDialog;
};

