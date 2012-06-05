/****************************************************************************\
**	setsNameInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/setsNameInterest.hpp"

#include "Systems/Sets/Data/setsScriptData.hpp"
#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/Data/setsObject.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
setsNameInterest::setsNameInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
setsNameInterest::~setsNameInterest()
{
}

//--------------------------------------------------------------------
//	Returns object by name.  May return NULL.
//--------------------------------------------------------------------
//virtual
nameObject* setsNameInterest::GetObjectByName( const nameString& i_String )
{
	int num_objs = setsDataMgr::GetNumSetItems();
	for ( int i = 0 ; i < num_objs; i++ )
	{
		//DBG_LOG3( "GOBN %02d)  %02d vs %02d", i, i_String.GetUID(), setsDataMgr::GetItemData(i).m_BaseData.m_Name.GetUID() );

		if (   ( setsDataMgr::GetItemData(i).m_BaseData.m_Name.GetUID() != nameString::e_InvalidUID )
			&& ( i_String.GetUID() != nameString::e_InvalidUID ) )
		{
			if ( setsDataMgr::GetItemData(i).m_BaseData.m_Name.GetUID() == i_String.GetUID() )
			{
				//DBG_LOG4("  Got named object: %2d) %d (%s)  sets %d", i, i_String.GetUID(), i_String.GetString().c_str(), setsDataMgr::GetObject(i)->GetName().GetUID() );
				return setsDataMgr::GetObject(i);
			}
		}
		else
		{
			if ( setsDataMgr::GetItemData(i).m_BaseData.m_Name.GetString() == i_String.GetString() )
			{
				nameObject* no = setsDataMgr::GetObject(i);
				//DBG_LOG4("  Got named object. %2d) %d (%s)  sets %d", i, i_String.GetUID(), i_String.GetString().c_str(), no->GetName().GetUID() );
				return setsDataMgr::GetObject(i);
			}
		}
	}
	return NULL;
}


//--------------------------------------------------------------------
//	Get a list of all the names
//--------------------------------------------------------------------
void setsNameInterest::GetNameList( nameList& io_NameList )
{
	int num_items = setsDataMgr::GetNumSetItems();
	int size = io_NameList.size();

	//	resize the list to accomodate all the new names
	//
	io_NameList.resize( size + num_items );

	//	add the names to the list
	//
	for ( int i=0; i < num_items; i++ )
	{
		io_NameList[ size + i ] = &(setsDataMgr::GetItemData(i).m_BaseData.m_Name.GetValue());

		//DBG_LOG3( "sets %02d - (%s) [%s]", i, io_NameList[i]->GetString().c_str(), 
		//	setsDataMgr::Getsets(i)->GetPickObject()->GetName().GetString().c_str() );
	}
}


//--------------------------------------------------------------------
//	Get the name of the object with the passed in UID.
//--------------------------------------------------------------------
void setsNameInterest::GetNameString( nameUID i_NameUID, std::string& o_NameString )
{
	int num_items = setsDataMgr::GetNumSetItems();

	//	add the names to the list
	//
	for ( int i=0; i < num_items; i++ )
	{
		//DBG_LOG3( "GetNameString %02d (%s) vs %02d", setsDataMgr::Getsets(i)->GetPickObject()->GetName().GetUID(), setsDataMgr::Getsets(i)->GetPickObject()->GetName().GetString().c_str(), i_NameUID );

		if (setsDataMgr::GetItemData(i).m_BaseData.m_Name.GetUID() == i_NameUID )
		{
			//DBG_LOG2( "sets %02d - (%s)", i, setsDataMgr::Getsets(i)->GetPickObject()->GetName().GetString().c_str() );

			o_NameString = setsDataMgr::GetItemData(i).m_BaseData.m_Name.GetString();
			return;
		}
	}
}

