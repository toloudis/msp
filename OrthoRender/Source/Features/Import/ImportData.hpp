/********************************************************************************************\
**  ImportData.hpp
**
**		Data for the import.
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef IMPORTDATA_HPP
#error ImportData.hpp multiply included
#endif
#define IMPORTDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

//struct ImportChunkData
//{
//	std::string m_Desc;
//}

struct ImportChunkListData
{
	std::string m_Desc;
	bool m_bRemovableChunk;
	std::vector<std::string> m_Items;
	//std::vector<ImportChunkData> m_Items;
};

//
//
struct ImportData
{
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	fsLocator m_LastFile;

	std::vector<ImportChunkListData> m_Chunks;
};

