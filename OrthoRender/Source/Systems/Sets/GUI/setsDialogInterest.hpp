/****************************************************************************\
**	setsDialogInterest.hpp
**
**		A DialogInterest is registered by a system so that the Object and
**	System dialogs can update information.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SETS_DIALOGINTEREST_HPP
#error setsDialogInterest.hpp multiply included
#endif
#define SETS_DIALOGINTEREST_HPP

#ifndef	CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class setsDialogInterest : public cmmDialogInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		setsDialogInterest();

		//--------------------------------------------------------------------
		//	Add Object to Placed
		//--------------------------------------------------------------------
		virtual void AddObject(const itString& i_FileName, const fsLocator& i_Path);

		//--------------------------------------------------------------------
		//	Delete Object from Placed
		//--------------------------------------------------------------------
		virtual void DeleteObject(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	Duplicate Object from Placed
		//--------------------------------------------------------------------
		virtual void DuplicateObject(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	Reload Object from Placed
		//--------------------------------------------------------------------
		virtual void ReloadObject(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	Select Object from Placed
		//--------------------------------------------------------------------
		virtual void SelectObject(const nameString& i_Name, bool i_bAppend) const;

		//--------------------------------------------------------------------
		//	Remove Object from Selection
		//--------------------------------------------------------------------
		virtual void DeselectObject(const nameString& i_Name) const;

		//--------------------------------------------------------------------
		//	GetPlacedObjects - Get Placed objects
		//--------------------------------------------------------------------
		virtual void GetPlacedObjects(cmmDialogDataList& io_DataList) const;

		//--------------------------------------------------------------------
		//	GetAvailableObjects - Get available objects
		//--------------------------------------------------------------------
		virtual void GetAvailableObjects(fsysFileList& io_FileList) const;

		//--------------------------------------------------------------------
		//	GetAllowedInteractions - returns a set of bits for which
		//	interactions are allowed (edit, delete, etc.)
		//--------------------------------------------------------------------
		virtual int GetAllowedInteractions(const fsLocator& i_Path) const;
};
