/*****************************************************************************
**	ptltObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Systems/PtLt/Undo/ptltOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"


//============================================================================
//============================================================================
namespace
{
	//int l_ObjectCounter = 0;

	////--------------------------------------------------------------------
	//void create_default_name( const itString& i_Filename, nameString& o_NameString )
	//{
	//	char strName[64];
	//	char filename_base[64];
	//	char *filename;
	//	strcpy( filename_base, itStringUtil::GetStdString( i_Filename ).c_str() );
	//	filename = strtok( filename_base, "." );

	//	do
	//	{
	//		std::ostringstream oss;
	//		std::string strName;
	//		oss<< filename<<"-"<<l_ObjectCounter;
	//		strName = oss.str();
	//		o_NameString.SetString( strName );
	//		l_ObjectCounter++;
	//	} while ( !ptltObjectMgr::VerifyNodupeName(strName) );

	//	DBG_LOG( "Added -- name (" << strName << ")"  );
	//}


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

		////	if there is a name in the data, use that instead of
		////	creating a generic name.
		//if (i_Data.m_BaseData.m_Name.GetValue().IsEmpty())
		//{
		//	//create_default_name( itString("PointLight"), objName );
		//	ptltObjectMgr::create_default_name( itString("PointLight"), objName,
		//		l_ObjectCounter);
		//}
		//else
		//{
		//	create_default_name( itString(i_Data.m_BaseData.m_Name.GetString().c_str()), objName );
		//}
		
		ptltOperations::CreateNewObjectName(nameString(), objName);
		pLight->SetName( objName );
	}
	return pLight;
}

//--------------------------------------------------------------------
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void ptltObjectMgr::ClearItemNameUIDs(ptltPointLightsData &o_Data)
{}
