/*****************************************************************************
**	camsDirectorsCutMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/cams/camsDirectorsCutMgr.hpp"

#include "Support/cams/camsDirectorsCut.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"

#include <iterator>

//============================================================================
//============================================================================
namespace camsDirectorsCutMgr
{
	namespace
	{
		std::vector<nameString> l_Names;
		std::vector<std::string> l_Descs;
		std::vector<camsDirectorsCut*> l_Cuts;

		// Callbacks
		std::vector<DirectorsCutListChangedCallback*> l_Callbacks;

		void notify_callbacks()
		{
			std::for_each(l_Callbacks.begin(), l_Callbacks.end(), 
				std::mem_fn(&DirectorsCutListChangedCallback::DirectorsCutListChanged));
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Get number of director's cuts
	//--------------------------------------------------------------------
	int GetNumDirectorsCuts()
	{
		return l_Names.size();
	}

	//--------------------------------------------------------------------
	//  Return index for which camera to use at this moment 
	//  for the director's cut with the given index.
	//--------------------------------------------------------------------
	int GetCameraIndex(int i_Index)
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < (int) l_Cuts.size()), "index out of range " << i_Index << " < " <<  (int)l_Cuts.size() );
		return l_Cuts[i_Index]->GetCameraIndex();
	}

	//--------------------------------------------------------------------
	//  Get names of director's cuts
	//--------------------------------------------------------------------
	void GetDirectorsCutNames(std::vector<nameString> &o_Names)
	{
		std::copy(l_Names.begin(), l_Names.end(), std::back_inserter(o_Names));
	}

	//--------------------------------------------------------------------
	//  Get name of director's cut
	//--------------------------------------------------------------------
	void GetDirectorsCutName( int i_Index, nameString &o_Name )
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < (int) l_Names.size()), "index out of range " << i_Index << " < " <<  (int)l_Names.size() );
		o_Name = l_Names[i_Index];
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  SetDirectorsCutDescription(int i_Index, const std::string& i_Description)
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < (int) l_Descs.size()), "index out of range " << i_Index << " < " <<  (int)l_Descs.size() );
		l_Descs[i_Index] = i_Description;
		notify_callbacks();
	}
	void GetDirectorsCutDescription( int i_Index, std::string& o_Description )
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < (int) l_Descs.size()), "index out of range " << i_Index << " < " <<  (int)l_Descs.size() );
		o_Description = l_Descs[i_Index];
	}

	//--------------------------------------------------------------------
	//  Get directors cut by index
	//--------------------------------------------------------------------
	camsDirectorsCut* GetDirectorsCut( int i_Index )
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cuts.size()), "index out of range" );
		return l_Cuts[i_Index];
	}

	//--------------------------------------------------------------------
	// return index of given named camera, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForName( const nameString& i_Name )
	{
		for (int i=0; i<l_Names.size(); i++)
			if (l_Names[i] == i_Name)
				return i;
		return -1;
	}

	//--------------------------------------------------------------------
	// return index of given directors cut, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForDirectorsCut( camsDirectorsCut* i_pDirectorsCut )
	{
		for (int i=0; i<l_Cuts.size(); i++)
			if (l_Cuts[i] == i_pDirectorsCut)
				return i;
		return -1;

	}

	//--------------------------------------------------------------------
	//  Clear out list of director's cuts
	//--------------------------------------------------------------------
	void Clear()
	{
		l_Names.clear();
		l_Descs.clear();
		l_Cuts.clear();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Add new director's cut to list with given name
	//--------------------------------------------------------------------
	int  AddDirectorsCut(const nameString& i_Name, 
						 const std::string& i_Description,
						 camsDirectorsCut* i_pDirectorsCut)
	{
		l_Names.push_back(i_Name);
		l_Descs.push_back(i_Description);
		l_Cuts.push_back(i_pDirectorsCut);
		notify_callbacks();

		int index = l_Names.size() - 1;
		return index;
	}

	//--------------------------------------------------------------------
	//  Remove director's cut from list
	//--------------------------------------------------------------------
	void  RemoveDirectorsCut(const int i_Index)
	{
		if (i_Index >= 0)
		{
			l_Names.erase(l_Names.begin() + i_Index);
			l_Descs.erase(l_Descs.begin() + i_Index);
			l_Cuts.erase(l_Cuts.begin() + i_Index);
			
			notify_callbacks();
		}
	}

	//--------------------------------------------------------------------
	//  Change name of director's cut
	//--------------------------------------------------------------------
	void  SetDirectorsCutName(int i_Index, const nameString& i_Name)
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_Names.size()), "index out of range" );
		l_Names[i_Index] = i_Name;
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Send message to callbacks that the director's cut with the given
	//	index should be selected in the user interface.
	//--------------------------------------------------------------------
	void  SelectDirectorsCut(int i_Index)
	{
		std::vector<DirectorsCutListChangedCallback*>::iterator it;
		for (it = l_Callbacks.begin(); it != l_Callbacks.end(); ++it)
		{
			(*it)->SelectDirectorsCut(i_Index);
		}
	}

	//--------------------------------------------------------------------
	//	List the names and IDs
	//--------------------------------------------------------------------
	void DebugNames()
	{
		DBG_TRACE("Names................................");
		int index;
		for (index = 0; index < l_Names.size(); ++index)
		{
			DBG_TRACE(l_Names[index].GetUID() << " _ " << l_Names[index].GetString().c_str() );
		}
	}

	//--------------------------------------------------------------------
	//  Add/Remove callbacks - callback objects are not owned
	//--------------------------------------------------------------------
	void AddCallback(DirectorsCutListChangedCallback* i_pCallback)
	{
		l_Callbacks.push_back(i_pCallback);
	}
	void RemoveCallback(DirectorsCutListChangedCallback* i_pCallback)
	{
		envSTLHelpers::RemoveOneValue(l_Callbacks, i_pCallback);
	}

}	// end of namespace
