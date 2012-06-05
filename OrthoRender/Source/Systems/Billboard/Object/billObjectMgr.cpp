/*****************************************************************************
**	billObjectMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Billboard/Object/billObjectMgr.hpp"

#include "Systems/Billboard/GUI/billGeomList.hpp"

#include "Tool/pick3d/pick3dPickList.hpp"

#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <vector>


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
		} while ( !billObjectMgr::VerifyNodupeName(strName) );

		DBG_LOG1( "Added -- name (%s)", strName );
	}

}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
billScriptObject* billObjectCreator::Create(const billScriptData &i_Data)
{
	// load texture using filename from data
	fsLocator tex_loc;
	if (billGeomList::FindFile(i_Data.m_BaseData.m_Filename.GetValue(), tex_loc))
	{
		// Let the billboard object load the texture now that it responds
		// to changes in the filename property.
		//matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc);

		// create actual point Billboard
		api3dBillboard *billboard = new api3dBillboard(NULL);
		//api3dBillboard *billboard = new api3dBillboard(pTexture);
		api3dScene::AddObject( billboard );

		billScriptObject *Billboard = new billScriptObject( billboard );
		Billboard->SetScriptData( i_Data );

		// Maintain the current icon visibility
		Billboard->ShowIcons(billObjectMgr::IconsVisible());

		if (i_Data.m_BaseData.m_Name.GetString().empty())
		{
			//	set a default name for the object
			nameString objName;
			create_default_name( i_Data.m_BaseData.m_Filename.GetValue(), objName );
			Billboard->SetName( objName );
		}
		return Billboard;
	}
	else
	{
		DBG_WARNING1("cannot find billboard (%s)", i_Data.m_BaseData.m_Filename.GetString().c_str() );
	}
	return 0;
}

//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  billObjectMgr::Init()
{
	//billDataMgr::AddDataChangedCallback(&l_DataChangedObj);
}

//--------------------------------------------------------------------
//  Clean up
//--------------------------------------------------------------------
void  billObjectMgr::CleanUp()
{
	//billDataMgr::RemoveDataChangedCallback(&l_DataChangedObj);
}

