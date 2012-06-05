/*****************************************************************************
**	chtrObjectMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Systems/Character/Expressions/chtrExpressionDataParser.hpp"
#include "Systems/Character/Expressions/chtrExpressionObject.hpp"
#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/env/envThreadGroup.hpp"
#include "Core/fs/fsFileUtil.hpp"		// for debug only
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfFileX.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/ent/entEntityTemplate.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	int l_ObjectCounter = 0;
	bool l_bCreatingCharacter = false;

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
		} while ( !chtrObjectMgr::VerifyNodupeName(strName) );

		//DBG_LOG1( "Added -- name (%s)", strName );
	}

	//--------------------------------------------------------------------
	fsLocator get_texture_path(const fsLocator& i_Locator)
	{
		//	DEBUG only
		std::string data_dir;
		fsFileUtil::LocatorToANSIFilename( i_Locator, data_dir );
		DBG_LOG1( "Texture path (%s)", data_dir.c_str() );

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
	// load entity from .chx (usually loaded from .edf)
	//--------------------------------------------------------------------
	api3dObjectEntity* load_chx_entity(const fsLocator& i_Locator)
	{
		// Load Model in new style
		fsResourceFinderDir finder(get_texture_path(i_Locator));

		entModelTemplate* mdl_template = entImport::LoadGeometry(i_Locator, finder);
		if (mdl_template)
		{
			entEntityTemplate* ent_template = dynamic_cast<entEntityTemplate*>(mdl_template);
			if (ent_template)
			{
				entEntity* pEnt = new entEntity( *ent_template );

				// pass ownership of pointers to new object
				return new api3dObjectEntity(ent_template, pEnt);
			}
			else 
			{
				delete mdl_template; // not correct type
				DBG_ASSERT0(false, "Geometry loaded cannot be animated.");
			}
		}

		return NULL;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//int find_expression_by_name(const std::vector<chtrExpressionDef> &i_Expressions,
	//	const std::string &i_Name)
	//{
	//	for (int i=0; i<i_Expressions.size(); ++i)
	//	{
	//		if (i_Expressions[i].m_Name == i_Name)
	//			return i;
	//	}
	//	return -1;
	//}

	//--------------------------------------------------------------------
	// find full locator to filename
	//--------------------------------------------------------------------
	fsLocator locate_character(const itString& i_ObjectFilename)
	{
		//	grab the filename + load the object
		fsLocator file_loc;

		//	get the file path
		fsysFileList file_list;
		chtrGeomList::BuildFileList(file_list);
		file_list.GetFilePath(i_ObjectFilename, file_loc);
		file_loc.Push(i_ObjectFilename);

		return file_loc;
	}

	//--------------------------------------------------------------------
	// load entity and expressions from filename
	//--------------------------------------------------------------------
	void load_character(const fsLocator& i_FileLoc,
						api3dObjectEntity*& o_pEntitiyObj, 
						chtrExpressionObject*& o_pExpressionObj,
						fsLocator& o_GeomLoc)
	{
		o_pEntitiyObj = NULL;
		o_pExpressionObj = NULL;

		// Either load character data file (.chd) with expressions, or
		// just traditional geometry file (.chx)
		itString filename = i_FileLoc.GetLastName();
		if (filename.HasSubString(itString(".chd")))
		{
			//fsResourceTracker::MarkBegin(i_FileLoc);

			//// Loading expressions data file (.chd)
			//DBG_LOG0("Loading expressions");
			//chtrExpressionModelDef expression_data;
			//chtrExpressionDataParser::ReadData(i_FileLoc, expression_data);

			//// Now set up o_GeomLoc to point to geometry filename within .chd
			//fsysFileList file_list;
			//chtrGeomList::BuildFileList(file_list);
			//file_list.GetFilePath(expression_data.m_ModelFilename.GetValue(), o_GeomLoc);
			//o_GeomLoc.Push(expression_data.m_ModelFilename.GetValue());
		
			//std::string char_fname;
			//fsFileUtil::LocatorToANSIFilename( o_GeomLoc, char_fname );
			//DBG_LOG1( "Loading chx = [%s]", char_fname.c_str() );
			//o_pEntitiyObj = load_chx_entity(o_GeomLoc);

			////	Get character directory in order to find expressions
			//fsLocator char_dir(i_FileLoc);
			//char_dir.Pop();	// filename

			//// prepare a file list for the sub animation files
			//fsysFileList subanim_list;
			//chtrAnimList::BuildFileList(subanim_list, char_dir);

			//// Set up the idle animation
			//entEntityTemplate *ent_template = o_pEntitiyObj->GetEntityTemplate();
			//if (ent_template && expression_data.m_RestAnimFilename.GetValue().GetLength() > 0)
			//{
			//	fsLocator idle_loc;
			//	subanim_list.GetFilePath(expression_data.m_RestAnimFilename.GetValue(), idle_loc);
			//	idle_loc.Push(expression_data.m_RestAnimFilename.GetValue());

			//	// animation data owned by template in traditional way
			//	entAnimKeys* idle_keys = entImport::LoadAnimKeys( idle_loc );
			//	ent_template->AddAnimKeys(idle_keys, idle_loc );

			//	// assign idle animation to zero slot in template
			//	DBG_ASSERT0(o_pEntitiyObj->GetEntityTemplate()->GetNumAnimations() == 0, "Idle needs to go into zero slot");
			//	entAnimation* idle_anim = entImport::CreateAnimation(*idle_keys);
			//	ent_template->AppendAnimation(idle_anim);
			//}

			//// create the expression object
			////
			////if (o_pExpressionObj == NULL)
			////	o_pExpressionObj = new chtrExpressionObject(o_pEntitiyObj, o_GeomLoc);

			////const int nExpressions = expression_data.m_Expressions.size();
			////for (int e=0; e<nExpressions; e++)
			////{
			////	const chtrExpressionDef &exp  = expression_data.m_Expressions[e];
			////	DBG_LOG3("Adding expression %02d of %02d: %s", e, nExpressions, exp.m_Name.GetValue().c_str());

			////	o_pExpressionObj->AddExpression(exp.m_Name.GetValue(),
			////									exp.m_Filename.GetValue());
			////}

			////std::set<int> grouped_expressions;

			////const int nPairs = expression_data.m_MultiPairs.size();
			////for (int p=0; p<nPairs; p++)
			////{
			////	int left = find_expression_by_name(expression_data.m_Expressions, 
			////									   expression_data.m_MultiPairs[p].m_Left.GetValue());
			////	int right = find_expression_by_name(expression_data.m_Expressions, 
			////										expression_data.m_MultiPairs[p].m_Right.GetValue());
			////	if (left >= 0 && right >= 0)
			////	{
			////		o_pExpressionObj->AddPairProperty(expression_data.m_MultiPairs[p].m_Name.GetValue(),
			////										left, right);
			////		grouped_expressions.insert( left );
			////		grouped_expressions.insert( right );
			////	}
			////	else
			////	{
			////		DBG_WARNING2("Expression not found by name: %s %s", expression_data.m_MultiPairs[p].m_Left.GetValue().c_str(), expression_data.m_MultiPairs[p].m_Right.GetValue().c_str());
			////	}
			////}

			////// Now create single properties for ungrouped expressions
			////for (int e=0; e<nExpressions; e++)
			////{
			////	const chtrExpressionDef &exp  = expression_data.m_Expressions[e];
			////	if (grouped_expressions.find(e) == grouped_expressions.end())		// if not already used...
			////	{
			////		o_pExpressionObj->AddSingleProperty(expression_data.m_Expressions[e].m_Name.GetValue(), e);
			////	}
			////}

			//fsResourceTracker::MarkEnd(i_FileLoc);
		}
		else
		{
			// Loading geometry directly (.chx)
			o_GeomLoc = i_FileLoc;
			o_pEntitiyObj = load_chx_entity(i_FileLoc);
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chtrScriptObject* create_script_object(const chtrScriptData &i_Data, 
										   api3dObjectEntity* i_pEntObject, 
										   chtrExpressionObject* i_pExpressionObj, 
										   const fsLocator& i_GeomLoc, 
										   const fsLocator& i_CharDir)
	{
		l_bCreatingCharacter = true;

		//	add it to the scene
		api3dScene::AddObject(i_pEntObject);

		// create script object for 3d object
		chtrScriptObject *pCharacter = new chtrScriptObject(i_pEntObject, i_pExpressionObj, 
			i_GeomLoc, i_CharDir);
		pCharacter->SetScriptData( i_Data );

		// Maintain the current icon visibility
		pCharacter->ShowIcons(chtrObjectMgr::IconsVisible());

		if (i_Data.m_BaseData.m_Name.GetString().empty())
		{
			//	set a default name for the character
			nameString objName;
			itString filename = i_Data.m_BaseData.m_Filename.GetValue();
			create_default_name( filename, objName );
			pCharacter->SetName( objName );
		}

		l_bCreatingCharacter = false;
		return pCharacter;

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	struct sCharacterLoadThread
	{
		const fsLocator& i_FileLoc;
		api3dObjectEntity*& o_pEntitiyObj; 
		chtrExpressionObject*& o_pExpressionObj;
		fsLocator& o_GeomLoc;

		void operator()()
		{
			load_character(i_FileLoc, o_pEntitiyObj, o_pExpressionObj, o_GeomLoc);
		}
	};

}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrScriptObject* chtrObjectCreator::Create(const chtrScriptData &i_Data)
{
	// create actual point Character

	//	grab the filename + load the object
	//
	itString filename = i_Data.m_BaseData.m_Filename.GetValue();
	fsLocator file_loc = locate_character(filename);

	std::string char_fname;
	fsFileUtil::LocatorToANSIFilename( file_loc, char_fname );
	DBG_LOG1( "Creating character = [%s]", char_fname.c_str() );

	//	store the directory so some drivers can get access to it.
	fsLocator char_dir(file_loc);
	char_dir.Pop();	// filename

	// Either load character data file (.chd) with expressions, or
	// just traditional geometry file (.chx)
	api3dObjectEntity* entity_obj = NULL;
	chtrExpressionObject* expression_obj = NULL;
	fsLocator geom_loc;
	load_character(file_loc, entity_obj, expression_obj, geom_loc);

	// Create script object from the entity, expressions and data
	chtrScriptObject *pCharacter = create_script_object(i_Data, 
														entity_obj, 
														expression_obj, 
														geom_loc, 
														char_dir);
	return pCharacter;
}

//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  chtrObjectMgr::Init()
{
	//chtrScriptDataMgr::AddDataChangedCallback(&l_DataChangedObj);
}

//--------------------------------------------------------------------
//  Clean up
//--------------------------------------------------------------------
void  chtrObjectMgr::CleanUp()
{
	//chtrScriptDataMgr::RemoveDataChangedCallback(&l_DataChangedObj);
}

//--------------------------------------------------------------------
//  Reload geometry of character with given index
//--------------------------------------------------------------------
void  chtrObjectMgr::ReloadCharacter(int i_Index)
{
	//	prevent the changing of filename to cause an infinite loop
	if (l_bCreatingCharacter)
		return;

	// I think the safest way to handle this is to delete the old
	// object and create a new one from the data, recreating drivers
	// and channels for the new object. Instead of trying to remap
	// all of the old pointers.
	chtrScriptObject* old_obj = sm_Objects[i_Index];
	chtrScriptData data = old_obj->GetScriptData();
	chtrScriptObject* new_obj = chtrObjectCreator::Create(data);
	if (new_obj)
	{
		// Transfer light set assignments from old object to new object
		ltstLightSetMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		// Transfer light set assignments from old object to new object
		evmtEnvironmentMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		// Transfer layers
		lyerLayerMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		// Transfer groups
		grpsGroupMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		sm_Objects[i_Index] = new_obj;
		SelectObject(i_Index);
		delete old_obj;
	}
}

//--------------------------------------------------------------------
//  Change geometry of character with given index
//--------------------------------------------------------------------
void  chtrObjectMgr::ChangeFilename(int i_Index, const itString& i_Filename)
{
	if (sm_Objects.size() == 0)
		return;
	//	prevent the changing of filename to cause an infinite loop
	if (l_bCreatingCharacter)
		return;

	// I think the safest way to handle this is to delete the old
	// object and create a new one from the data, recreating drivers
	// and channels for the new object. Instead of trying to remap
	// all of the old pointers.
	chtrScriptObject* old_obj = sm_Objects[i_Index];
	chtrScriptData data = old_obj->GetScriptData();
	data.m_BaseData.m_Filename = i_Filename;
	data.m_Materials.clear();	// Clear out material overrides when switching to new model.
	chtrScriptObject* new_obj = chtrObjectCreator::Create(data);
	if (new_obj)
	{
		// Transfer light set assignments from old object to new object
		ltstLightSetMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		// Transfer light set assignments from old object to new object
		evmtEnvironmentMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		// Transfer layers
		lyerLayerMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		// Transfer groups
		grpsGroupMgr::ReplaceObject(old_obj->GetPickObject(), new_obj->GetPickObject());

		sm_Objects[i_Index] = new_obj;
		SelectObject(i_Index);
		delete old_obj;
	}
}

//--------------------------------------------------------------------
// Set subdivision level being used.
//--------------------------------------------------------------------
void chtrObjectMgr::SetSubdivLevel(int i_SubdivLevel)
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


//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void chtrObjectMgr::SetData(const chtrCharactersData &i_Data)
{
	envSTLHelpers::DeleteContainer(sm_Objects);

	const int num_objects = i_Data.m_Items.size();

	// Load all geometry first in multiple threads
	std::vector<fsLocator> file_locs(num_objects);
	std::vector<fsLocator> geom_locs(num_objects);
	std::vector<api3dObjectEntity*> entities(num_objects);
	std::vector<chtrExpressionObject*> expressions(num_objects);
	{
		// No threading of loads for now
		//envThreadGroup char_loader;
		for (int i=0; i<num_objects; i++)
		{
			file_locs[i] = locate_character(i_Data.m_Items[i].m_BaseData.m_Filename.GetValue());

			// next two lines are threaded version
			//sCharacterLoadThread load_char_thread = {file_locs[i], entities[i], expressions[i], geom_locs[i]};
			//char_loader.AddThread(load_char_thread);

			// non-threaded version
			try
			{
				load_character(file_locs[i], entities[i], expressions[i], geom_locs[i]);
			}
			catch ( gfInvalidFileBinX& /*i_Ex*/ )
			{
				entities[i] = NULL;

				char message[512];
				std::string bad_file;
				fsFileUtil::LocatorToANSIFilename( file_locs[i], bad_file );
				sprintf( message, "Encountered an invalid or junk file (%s)", bad_file.c_str() );
				DBG_ERROR1("%s", message);
				guiMessageBox::Show(message, "Error", guiMessageBox::e_OKOnly);
			}

		}
		//char_loader.WaitForAll();
	}

	// Create the character script objects
	for (int i=0; i<num_objects; i++)
	{
		//sm_Objects.push_back( charObjectCreator::Create(i_Data.m_Items[i]) );

		if (entities[i] != NULL)
		{
			fsLocator char_dir(file_locs[i]);
			char_dir.Pop();
			chtrScriptObject* pSO = create_script_object(i_Data.m_Items[i], entities[i], expressions[i], geom_locs[i], char_dir);
			sm_Objects.push_back( pSO );
		}
	}
}

//--------------------------------------------------------------------
// Make sure that all geometry is visible for rendering
//--------------------------------------------------------------------
void chtrObjectMgr::ConfirmGeometryVisible()
{
	for (int i=0; i<sm_Objects.size(); i++)
	{
		sm_Objects[i]->SetEditorVisible(true);
	}
}
