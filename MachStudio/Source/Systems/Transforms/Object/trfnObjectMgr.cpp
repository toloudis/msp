/*****************************************************************************
**	trfnObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Core/it/itStringUtil.hpp"
//#include "Core/name/nameString.hpp"
#include <sstream>
#include <iomanip>

//============================================================================
//============================================================================
namespace
{
	int l_ObjectCounter = 0;

	//--------------------------------------------------------------------
	void create_default_name( const itString& i_Filename, nameString& o_NameString )
	{
		//char strName[64];
		std::string strName;
		char filename_base[64];
		char *filename;
		strcpy( filename_base, itStringUtil::GetStdString( i_Filename ).c_str() );
		filename = strtok( filename_base, "." );

		do
		{
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss.setf(std::ios::fixed, std::ios::floatfield);
			oss <<filename<<"-"<<std::setw(3)<<std::setfill('0')<<l_ObjectCounter;
			strName = oss.str();
			
			//sprintf( strName, "%s-%03d", filename, l_ObjectCounter );
			o_NameString.SetString( strName );
			l_ObjectCounter++;
		} while ( !trfnObjectMgr::VerifyNodupeName((char*)strName.c_str()) );

		//DBG_LOG( "Added -- name (" << strName << ")" );
	}


}	// end of namespace

//--------------------------------------------------------------------
// Create trfnScriptObject from trfnData
//--------------------------------------------------------------------
trfnScriptObject* trfnObjectCreator::Create(const trfnScriptData &i_Data)
{
	// create object for transform
	trfnScriptObject *pObject = new trfnScriptObject();
	pObject->SetScriptData( i_Data );

	// Maintain the current icon visibility
	pObject->ShowIcons(trfnObjectMgr::IconsVisible());

	//	set a default name for the transform if none provided
	if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
	{
		nameString objName(i_Data.m_BaseData.m_Name.GetValue());
		create_default_name( itString("Parent"), objName );
		pObject->SetName( objName );
	}

	return pObject;
}

//--------------------------------------------------------------------
// Return name of transform with given index
//--------------------------------------------------------------------
nameString trfnObjectMgr::GetName(int i_Index)
{
	return sm_Objects[i_Index]->GetName();
}

//--------------------------------------------------------------------
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void trfnObjectMgr::ClearItemNameUIDs(trfnTransformsData &o_Data)
{
	int num_items = o_Data.m_Items.size();
	int num_objects = 0;
	for( int i = 0; i < num_items; ++i)
	{
		//clear the object names
		num_objects = o_Data.m_Items[i].m_BaseData.m_Objects.size();
		for( int j = 0; j < num_objects; ++j )
		{
			o_Data.m_Items[i].m_BaseData.m_Objects[j].SetUID( nameString::e_InvalidUID );
		}
	}
}
