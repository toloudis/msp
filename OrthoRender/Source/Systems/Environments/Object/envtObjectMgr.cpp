/*****************************************************************************
**	envtObjectMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtObjectMgr.hpp"

#include "Tool/pick3d/pick3dPickList.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Core/it/itStringUtil.hpp"
//#include "Core/name/nameString.hpp"


//============================================================================
//============================================================================
namespace
{

	int l_ObjectCounter = 0;

	//--------------------------------------------------------------------
	void create_default_name( const itString& i_Filename, nameString& o_NameString )
	{
		char strName[64];
		char filename_base[64];
		char *filename;
		strcpy( filename_base, itStringUtil::GetStdString( i_Filename ).c_str() );
		filename = strtok( filename_base, "." );

		do
		{
			sprintf( strName, "%s-%03d", filename, l_ObjectCounter );
			o_NameString.SetString( strName );
			l_ObjectCounter++;
		} while ( !envtObjectMgr::VerifyNodupeName(strName) );

		//DBG_LOG1( "Added -- name (%s)", strName );
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
}
