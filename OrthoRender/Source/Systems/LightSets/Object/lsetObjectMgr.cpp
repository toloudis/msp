/*****************************************************************************
**	lsetObjectMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#include "Systems/LightSets/Object/lsetGlobalObject.hpp"

#include "Tool/pick3d/pick3dPickList.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Core/it/itStringUtil.hpp"
//#include "Core/name/nameString.hpp"


//============================================================================
//============================================================================
namespace
{
	// Global prtyObject for setting scene global ambient color
	lsetGlobalObject l_GlobalObject;

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
		} while ( !lsetObjectMgr::VerifyNodupeName(strName) );

		//DBG_LOG1( "Added -- name (%s)", strName );
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
// Select the prtyObject that has the global ambient light color
//--------------------------------------------------------------------
void lsetObjectMgr::SelectGlobalObject(bool i_bAppend)
{
	if (sel3dMgr::GetSelected() != &l_GlobalObject)
	{
		sel3dMgr::CreateUndoOperation();
		if (i_bAppend)
			sel3dMgr::AddToSelection(&l_GlobalObject);
		else
			sel3dMgr::Select(&l_GlobalObject);
	}
}
void lsetObjectMgr::DeselectGlobalObject()
{
	//if (sel3dMgr::GetSelected() != &l_GlobalObject)
	{
		sel3dMgr::CreateUndoOperation();
		sel3dMgr::RemoveFromSelection(&l_GlobalObject);
	}
}

//--------------------------------------------------------------------
// Return pointer to global prtyObject
//--------------------------------------------------------------------
lsetGlobalObject* lsetObjectMgr::GetGlobalObject()
{
	return &l_GlobalObject;
}

//--------------------------------------------------------------------
// Return string to use for global prtyObject
//--------------------------------------------------------------------
std::string lsetObjectMgr::GetGlobalObjectName()
{
	return l_GlobalObject.GetPick3dName();
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

	// Set the global ambient light into the special global prtyObject
	l_GlobalObject.PropertyAmbientLight().SetValue( i_Data.m_GlobalAmbient );
}

