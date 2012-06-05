/*****************************************************************************
**	propObjectMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Object/propObjectMgr.hpp"

#include "Systems/Props/GUI/propGeomList.hpp"

#include "Tool/pick3d/pick3dPickList.hpp"

#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Core/app/appTime.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/ent/entEntityTemplate.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Core/env/envThreadGroup.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"			// for debugging only
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfPaths.hpp"
//#include "Core/it/itStringUtil.hpp"


namespace
{
	//----------------------------------------------------------------------------
	void set_prop_data(api3dObject *io_Prop, const propScriptData &i_Data)
	{
		io_Prop->SetPosition( i_Data.m_BaseData.m_Position.GetValue() );
		io_Prop->SetOrientation( i_Data.m_BaseData.m_Orientation.GetQuaternion() );
	}

	int l_ObjectCounter = 0;


	//--------------------------------------------------------------------
	fsLocator get_texture_path(const fsLocator& i_Locator)
	{
		//	DEBUG only
		//std::string data_dir;
		//fsFileUtil::LocatorToANSIFilename( i_Locator, data_dir );
		//DBG_LOG1( "Texture path (%s)", data_dir.c_str() );

		// remove the filename and then go up one directory from "Model"
		// come down "Textures"
		//
		fsLocator dir( i_Locator );
		dir.Pop();
		dir.Pop();
		dir.Push(gfPaths::GetSubPath(gfPaths::e_Textures));
		return dir;
	}

	//--------------------------------------------------------------------
	// the new way to load props without EDF files
	//--------------------------------------------------------------------
	api3dObjectEntity* create_object_entity(const fsLocator& i_ObjectFile)
	{
		// Load Model in new style
		fsResourceFinderDir finder(get_texture_path(i_ObjectFile));

		entModelTemplate* mdl_template = entImport::LoadGeometry(i_ObjectFile, finder);
		if (mdl_template)
		{
			entEntity* pEnt = new entEntity( *mdl_template );

			// pass ownership of pointers to new object
			return new api3dObjectEntity(mdl_template, pEnt);
			
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	// find full locator to filename
	//--------------------------------------------------------------------
	fsLocator locate_entity(const itString& i_ObjectFilename)
	{
		//	grab the filename + load the object
		fsLocator file_loc;

		//	get the file path
		fsysFileList file_list;
		propGeomList::BuildFileList(file_list);
		file_list.GetFilePath(i_ObjectFilename, file_loc);
		file_loc.Push(i_ObjectFilename);

		return file_loc;
	}

	//--------------------------------------------------------------------
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
		} while ( !propObjectMgr::VerifyNodupeName(strName) );

		//DBG_LOG1( "Added -- name (%s)", strName );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propScriptObject* create_script_object(const propScriptData &i_Data, 
										   api3dObjectEntity* i_pEntObject, 
										   const fsLocator& i_FileLoc)
	{
		//	set the data and add it to the object list
		set_prop_data(i_pEntObject, i_Data);

		//	add it to the scene
		api3dScene::AddObject(i_pEntObject);

		// create object for 3d icon, attach it to true Prop by id
		//int Prop_id = sm_Objects.size();
		propScriptObject *pProp = new propScriptObject(i_pEntObject, i_FileLoc);
		pProp->SetScriptData( i_Data );

		// Maintain the current icon visibility
		pProp->ShowIcons(propObjectMgr::IconsVisible());

		// Give default name if necessary
		if (i_Data.m_BaseData.m_Name.GetString().empty())
		{
			//	set a default name for the prop
			nameString objName;
			create_default_name( i_Data.m_BaseData.m_Filename.GetValue(), objName );
			pProp->SetName( objName );
		}

		return pProp;
	}

	struct sEntityLoadThread
	{
		const fsLocator& i_Locator;
		api3dObjectEntity*& o_Entity;

		void operator()()
		{
			o_Entity = create_object_entity(i_Locator);
		}
	};
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
propScriptObject* propObjectCreator::Create(const propScriptData &i_Data)
{
	//	grab the filename + load the object
	fsLocator file_loc = locate_entity(i_Data.m_BaseData.m_Filename.GetValue());
	
	//	std::string dir;
	//	fsFileUtil::LocatorToANSIFilename( file_loc, dir );
	//	DBG_WARNING1( "Loading prop = [%s]", dir.c_str() );

	api3dObjectEntity* pObj = create_object_entity(file_loc);

	// Create script object from entity
	return create_script_object(i_Data, pObj, file_loc);
}

//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  propObjectMgr::Init()
{
}

//--------------------------------------------------------------------
//  Clean up
//--------------------------------------------------------------------
void  propObjectMgr::CleanUp()
{
}



//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void propObjectMgr::SetData(const propPropsData &i_Data)
{
	envSTLHelpers::DeleteContainer(sm_Objects);

	const int num_objects = i_Data.m_Items.size();

	// Load all geometry first in multiple threads
	std::vector<fsLocator> locators(num_objects);
	std::vector<api3dObjectEntity*> entities(num_objects);
	{
		// No threading of loads for now
		//envThreadGroup entity_loader;
		for (int i=0; i<num_objects; i++)
		{
			locators[i] = locate_entity(i_Data.m_Items[i].m_BaseData.m_Filename.GetValue());

			// next two lines are threaded version
			//sEntityLoadThread load_entity_thread = {locators[i], entities[i]};
			//entity_loader.AddThread(load_entity_thread);

			// next line is non-threaded
			entities[i] = create_object_entity(locators[i]);

		}
		//entity_loader.WaitForAll();
	}

	// Create the prop script objects
	for (int i=0; i<num_objects; i++)
	{
		//sm_Objects.push_back( propObjectCreator::Create(i_Data.m_Items[i]) );
		sm_Objects.push_back( create_script_object(i_Data.m_Items[i], entities[i], locators[i]) );
	}
}

//--------------------------------------------------------------------
// Make sure that all geometry is visible for rendering
//--------------------------------------------------------------------
void propObjectMgr::ConfirmGeometryVisible()
{
	for (int i=0; i<sm_Objects.size(); i++)
	{
		sm_Objects[i]->SetEditorVisible(true);
	}
}


//--------------------------------------------------------------------
// Set subdivision level being used.
//--------------------------------------------------------------------
void propObjectMgr::SetSubdivLevel(int i_SubdivLevel)
{
	int i = 0;
	int goal_subdiv_level = i_SubdivLevel;
	const int num_objects = sm_Objects.size();
	while (i < num_objects)
	{
		try
		{
			sm_Objects[i]->SetSubdivLevel(goal_subdiv_level);
			i++;
		}
		catch (const std::bad_alloc&)
		{
			std::string message("Out of memory when setting subdivision level on character ");
			message += sm_Objects[i]->GetName().GetString();
			DBG_ERROR0(message.c_str());

			message += ". Switching to base mesh subdivision level 0.";
			guiMessageBox::Show(message.c_str(), "Not enough memory to subdivide.");

			// Jump down to the base mesh level in order to free up some memory.
			// Note that this will not compress the expanded subdiv networks,
			// it will just free some space by shrinking the fragment sizes
			// and it will make sure that all objects are on the same subdiv level.
			if (goal_subdiv_level > 0)
			{
				goal_subdiv_level = 0; // new goal level is the base mesh
				i = 0; // start over with the first object
			}
			else
			{
				// We failed while trying to reach the base mesh,
				// there is no help for us now.
				// re-throw the exception up to stop the program.
				throw;
			}
		}
	}

	// If we decided to reduce the subdiv level to a lower level
	// in order to avoid a out-of-memory error, then set the
	// subdiv level with the api3d setting in order to make sure
	// the interface matches this decision
	if (goal_subdiv_level != i_SubdivLevel)
	{
		api3dSubdiv::SetSubdivLevel(goal_subdiv_level);
	}
}
