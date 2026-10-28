/********************************************************************************************\
**	ImportData.hpp
**
**		Data for the import.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef IMPORTDATA_HPP
#error ImportData.hpp multiply included
#endif
#define IMPORTDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif



//============================================================================
//============================================================================
//struct ImportChunkData
//{
//	std::string m_Desc;
//}


//============================================================================
//============================================================================
class ImportChunkListData
{
public:
	std::string m_Desc;
	bool m_bRemovableChunk;
	std::vector<std::string> m_Items;
	//std::vector<ImportChunkData> m_Items;
};


//============================================================================
//============================================================================
class ImportData
{
public:
	//----------------------------------------------------------------------------
	//	Constructor
	//----------------------------------------------------------------------------
	ImportData();

	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	fsLocator m_LastFile;
	docDocument::LoadFlags m_LoadMethod;

	std::vector<ImportChunkListData> m_Chunks;
};
