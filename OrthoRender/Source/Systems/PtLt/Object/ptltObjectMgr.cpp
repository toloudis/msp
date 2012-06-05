/*****************************************************************************
**	ptltObjectMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

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
		} while ( !ptltObjectMgr::VerifyNodupeName(strName) );

		//DBG_LOG1( "Added -- name (%s)", strName );
	}


}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptltScriptObject* ptltObjectCreator::Create(const ptltScriptData &i_Data)
{
	// create object for point light
	ptltScriptObject *pLight = new ptltScriptObject(i_Data);

	// Maintain the current icon visibility
	pLight->ShowIcons(ptltObjectMgr::IconsVisible());

	//	set a default name for the prop
	if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
	{
		nameString objName;

		//	if there is a name in the data, use that instead of
		//	creating a generic name.
		if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
		{
			create_default_name( itString("PointLight"), objName );
		}
		else
		{
			create_default_name( itString(i_Data.m_BaseData.m_Name.GetString().c_str()), objName );
		}
		pLight->SetName( objName );
	}
	return pLight;
}


