/*****************************************************************************
**	cmaData.hpp
**
**		Command Data
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_DATA_HPP
#error cmaData multiply included
#endif
#define CMA_DATA_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//	represents one hot key + menu item
//============================================================================
struct cmaHotKeyDataItem
{
	std::string m_CommandName;
	std::string m_KeyCombo;
};


//============================================================================
//	represents a list of all menu items and their hot key
//============================================================================
class cmaHotKeyDataList
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmaHotKeyDataList() {};
	cmaHotKeyDataList(const cmaHotKeyDataList&) {};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static int AddHotKey(const std::string& i_CommandName)
	{
		int found_index = find_command(i_CommandName);
		
		if (found_index == -1)
		{
			int index = sm_HotKeys.size();
			sm_HotKeys.resize(index+1);
			sm_HotKeys[index].m_CommandName = i_CommandName;
			return index;
		}

		return found_index;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static void Clear()
	{
		sm_HotKeys.clear();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static int Size()
	{
		return sm_HotKeys.size();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void SetCommandName(int i_Index, const std::string& i_CommandName)
	{
		DBG_ASSERT(((i_Index >= 0) && (i_Index < sm_HotKeys.size())), "Invalid index");

		sm_HotKeys[i_Index].m_CommandName = i_CommandName;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void SetKeyCombo(int i_Index, const std::string& i_KeyCombo)
	{
		DBG_ASSERT(((i_Index >= 0) && (i_Index < sm_HotKeys.size())), "Invalid index");

		sm_HotKeys[i_Index].m_KeyCombo = i_KeyCombo;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void ClearKeyCombo(int i_Index)
	{
		DBG_ASSERT(((i_Index >= 0) && (i_Index < sm_HotKeys.size())), "Invalid index");

		sm_HotKeys[i_Index].m_KeyCombo = "";
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static std::string isUsedKeyCombo( const std::string& i_CommandName, const std::string& i_KeyCombo )
	{
		std::string usedCommand = "";
		//returns an empty string if no command is using this hotkey
		if( i_KeyCombo == "" )
			return usedCommand;
		
		int size = sm_HotKeys.size();
		//iterate through command/hotkey pairs and check if it already matches, if so return the command that already
		//has the hotkey in use
		for (int i = 0; i < size; ++i)
		{
			if ((sm_HotKeys[i].m_KeyCombo == i_KeyCombo) && (sm_HotKeys[i].m_CommandName != i_CommandName))
			{
				usedCommand = sm_HotKeys[i].m_CommandName;
				return usedCommand;
			}
		}
		return usedCommand;
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static const std::string& GetCommandName(int i_Index)
	{
		DBG_ASSERT(((i_Index >= 0) && (i_Index < sm_HotKeys.size())), "Invalid index");

		return sm_HotKeys[i_Index].m_CommandName;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static const std::string& GetKeyCombo(int i_Index)
	{
		DBG_ASSERT(((i_Index >= 0) && (i_Index < sm_HotKeys.size())), "Invalid index");

		return sm_HotKeys[i_Index].m_KeyCombo;
	}
	
private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static int find_command(const std::string& i_CommandName)
	{
		int size = sm_HotKeys.size();
		for (int i = 0; i < size; ++i)
		{
			if (sm_HotKeys[i].m_CommandName == i_CommandName)
			{
				return i;
			}
		}
		return -1;
	}

	//private: static class std::vector<struct cmaHotKeyDataItem,class std::allocator<struct cmaHotKeyDataItem> > cmaHotKeyDataList::sm_HotKeys

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static std::vector<cmaHotKeyDataItem> sm_HotKeys;
};


//============================================================================
// Initialiazing static members
//============================================================================
std::vector<cmaHotKeyDataItem>  cmaHotKeyDataList::sm_HotKeys;
