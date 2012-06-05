/*****************************************************************************
**	grupObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/Object/grupObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"

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
		} while ( !grupObjectMgr::VerifyNodupeName((char*)strName.c_str()) );

		//DBG_LOG( "Added -- name (" << strName << ")" );
	}


}	// end of namespace

//--------------------------------------------------------------------
// Create grupGroupObject from grupData
//--------------------------------------------------------------------
grupGroupObject* grupObjectCreator::Create(const grupData &i_Data)
{
	nameString objName(i_Data.m_Name.GetValue());

	//	set a default name for the group if none provided
	if (i_Data.m_Name.GetValue().IsEmpty())
	{
		create_default_name( itString("SelectionSet"), objName );
	}

	// create object for group
	grupGroupObject *pObject = new grupGroupObject(i_Data, objName);

	return pObject;
}

//--------------------------------------------------------------------
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void grupObjectMgr::ClearItemNameUIDs(grupGroupsData &o_Data)
{}
