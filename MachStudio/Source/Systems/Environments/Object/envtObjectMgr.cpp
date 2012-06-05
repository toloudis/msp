/*****************************************************************************
**	envtObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtObjectMgr.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Systems/Environments/Object/envtDefaultEnvironment.hpp"
#include "Systems/Environments/Object/envtSwlEnvironment.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"

#include <iomanip>


//============================================================================
//============================================================================
namespace
{
	envtDefaultEnvironment* l_DefaultEnvironment = NULL;
	envtSwlEnvironment* l_SwlEnvironment = NULL;

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
			oss <<filename<<"-"<<std::setw(3)<<std::setfill('0')<<l_ObjectCounter;
			strName = oss.str();
			
			//sprintf( strName, "%s-%03d", filename, l_ObjectCounter );
			o_NameString.SetString( strName );
			l_ObjectCounter++;
		} while ( !envtObjectMgr::VerifyNodupeName((char*)strName.c_str()) );

		//DBG_LOG( "Added -- name (" << strName << ")" );
	}


}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtScriptObject* envtObjectCreator::Create(const envtScriptData &i_Data)
{
	nameString objName(i_Data.m_BaseData.m_Name.GetValue());

	//	set a default name for the environment if none provided
	if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
	{
		//	if there is a name in the data, use that instead of
		//	creating a generic name.
		if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
		{
			create_default_name( itString("Environment"), objName );
		}
		else
		{
			create_default_name( itString(i_Data.m_BaseData.m_Name.GetString().c_str()), objName );
		}
	}

	// create object for environment
	envtScriptObject *pEnvt = new envtScriptObject(i_Data, objName);

	// Maintain the current icon visibility
	//pEnvt->ShowIcons(envtObjectMgr::IconsVisible());

	return pEnvt;
}

void envtObjectMgr::CreateDefaultEnvironment()
{
	DBG_ASSERT(l_DefaultEnvironment == NULL, "Tried to create default environment when it already exists.");
	if (l_DefaultEnvironment == NULL)
	{
		l_DefaultEnvironment = new envtDefaultEnvironment();

//		l_DefaultEnvironment = envtObjectCreator::Create(envtScriptData());
//		evmtEnvironmentMgr::SetDefaultEnvironment(l_DefaultEnvironment->GetName());
	}
}

void envtObjectMgr::DestroyDefaultEnvironment()
{
	delete l_DefaultEnvironment;
	l_DefaultEnvironment = NULL;
}

envtDefaultEnvironment* envtObjectMgr::GetDefaultEnvironment()
{
	return l_DefaultEnvironment;
}

void envtObjectMgr::CreateSwlEnvironment()
{
	DBG_ASSERT(l_SwlEnvironment == NULL, "Tried to create swl environment when it already exists.");
	if (l_SwlEnvironment == NULL)
	{
		l_SwlEnvironment = new envtSwlEnvironment();
	}
}

void envtObjectMgr::DestroySwlEnvironment()
{
	delete l_SwlEnvironment;
	l_SwlEnvironment = NULL;
}

envtSwlEnvironment* envtObjectMgr::GetSwlEnvironment()
{
	return l_SwlEnvironment;
}


//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
//static 
void envtObjectMgr::SetData(const envtEnvironmentsData &i_Data)
{
	// call the unthreaded variation of the function
	// in the default case. Geometry systems can then
	// use a threaded variation of this function.
	SetDataUnthreaded(i_Data);

	l_DefaultEnvironment->SetData(i_Data.m_DefaultEnv.m_BaseData);
	l_SwlEnvironment->SetData(i_Data.m_SwlEnv.m_BaseData);
}

//--------------------------------------------------------------------
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void envtObjectMgr::ClearItemNameUIDs(envtEnvironmentsData &o_Data)
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

//--------------------------------------------------------------------
//  Access to whole data as one structure for easy display and
// parsing
//--------------------------------------------------------------------
//static 
envtEnvironmentsData envtObjectMgr::GetData()
{
	envtEnvironmentsData data;

	// special handling for default environment.
	data.m_DefaultEnv.m_BaseData = l_DefaultEnvironment->GetData();
	data.m_SwlEnv.m_BaseData = l_SwlEnvironment->GetData();

	const int num_objects = sm_Objects.size();
	for (int i=0; i<num_objects; i++)
	{
		data.m_Items.push_back( sm_Objects[i]->GetScriptData() );
	}
	return data;
}

//--------------------------------------------------------------------
//  Select object with given index
//--------------------------------------------------------------------
void envtObjectMgr::SelectDefaultEnvironment(bool i_bAppend /*= false*/)
{
	if (sel3dMgr::GetSelected() != l_DefaultEnvironment)
	{
		if (i_bAppend)
			sel3dMgr::AddToSelection(l_DefaultEnvironment);
		else
			sel3dMgr::Select(l_DefaultEnvironment);
	}
}

//--------------------------------------------------------------------
//  Remove object with given index from selection
//--------------------------------------------------------------------
void envtObjectMgr::DeselectDefaultEnvironment()
{
	//if (sel3dMgr::GetSelected() != sm_Objects[i_Index]->GetPickObject())
	{
		sel3dMgr::RemoveFromSelection(l_DefaultEnvironment);
	}
}

//--------------------------------------------------------------------
//  Select object with given index
//--------------------------------------------------------------------
void envtObjectMgr::SelectSwlEnvironment(bool i_bAppend /*= false*/)
{
	if (sel3dMgr::GetSelected() != l_SwlEnvironment)
	{
		if (i_bAppend)
			sel3dMgr::AddToSelection(l_SwlEnvironment);
		else
			sel3dMgr::Select(l_SwlEnvironment);
	}
}

//--------------------------------------------------------------------
//  Remove object with given index from selection
//--------------------------------------------------------------------
void envtObjectMgr::DeselectSwlEnvironment()
{
	//if (sel3dMgr::GetSelected() != sm_Objects[i_Index]->GetPickObject())
	{
		sel3dMgr::RemoveFromSelection(l_SwlEnvironment);
	}
}

