/*****************************************************************************
**  cmmNameInterestTemplate.hpp
**
**      Name Interest for systems with lists of objects
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef CMM_NAMEINTERESTTEMPLATE_HPP
#error cmmNameInterestTemplate.hpp multiply included
#endif
#define CMM_NAMEINTERESTTEMPLATE_HPP

#ifndef NAME_NAMEINTEREST_HPP
#include "Core/name/nameNameInterest.hpp"
#endif

//============================================================================
//============================================================================
template<class xxxObjectMgr, class xxxPickObject>
class cmmNameInterestTemplate : public nameNameInterest
{
public:
	//--------------------------------------------------------------------
	//	Returns object by name.  May return NULL.
	//--------------------------------------------------------------------
	virtual nameObject* GetObjectByName( const nameString& i_String )
	{
		const int num_objs = xxxObjectMgr::GetNumObjects();
		for ( int i = 0 ; i < num_objs; i++ )
		{
			xxxPickObject *pObject = xxxObjectMgr::GetPickObject(i);
			if ( pObject->GetName() == i_String )
			{
				return pObject;
			}
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	//	Get a list of all the names
	//--------------------------------------------------------------------
	void GetNameList( nameList& io_NameList )
	{
		int num_items = xxxObjectMgr::GetNumObjects();
		int size = io_NameList.size();

		//	resize the list to accomodate all the new names
		//
		io_NameList.resize( size + num_items );

		//	add the names to the list
		//
		for ( int i=0; i < num_items; i++ )
		{
			io_NameList[ size + i ] = &(xxxObjectMgr::GetPickObject(i)->GetName());
		}
	}


	//--------------------------------------------------------------------
	//	Get the name string of the object with the passed in UID.
	//--------------------------------------------------------------------
	void GetNameString( nameUID i_NameUID, std::string& o_NameString )
	{
		//	find the object with the same name UID
		//
		const int num_items = xxxObjectMgr::GetNumObjects();
		for ( int i=0; i < num_items; i++ )
		{
			xxxPickObject *pObject = xxxObjectMgr::GetPickObject(i);
			if (pObject->GetName().GetUID() == i_NameUID )
			{
				o_NameString = pObject->GetName().GetString();
				return;
			}
		}
	}
};
