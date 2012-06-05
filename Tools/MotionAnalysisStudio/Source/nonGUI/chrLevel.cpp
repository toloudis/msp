/********************************************************************************************\
**  chrLevel.cpp
**
**      Keeps track of expressions in viewed character.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "chrLevel.hpp"

#include "chrDataParser.hpp"
#include "mcpDirectController.hpp"
#include "mcpErrorHandler.hpp"
#include "mcpLightMgr.hpp"
#include "mcpHTRData.hpp"

#include "an2StateAnimation.hpp"
#include "anKeyAnimation.hpp"
#include "api3dObjectEntity.hpp"
#include "api3dScene.hpp"
#include "appSimTime.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "entEntity.hpp"
#include "entEntityTemplate.hpp"
#include "entImport.hpp"
#include "envSTLHelpers.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "fsResourceFinderDir.hpp"
#include "g3dFragment.hpp"
#include "g3dSceneNode.hpp"
#include "matMatAnim.hpp"
#include "matMaterial.hpp"
#include "matTextureMgr.hpp"
#include "mayExceptionX.hpp"
#include "mayFragInfo.hpp"
#include "itStringUtil.hpp"
#include "scObject.hpp"
#include "smdlSubdivCharacter.hpp"
#include "smdlSingleSkinObject.hpp"
#include "smdlJointedObject.hpp"

#include <algorithm>
#include <memory>

//========================================================================
// Local variables and functions
//========================================================================
namespace
{
fsLocator					l_ModelLocator;
fsLocator					l_AnimLocator;
fsLocator					l_TextureDirectory;
//entModelTemplate*			l_ModelTemplate = NULL;
//api3dObjectGeom*			l_pObject = NULL;
entEntityTemplate*			l_EntityTemplate = NULL;
api3dObjectEntity*			l_pObject = NULL;
smdlSubdivCharacter*		l_pCharacterModel = NULL; // not-owned, can be NULL

mcpModelChangeCallback *		l_ChangeCallback = NULL;
mcpDirectController*			l_pController = NULL;

const int					c_MaxVertsPerInitialLevel = 100000;

struct TargetIndex
{
	TargetIndex(int s, int t) : m_SkinIndex(s),m_TargetIndex(t) {}
	int m_SkinIndex;
	int m_TargetIndex;
};

struct NamedTarget 
{
	NamedTarget(): m_Weight(0.0f) {}
	NamedTarget(const std::string i_Name): m_Name(i_Name), m_Weight(0.0f) {}
	std::string m_Name;
	std::vector<TargetIndex> m_Targets;
	float m_Weight;
};

std::vector<NamedTarget> l_Targets;


struct Expression 
{
	Expression(entSubAnimation* i_pAnim)
		: m_pAnim(i_pAnim), m_Weight(0.0f) {}
	Expression(entSubAnimation* i_pAnim, const std::string i_Name, const fsLocator &i_AnimLocator)
		: m_pAnim(i_pAnim), m_Name(i_Name), m_AnimLocator(i_AnimLocator), m_Weight(0.0f) {}

	std::string m_Name;
	fsLocator m_AnimLocator;
	entSubAnimation* m_pAnim;
	float m_Weight;
};

std::vector<Expression> l_Expressions;

//====================================================================
// given geometry filename, find texture directory
//====================================================================
fsLocator find_texture_path(const fsLocator &i_Locator)
{
	// just return directory from filename for now
	fsLocator dir = i_Locator;
	dir.Pop();

	// look up a directory and then down into "Textures"
	fsLocator tex_dir = dir;
	tex_dir.Pop();
	tex_dir.Push("Textures");

	if (fsFileUtil::DirectoryExists(tex_dir))
		return tex_dir;

	// otherwise, just look in the directory where the model is
	return dir;
}

//====================================================================
// given data (.chd) filename, find geometry directory
//====================================================================
fsLocator find_model_path(const fsLocator &i_Locator)
{
	// just return directory from filename for now
	fsLocator dir = i_Locator;
	dir.Pop();

	// look up a directory and then down into "Models"
	fsLocator geo_dir = dir;
	geo_dir.Pop();
	geo_dir.Push("Models");

	if (fsFileUtil::DirectoryExists(geo_dir))
		return geo_dir;

	// otherwise, just look in the directory where the data file is
	return dir;
}

//====================================================================
// returns -1 if not found.
//====================================================================
int find_named_target(const std::string& i_Name)
{
	int nTargets = l_Targets.size();
	for (int t=0; t<nTargets; t++)
	{
		if (l_Targets[t].m_Name == i_Name)
			return t;
	}
	return -1;
}

//====================================================================
//====================================================================
void set_initial_subdiv_level(smdlSubdivCharacter* i_pCharacterModel)
{
	const int max_level = i_pCharacterModel->GetMaxSubdivLevel();
	int level;
	for (level=max_level; level>0; level--)
	{
		if (i_pCharacterModel->GetNumFacesAtLevel(level) < c_MaxVertsPerInitialLevel)
		{
			break;
		}
	}

	// set to a lower level if necessary
	if (level != i_pCharacterModel->GetCurrentSubdivLevel())
		i_pCharacterModel->SetCurrentSubdivLevel(level);
}

//====================================================================
//====================================================================
void gather_targets(smdlSubdivCharacter* i_pCharacterModel)
{
	int nSkins = i_pCharacterModel->GetNumSkins();
	for (int s=0; s<nSkins; s++)
	{
		int nTargets = i_pCharacterModel->GetNumMorphTargets(s);
		for (int t=0; t<nTargets; t++)
		{
			std::string name = i_pCharacterModel->GetMorphTargetName(s, t);
			int index = find_named_target(name);
			if (index < 0)
			{
				// add new target
				NamedTarget target(name);
				target.m_Targets.push_back(TargetIndex(s,t));
				l_Targets.push_back(target);
			}
			else
			{
				// add influence to existing target
				NamedTarget &target = l_Targets[index];
				target.m_Targets.push_back(TargetIndex(s,t));
			}
		}
	}
}

//========================================================================
// private function used by Load() and LoadModel()
//========================================================================
void internal_load_model(const fsLocator& i_Locator)
{
	try
	{
		// Load Model in new style
		l_TextureDirectory = find_texture_path(i_Locator);
		fsResourceFinderDir finder(l_TextureDirectory);
		entModelTemplate* ent_template = entImport::LoadGeometry(i_Locator, finder);
		if (ent_template)
		{
			l_EntityTemplate = 	dynamic_cast<entEntityTemplate*>(ent_template);
			if (l_EntityTemplate)
			{
				//scObject* obj_ptr = entImport::CreateObject( *ent_template );
				entEntity* pEnt = new entEntity( *l_EntityTemplate );
				l_pCharacterModel = dynamic_cast<smdlSubdivCharacter*>(pEnt->Object());
				if (l_pCharacterModel)
				{
					gather_targets(l_pCharacterModel);
					set_initial_subdiv_level(l_pCharacterModel);
				}

				// pass ownership of pointers to new object
				l_ModelLocator = i_Locator;
				l_pObject = new api3dObjectEntity(l_EntityTemplate, pEnt);
				if (l_pObject)
				{
					api3dScene::AddObject(l_pObject);

					mcpLightMgr::SetCenter(l_pObject->GetWorldBox());
					chrLevel::FocusCamera();
				}
				
				l_ModelLocator = i_Locator;
			}
			else 
			{
				delete ent_template; // not correct type
				DBG_ASSERT0(false, "Geometry loaded cannot be animated.");
			}
		}

	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
	}
	catch( const mayInvalidModelFileX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
		if (handler)
			handler->InvalidFileFormat(filename.c_str());
	}
	catch( ... )
	{
		mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

}

} // end of namespace

//========================================================================
//	Initialize()
//========================================================================
void
chrLevel::Initialize()
{
	matTextureMgr::SetAllowNullTextures(true);
#ifdef _DEBUG
	smdlSubdivCharacter::SetMaxSubdivLevel(0);
#else
	smdlSubdivCharacter::SetMaxSubdivLevel(3);
#endif
}


//========================================================================
//	DeInitialize()
//========================================================================
void
chrLevel::DeInitialize()
{
	Clear();
}

//============================================================================
//	SetModelChangeCallback
//============================================================================
void	chrLevel::SetModelChangeCallback(mcpModelChangeCallback *i_Callback)
{
	l_ChangeCallback = i_Callback;
}

//============================================================================
//	Think - Handle material animation timing
//============================================================================
void	chrLevel::Think()
{
}

//========================================================================
//	Clear
//========================================================================
void
chrLevel::Clear()
{
	if (l_pController)
		delete l_pController;
	l_pController = NULL;
	if (l_pObject)
		api3dScene::RemoveObject(l_pObject);
	delete l_pObject;
	l_pObject = NULL;
	l_pCharacterModel = NULL;
	l_EntityTemplate = NULL;	// owned by api3dObject

	l_ModelLocator.Clear();
	l_AnimLocator.Clear();
	l_Targets.clear();
	l_Expressions.clear();

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
}


//========================================================================
//	Save saves a level to a given locator
//========================================================================
void
chrLevel::Save(const fsLocator& i_Locator)
{
	// Can't save empty file
	if (!HasModel())
		return;

	try
	{
		chrData data;
		if (l_ModelLocator.GetNumNames() > 0)
			data.m_ModelFilename = l_ModelLocator.GetLastName();
		if (l_AnimLocator.GetNumNames() > 0)
			data.m_RestAnimFilename = l_AnimLocator.GetLastName();

		const int nExpressions = l_Expressions.size();
		for (int e=0; e<nExpressions; e++)
		{
			data.AddExpression(l_Expressions[e].m_Name, l_Expressions[e].m_AnimLocator.GetLastName());
		}

		chrDataParser::WriteData( i_Locator, data );
	}
	catch( const fsReadOnlyX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileReadOnly(filename.c_str());
	}
	catch( ... )
	{
		mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

}

//========================================================================
//	Load() loads .chd character definition file.
//========================================================================
void 
chrLevel::Load(const fsLocator& i_Locator)
{
	Clear();

	try
	{		
		chrData data;
		chrDataParser::ReadData( i_Locator, data );

		fsLocator geo_dir = find_model_path(i_Locator);

		// Load model file
		fsLocator model_file = geo_dir;
		model_file.Push( data.m_ModelFilename );
		internal_load_model(model_file);

		// Load animation if needed,
		if (data.m_RestAnimFilename.GetLength() > 0)
		{
			fsLocator anim_file = geo_dir;
			anim_file.Push( data.m_RestAnimFilename );
			chrLevel::LoadAnimation(anim_file);
		}

		// Load expressions
		const int nExpressions = data.m_Expressions.size();
		for (int e=0; e<nExpressions; e++)
		{
			fsLocator subanim_file = geo_dir;
			subanim_file.Push( data.m_Expressions[e].m_Filename );
			chrLevel::AddExpression(subanim_file, data.m_Expressions[e].m_Name);
		}
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
	}
	catch( ... )
	{
		mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
		if (handler)
			handler->GeneralError();
		throw;
	}

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
}


//========================================================================
//	LoadModel loads geometry from the given locator
//========================================================================
void 
chrLevel::LoadModel(const fsLocator& i_Locator)
{
	Clear();

	internal_load_model(i_Locator);

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
}

//========================================================================
//	LoadAnimation loads animation for given model.
//========================================================================
void	chrLevel::LoadAnimation(const fsLocator& i_AnimLocator)
{
	if (l_EntityTemplate)
	{
		const entAnimKeys* anim_keys = l_EntityTemplate->GetAnimKeys(i_AnimLocator);
		if (!anim_keys)
		{
			entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
			l_EntityTemplate->AddAnimKeys(new_keys, i_AnimLocator );
			anim_keys = new_keys;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);
		l_EntityTemplate->AppendAnimation(animation);

		int num_anims = l_EntityTemplate->GetNumAnimations();
		l_pObject->GetEntity()->SetAnimation(num_anims-1, appSimTime::GetTime());

		l_AnimLocator = i_AnimLocator;
	}
}

//========================================================================
//	LoadSubAnimation loads sub-animation for given model.
//========================================================================
void	chrLevel::LoadSubAnimation(const fsLocator& i_AnimLocator)
{
	if (l_EntityTemplate)
	{
		const entAnimKeys* anim_keys = l_EntityTemplate->GetAnimKeys(i_AnimLocator);
		if (!anim_keys)
		{
			entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
			l_EntityTemplate->AddAnimKeys(new_keys, i_AnimLocator );
			anim_keys = new_keys;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);
		l_EntityTemplate->AppendAnimation(animation);

		const bool bPreserve = false;
		l_pObject->GetEntity()->AddSubAnimation(animation, appSimTime::GetTime(), bPreserve);
	}
}

//========================================================================
//	SwitchModel changes base geometry and idle animation, leaving
//	subanims in place.
//========================================================================
void chrLevel::SwitchModel(const fsLocator& i_ModelLocator,
						   const fsLocator& i_IdleAnimLocator)
{
	// Gather up data to represent current expressions
	chrData data;
	const int nExpressions = l_Expressions.size();
	for (int e=0; e<nExpressions; e++)
	{
		data.AddExpression(l_Expressions[e].m_Name, l_Expressions[e].m_AnimLocator.GetLastName());
	}


	Clear();

	internal_load_model(i_ModelLocator);

	if (i_IdleAnimLocator.GetNumNames() > 0)
	{
		chrLevel::LoadAnimation(i_IdleAnimLocator);
	}

	fsLocator geo_dir = find_model_path(i_ModelLocator);

	// Load expressions back in
	for (int e=0; e<nExpressions; e++)
	{
		fsLocator subanim_file = geo_dir;
		subanim_file.Push( data.m_Expressions[e].m_Filename );
		chrLevel::AddExpression(subanim_file, data.m_Expressions[e].m_Name);
	}

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
}

//========================================================================
//	Return directory from which to load textures
//========================================================================
const fsLocator&	chrLevel::GetTextureDir()
{
	return l_TextureDirectory;
}


//========================================================================
// Focus camera on bounding box of object.
//========================================================================
void chrLevel::FocusCamera()
{
	if (l_pObject)
	{
		DBG_LOG1("Model size: %f", l_pObject->GetWorldBox().GetRadius() );
		cam3dMgr::FocusCamera(l_pObject->GetWorldBox());
	}
}


//========================================================================
// Returns true if model is loaded.
//========================================================================
bool chrLevel::HasModel()
{
	return (l_pObject != NULL);
}

//========================================================================
// Return number of named morph targets
//========================================================================
int chrLevel::GetNumMorphTargets()
{
	return l_Targets.size();
}

//========================================================================
// Return name of morph target with given index
//========================================================================
std::string chrLevel::GetMorphTargetName(int i_Index)
{
	return l_Targets[i_Index].m_Name;
}

//========================================================================
// MorphTargetWeight - usually number from 0-1 to set influence 
//	of this morph target.
//========================================================================
float chrLevel::GetMorphTargetWeight(int i_Index)
{
	return l_Targets[i_Index].m_Weight;
}
void chrLevel::SetMorphTargetWeight(int i_Index, float i_Weight)
{
	// still to do
	//DBG_LOG2("Target %d, weight %f", i_Index, i_Weight);
	NamedTarget &target = l_Targets[i_Index];
	target.m_Weight = i_Weight;
	int num = target.m_Targets.size();
	for (int i=0; i<num; i++)
	{
		TargetIndex &index = target.m_Targets[i];
		l_pCharacterModel->SetMorphTargetWeight(index.m_SkinIndex, index.m_TargetIndex, i_Weight);
	}
}

//========================================================================
// Return current subdivision level being used.
//========================================================================
int chrLevel::GetCurrentSubdivLevel()
{
	return (l_pCharacterModel) ? l_pCharacterModel->GetCurrentSubdivLevel() : 0;

}

//========================================================================
// Set the current subdivision level being used, this should be 
// a level less than or equal to the return value of 
// GetMaxSubdivLevel()
//========================================================================
void chrLevel::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	if (l_pCharacterModel)
		l_pCharacterModel->SetCurrentSubdivLevel(i_SubdivLevel);
}

//========================================================================
//	AddExpression loads sub-animation as expression that can then
//	be blended with an alpha from 0-1
//========================================================================
void chrLevel::AddExpression(const fsLocator& i_AnimLocator, const std::string &i_Name)
{
	if (l_EntityTemplate)
	{
		const entAnimKeys* anim_keys = l_EntityTemplate->GetAnimKeys(i_AnimLocator);
		if (!anim_keys)
		{
			DBG_LOG0("Loading subanimation for expression");
			entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
			l_EntityTemplate->AddAnimKeys(new_keys, i_AnimLocator );
			anim_keys = new_keys;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);
		l_EntityTemplate->AppendAnimation(animation);

		const bool bPreserve = true;
		entSubAnimation *sub_anim = l_pObject->GetEntity()->AddSubAnimation(animation, appSimTime::GetTime(), bPreserve);
		// Start with 0.0 weight for expressions
		l_pObject->GetEntity()->SetSubAnimationBlend(sub_anim, 0.0f);

		// add new target
		Expression exp(sub_anim, i_Name, i_AnimLocator);
		l_Expressions.push_back(exp);
	}

}

//========================================================================
// Return number of named morph targets
//========================================================================
int chrLevel::GetNumExpressions()
{
	return l_Expressions.size();
}

//========================================================================
// Return name of morph target with given index
//========================================================================
std::string chrLevel::GetExpressionName(int i_Index)
{
	return l_Expressions[i_Index].m_Name;
}
void chrLevel::SetExpressionName(int i_Index, const std::string& i_Name)
{
	l_Expressions[i_Index].m_Name = i_Name;
}

//========================================================================
// ExpressionWeight - usually number from 0-1 to set influence 
//	of this morph target.
//========================================================================
float chrLevel::GetExpressionWeight(int i_Index)
{
	return l_Expressions[i_Index].m_Weight;
}
void chrLevel::SetExpressionWeight(int i_Index, float i_Weight)
{
	Expression &exp = l_Expressions[i_Index];
	exp.m_Weight = i_Weight;

	// alter blend for subanimation 
	l_pObject->GetEntity()->SetSubAnimationBlend(exp.m_pAnim, i_Weight);
}

//========================================================================
// Remove expression with given index.
//========================================================================
void chrLevel::DeleteExpression(int i_Index)
{
	DBG_ASSERT0(i_Index >= 0 && i_Index < l_Expressions.size(), "Index out of range");
	Expression &exp = l_Expressions[i_Index];
	l_pObject->GetEntity()->RemoveSubAnimation(exp.m_pAnim);
	l_Expressions.erase(l_Expressions.begin() + i_Index);
}


//========================================================================
// Attach real-time controlled from segment data
//========================================================================
void chrLevel::AttachController(const std::vector<mcpHTRSegmentData> &i_Segments)
{
	RemoveController();

	// Create the controller
	g3dSceneNode *pBase = l_pObject->Object()->GetBase();
	if (pBase->GetNumChildren() > 0)
	{
		// The base node was added from the hierarchy object. We want
		// to connect to its child in order to 
		l_pController = new mcpDirectController(i_Segments, pBase->GetChild(0));
		DBG_LOG2("Good connections: %d out of %d", l_pController->GetNumGoodConnections(), i_Segments.size());
	}
}

//========================================================================
// Remove controller
//========================================================================
void chrLevel::RemoveController()
{
	if (l_pController)
	{
		delete l_pController;
		l_pController = NULL;
	}
}

//========================================================================
// Update hierarchy of skeleton using segment data
//========================================================================
void chrLevel::UpdateHierarchy(const std::vector<mcpHTRSegmentData> &i_Segments)
{
	if (l_pController)
	{
		l_pController->Update(i_Segments);
	}
	else
	{
		AttachController(i_Segments);
	}
}