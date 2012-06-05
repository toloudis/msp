/*****************************************************************************
**	dirltObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltObjectMgr.hpp"

#include "pick3dPickList.hpp"

#include "envSTLHelpers.hpp"
#include "itStringUtil.hpp"
//#include "nameString.hpp"

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
		} while ( !dirltObjectMgr::VerifyNodupeName(strName) );

		DBG_LOG1( "Added -- name (%s)", strName );
	}


}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dirltScriptObject* dirltObjectCreator::Create(const dirltScriptData &i_Data)
{
	// create object for dir light
	dirltScriptObject *light = new dirltScriptObject(i_Data);

	//	set a default name for the prop
	if (i_Data.m_BaseData.m_Name.GetString().empty())
	{
		nameString objName;
		create_default_name( itString("DirLight"), objName );
		light->SetName( objName );
	}
	return light;
}

//--------------------------------------------------------------------
//  Add dir light to world
//--------------------------------------------------------------------
int  dirltObjectMgr::AddDirLight( g3dDirectionalLight* i_pLight )
{
	// create object for dir light
	dirltScriptObject *light = new dirltScriptObject(i_pLight);
	sm_Objects.push_back(light);

	//	set a default name for the prop
	nameString objName;
	create_default_name( itString("DirLight"), objName );
	light->SetName( objName );

	//i_Data.m_Items
	int index = sm_Objects.size() - 1;
	return index;
}

//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void dirltObjectMgr::ShowIcons( bool i_bVisible )
{
	for (int i=0; i<sm_Objects.size(); i++)
	{
		sm_Objects[i]->SetRenderable(i_bVisible);
	}
}

