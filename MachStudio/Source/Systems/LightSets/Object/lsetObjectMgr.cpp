/*****************************************************************************
**	lsetObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"

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
			//sprintf( strName, "%s-%03d", filename, l_ObjectCounter );
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss.setf(std::ios::fixed, std::ios::floatfield);
			oss <<filename<<"-"<<std::setw(3)<<std::setfill('0')<<l_ObjectCounter;
			strName = oss.str();
			o_NameString.SetString( strName );
			l_ObjectCounter++;
		} while ( !lsetObjectMgr::VerifyNodupeName((char*)strName.c_str()) );

		//DBG_LOG( "Added -- name (" << strName << ")" );
	}


}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetScriptObject* lsetObjectCreator::Create(const lsetScriptData &i_Data)
{
	nameString objName(i_Data.m_BaseData.m_Name.GetValue());

	//	set a default name for the light set if none provided
	if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
	{
		//	if there is a name in the data, use that instead of
		//	creating a generic name.
		if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
		{
			create_default_name( itString("LightSet"), objName );
		}
		else
		{
			create_default_name( itString(i_Data.m_BaseData.m_Name.GetString().c_str()), objName );
		}
	}

	// create object for light set
	lsetScriptObject *pEnvt = new lsetScriptObject(i_Data, objName);

	return pEnvt;
}

//--------------------------------------------------------------------
// Return name of light set with given index
//--------------------------------------------------------------------
nameString lsetObjectMgr::GetName(int i_Index)
{
	return sm_Objects[i_Index]->GetName();
}

//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void lsetObjectMgr::SetData(const lsetLightSetsData &i_Data)
{
	// call the unthreaded variation of the function
	// in the default case. Geometry systems can then
	// use a threaded variation of this function.
	SetDataUnthreaded(i_Data);
}

//--------------------------------------------------------------------
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void lsetObjectMgr::ClearItemNameUIDs(lsetLightSetsData &o_Data)
{
	int num_items = o_Data.m_Items.size();
	int num_objects = 0;
	for( int i = 0; i < num_items; ++i)
	{
		//clear the object names
		num_objects = o_Data.m_Items[i].m_BaseData.m_Objects.size();
		for( int j = 0; j < num_objects; ++j )
		{
			o_Data.m_Items[i].m_BaseData.m_Objects[j].m_Name.SetUID( nameString::e_InvalidUID );
		}	
		//clear the light names
		num_objects = o_Data.m_Items[i].m_BaseData.m_Lights.size();
		for( int j = 0; j < num_objects; ++j )
		{
			o_Data.m_Items[i].m_BaseData.m_Lights[j].SetUID( nameString::e_InvalidUID );
		}	
	}
}

