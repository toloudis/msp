/********************************************************************************************\
**  UndoHistoryData.hpp
**
**		Data for the UndoHistory.
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef UNDOHISTORYDATA_HPP
#error UndoHistoryData.hpp multiply included
#endif
#define UNDOHISTORYDATA_HPP

#include <string>
#include <vector>


//
//
struct UndoHistoryDataItem
{
	std::string m_Name;
	int	m_Size;
};


//
//
struct UndoHistoryData
{
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	int m_NextUndoIndex;
	std::vector<UndoHistoryDataItem> m_HistoryList;
};

