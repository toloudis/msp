/*****************************************************************************
**	camsDirectorsCutMgr.hpp
**
**	Keeps track of the number and name of director's cuts in a scene
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CAMS_DIRECTORSCUTMGR_HPP
#error camsDirectorsCutMgr.hpp multiply included
#endif
#define CAMS_DIRECTORSCUTMGR_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>
#include <vector>

//============================================================================
//============================================================================
class camsDirectorsCut;

//============================================================================
//============================================================================
namespace camsDirectorsCutMgr
{
	//--------------------------------------------------------------------
	//  Get number of director's cuts
	//--------------------------------------------------------------------
	int GetNumDirectorsCuts();

	//--------------------------------------------------------------------
	//  Return index for which camera to use at this moment 
	//  for the director's cut with the given index.
	//--------------------------------------------------------------------
	int GetCameraIndex(int i_Index);

	//--------------------------------------------------------------------
	//  Get names of director's cuts
	//--------------------------------------------------------------------
	void GetDirectorsCutNames(std::vector<nameString> &o_Names);

	//--------------------------------------------------------------------
	//  Get name of director's cut
	//--------------------------------------------------------------------
	void GetDirectorsCutName( int i_Index, nameString &o_Name );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDirectorsCutDescription(int i_Index, const std::string& i_Description);
	void GetDirectorsCutDescription( int i_Index, std::string& o_Description );

	//--------------------------------------------------------------------
	//  Get directors cut by index
	//--------------------------------------------------------------------
	camsDirectorsCut* GetDirectorsCut( int i_Index );

	//--------------------------------------------------------------------
	// return index of given named camera, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForName( const nameString& i_Name );

	//--------------------------------------------------------------------
	// return index of given directors cut, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForDirectorsCut( camsDirectorsCut* i_pDirectorsCut );

	//--------------------------------------------------------------------
	//  Clear out list of director's cuts
	//--------------------------------------------------------------------
	void Clear();

	//--------------------------------------------------------------------
	//  Add new director's cut to list with given name
	//--------------------------------------------------------------------
	int  AddDirectorsCut(const nameString& i_Name, 
						 const std::string& i_Description,
						 camsDirectorsCut* i_pDirectorsCut);

	//--------------------------------------------------------------------
	//  Remove director's cut from list
	//--------------------------------------------------------------------
	void  RemoveDirectorsCut(const int i_Index);

	//--------------------------------------------------------------------
	//  Change name of director's cut
	//--------------------------------------------------------------------
	void  SetDirectorsCutName(int i_Index, const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Send message to callbacks that the director's cut with the given
	//	index should be selected in the user interface.
	//--------------------------------------------------------------------
	void  SelectDirectorsCut(int i_Index);

	//--------------------------------------------------------------------
	// Callback for when number, names, descriptions change
	//--------------------------------------------------------------------
	class DirectorsCutListChangedCallback
	{
	public:
		virtual void DirectorsCutListChanged() = 0;
		virtual void SelectDirectorsCut(int i_Index) {}
	};

	//--------------------------------------------------------------------
	//  Add/Remove callbacks - callback objects are not owned
	//--------------------------------------------------------------------
	void AddCallback(DirectorsCutListChangedCallback* i_pCallback);
	void RemoveCallback(DirectorsCutListChangedCallback* i_pCallback);

}	// end of namespace
