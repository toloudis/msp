/********************************************************************************************\
**  chrLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "nonGUI/chrLevel.hpp"

#include "nonGUI/chrDataParser.hpp"
#include "nonGUI/chrErrorHandler.hpp"
#include "nonGUI/chrLightMgr.hpp"

#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appSimTime.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/ent/entEntityTemplate.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
//#include "smdlSingleSkinObject.hpp"
//#include "smdlJointedObject.hpp"

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
api3dObject*				l_pObject = NULL;
//entEntityTemplate*			l_EntityTemplate = NULL;
api3dObjectEntity*			l_pEntity = NULL;		  // not-owned, can be NULL
smdlSubdivCharacter*		l_pCharacterModel = NULL; // not-owned, can be NULL

chrModelChangeCallback *		l_ChangeCallback = NULL;

const int					c_MaxVertsPerInitialLevel = 30000;

//struct TargetIndex
//{
//	TargetIndex(int s, int t) : m_SkinIndex(s),m_TargetIndex(t) {}
//	int m_SkinIndex;
//	int m_TargetIndex;
//};
//
//struct NamedTarget 
//{
//	NamedTarget(): m_Weight(0.0f) {}
//	NamedTarget(const std::string i_Name): m_Name(i_Name), m_Weight(0.0f) {}
//	std::string m_Name;
//	std::vector<TargetIndex> m_Targets;
//	float m_Weight;
//};
//
//std::vector<NamedTarget> l_Targets;


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

struct GroupPair
{
	std::string m_Name;
	int m_LeftIndex, m_RightIndex;
	float m_Weight;
};
std::vector<GroupPair> l_GroupPairs;

struct GroupFour
{
	std::string m_Name;
	int m_LeftIndex, m_RightIndex;
	int m_DownIndex, m_UpIndex;
	float m_Weight1, m_Weight2;
};
std::vector<GroupFour> l_GroupFours;

//====================================================================
// return index for expression with name, -1 if not found
//====================================================================
int find_name(const std::string &i_Name)
{
	for (int i=0; i<l_Expressions.size(); ++i)
	{
		if (l_Expressions[i].m_Name == i_Name)
			return i;
	}

	return -1;
}

//====================================================================
//====================================================================
float get_initial_group_weight(int i_LeftIndex, int i_RightIndex)
{
	float weight_left = l_Expressions[i_LeftIndex].m_Weight;
	float weight_right = l_Expressions[i_RightIndex].m_Weight;

	float group_weight = 0;
	if (weight_left > weight_right)
	{
		group_weight = -weight_left; // negate because left goes opposite direction (-1..0)

		// Other dimension of expression is forced to zero
		chrLevel::SetExpressionWeight(i_RightIndex, 0);
	}
	else
	{
		group_weight = weight_right;

		// Other dimension of expression is forced to zero
		chrLevel::SetExpressionWeight(i_LeftIndex, 0);
	}
	return group_weight;
}

//====================================================================
// Set weights of expressions within a dimension of a group
//====================================================================
void set_group_weight(float i_Weight, int i_LeftIndex, int i_RightIndex)
{
	if (i_Weight < 0)
	{
		chrLevel::SetExpressionWeight(i_LeftIndex, -i_Weight);
		chrLevel::SetExpressionWeight(i_RightIndex, 0);
	}
	else
	{
		chrLevel::SetExpressionWeight(i_RightIndex, i_Weight);
		chrLevel::SetExpressionWeight(i_LeftIndex, 0);
	}
}

//====================================================================
// An expression with given index is being deleted, drop down the
// indices of expresssions higher than this index.
//====================================================================
void shift_group_indices(int i_Index)
{
	for (int i=0; i<l_GroupPairs.size(); ++i)
	{
		DBG_ASSERT1(l_GroupPairs[i].m_LeftIndex != i_Index, "Group %s is using expression that was deleted.", l_GroupPairs[i].m_Name.c_str());
		DBG_ASSERT1(l_GroupPairs[i].m_RightIndex != i_Index, "Group %s is using expression that was deleted.", l_GroupPairs[i].m_Name.c_str());

		if (l_GroupPairs[i].m_LeftIndex > i_Index)
			l_GroupPairs[i].m_LeftIndex--;
		if (l_GroupPairs[i].m_RightIndex > i_Index)
			l_GroupPairs[i].m_RightIndex--;
	}
}

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
//int find_named_target(const std::string& i_Name)
//{
//	int nTargets = l_Targets.size();
//	for (int t=0; t<nTargets; t++)
//	{
//		if (l_Targets[t].m_Name == i_Name)
//			return t;
//	}
//	return -1;
//}

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
//void gather_targets(smdlSubdivCharacter* i_pCharacterModel)
//{
//	int nSkins = i_pCharacterModel->GetNumSkins();
//	for (int s=0; s<nSkins; s++)
//	{
//		int nTargets = i_pCharacterModel->GetNumMorphTargets(s);
//		for (int t=0; t<nTargets; t++)
//		{
//			std::string name = i_pCharacterModel->GetMorphTargetName(s, t);
//			int index = find_named_target(name);
//			if (index < 0)
//			{
//				// add new target
//				NamedTarget target(name);
//				target.m_Targets.push_back(TargetIndex(s,t));
//				l_Targets.push_back(target);
//			}
//			else
//			{
//				// add influence to existing target
//				NamedTarget &target = l_Targets[index];
//				target.m_Targets.push_back(TargetIndex(s,t));
//			}
//		}
//	}
//}

//========================================================================
// private function used by Load() and LoadModel()
//========================================================================
void internal_load_model(const fsLocator& i_Locator)
{
	try
	{
/*		// Load Model in new style
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

					chrLightMgr::SetCenter(l_pObject->GetWorldBox());
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
*/
		// Load using api3dImport, will handle static and animatable geometry
		l_pObject = api3dImport::LoadObject(i_Locator);
		if (l_pObject)
		{
			// Try to see if the object is animatable
			l_pEntity = dynamic_cast<api3dObjectEntity*>(l_pObject);
			if (l_pEntity)
			{
				// Have an object that can animate, now see if it
				// is a subdivision character. 
				l_pCharacterModel = dynamic_cast<smdlSubdivCharacter*>(l_pEntity->Object());
				if (l_pCharacterModel)
				{
					//If so, get out the morph targets
					//gather_targets(l_pCharacterModel);
					set_initial_subdiv_level(l_pCharacterModel);
				}
			}

			api3dScene::AddObject(l_pObject);

			chrLightMgr::SetCenter(l_pObject->GetWorldBox());
			chrLevel::FocusCamera();
		}
		l_ModelLocator = i_Locator;
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
	}
	catch( const mdlInvalidModelFileX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
		if (handler)
			handler->InvalidFileFormat(filename.c_str());
	}
	catch( ... )
	{
		chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
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
	smdlSubdivCharacter::SetMaxSubdivLevel(1);
#else
	smdlSubdivCharacter::SetMaxSubdivLevel(1);
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
void	chrLevel::SetModelChangeCallback(chrModelChangeCallback *i_Callback)
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
	if (l_pObject)
		api3dScene::RemoveObject(l_pObject);
	delete l_pObject;
	l_pObject = NULL;
	l_pEntity = NULL;	// just cast of the l_pObject, not owned
	l_pCharacterModel = NULL; // just cast, not owned

	l_ModelLocator.Clear();
	l_AnimLocator.Clear();
//	l_Targets.clear();
	l_Expressions.clear();
	l_GroupPairs.clear();
	l_GroupFours.clear();

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

		const int nPairs = l_GroupPairs.size();
		for (int p=0; p<nPairs; p++)
		{
			data.AddPair(l_GroupPairs[p].m_Name, 
				l_Expressions[l_GroupPairs[p].m_LeftIndex].m_Name, 
				l_Expressions[l_GroupPairs[p].m_RightIndex].m_Name);
		}

		const int nFours = l_GroupFours.size();
		for (int f=0; f<nFours; f++)
		{
			data.AddFour(l_GroupFours[f].m_Name, 
				l_Expressions[l_GroupFours[f].m_LeftIndex].m_Name, 
				l_Expressions[l_GroupFours[f].m_RightIndex].m_Name, 
				l_Expressions[l_GroupFours[f].m_DownIndex].m_Name, 
				l_Expressions[l_GroupFours[f].m_UpIndex].m_Name);
		}

		chrDataParser::WriteData( i_Locator, data );
	}
	catch( const fsReadOnlyX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileReadOnly(filename.c_str());
	}
	catch( ... )
	{
		chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
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

		// Asign grouped pairs of expressions
		const int nPairs = data.m_MultiPairs.size();
		for (int p=0; p<nPairs; p++)
		{
			chrLevel::AddExpressionPair(data.m_MultiPairs[p].m_Name,
					data.m_MultiPairs[p].m_Left,
					data.m_MultiPairs[p].m_Right);
		}

		// Asign grouped fours of expressions
		const int nFours = data.m_MultiFours.size();
		for (int f=0; f<nFours; f++)
		{
			chrLevel::AddExpressionFour(data.m_MultiFours[f].m_Name,
					data.m_MultiFours[f].m_Left,
					data.m_MultiFours[f].m_Right,
					data.m_MultiFours[f].m_Down,
					data.m_MultiFours[f].m_Up);
		}
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
		if (handler)
			handler->FileDoesntExist(filename.c_str());
	}
	catch( ... )
	{
		chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
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
	if (l_pEntity)
	{
		entEntityTemplate *pTemplate = l_pEntity->GetEntityTemplate();
		if (pTemplate)
		{
			const entAnimKeys* anim_keys = pTemplate->GetAnimKeys(i_AnimLocator);
			if (!anim_keys)
			{
				entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
				pTemplate->AddAnimKeys(new_keys, i_AnimLocator );
				anim_keys = new_keys;
			}

			entAnimation* animation = entImport::CreateAnimation(*anim_keys);
			pTemplate->AppendAnimation(animation);

			l_pEntity->GetEntity()->ClearAnimation();

			if (l_pEntity->GetEntity()->CheckAnimation(animation))
			{
				l_pEntity->GetEntity()->SetAnimation(animation, appSimTime::GetTime());

				l_AnimLocator = i_AnimLocator;

				// Force through one frame of animation in order to 
				// get an updated bounding box
				l_pEntity->AnimateSingleFrame();
				chrLevel::FocusCamera();
			}
			else
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_AnimLocator, filename);
				chrErrorHandler *handler = chrErrorHandler::GetErrorHandler();
				if (handler)
					handler->IncompatibleAnimation(filename.c_str());		
			}
		}
	}
}

//========================================================================
//	LoadSubAnimation loads sub-animation for given model.
//========================================================================
void	chrLevel::LoadSubAnimation(const fsLocator& i_AnimLocator)
{
	if (l_pEntity)
	{
		entEntityTemplate *pTemplate = l_pEntity->GetEntityTemplate();
		if (pTemplate)
		{
			const entAnimKeys* anim_keys = pTemplate->GetAnimKeys(i_AnimLocator);
			if (!anim_keys)
			{
				entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
				pTemplate->AddAnimKeys(new_keys, i_AnimLocator );
				anim_keys = new_keys;
			}

			entAnimation* animation = entImport::CreateAnimation(*anim_keys);
			pTemplate->AppendAnimation(animation);

			const bool bPreserve = false;
			l_pEntity->GetEntity()->AddSubAnimation(animation, appSimTime::GetTime(), bPreserve);
		}
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
// Returns true if a model with subdivision surfaces is loaded.
//========================================================================
bool chrLevel::HasSubdivModel()
{
	return (l_pCharacterModel != NULL);
}

//========================================================================
// Returns true if an animatable model is loaded.
//========================================================================
bool chrLevel::CanAnimate()
{
	return (l_pEntity != NULL);
}

//========================================================================
// Control over low resolution model display
//========================================================================
bool chrLevel::HasLowResModel()
{
//	return (l_pCharacterModel) ? l_pCharacterModel->HasLowResolutionModel() : false;
//	return (l_pEntity) ? l_pEntity->Object()->HasLowResolutionModel() : false;

	api3dObjectSingle *pSingle = dynamic_cast<api3dObjectSingle*>(l_pObject);
	return (pSingle) ? pSingle->Object()->HasLowResolutionModel() : false;
}
bool chrLevel::GetUseLowResModel()
{
	return (l_pObject) ? l_pObject->GetLowResolution() : false;
}
void chrLevel::SetUseLowResModel(bool i_bLowRes)
{
	if (l_pObject)
		l_pObject->SetLowResolution(i_bLowRes);
}

//========================================================================
// Control over joint bone display
//		0 - model only, 1 - joints only, 2 - model and joints
//========================================================================
bool chrLevel::HasBoneDisplay()
{
	return (l_pCharacterModel != NULL);
}
int chrLevel::GetBoneDisplay()
{
	if (l_pCharacterModel)
	{
		return (int) l_pCharacterModel->GetJointDisplay();
	}
	return 0;
}
void chrLevel::SetBoneDisplay(int i_DisplayMode)
{
	if (l_pCharacterModel)
	{
		l_pCharacterModel->SetJointDisplay(smdlSubdivCharacter::JointDisplay(i_DisplayMode));
	}
}

//========================================================================
// Return number of named morph targets
//========================================================================
//int chrLevel::GetNumMorphTargets()
//{
//	return l_Targets.size();
//}

//========================================================================
// Return name of morph target with given index
//========================================================================
//std::string chrLevel::GetMorphTargetName(int i_Index)
//{
//	return l_Targets[i_Index].m_Name;
//}

//========================================================================
// MorphTargetWeight - usually number from 0-1 to set influence 
//	of this morph target.
//========================================================================
//float chrLevel::GetMorphTargetWeight(int i_Index)
//{
//	return l_Targets[i_Index].m_Weight;
//}
//void chrLevel::SetMorphTargetWeight(int i_Index, float i_Weight)
//{
//	// still to do
//	//DBG_LOG2("Target %d, weight %f", i_Index, i_Weight);
//	NamedTarget &target = l_Targets[i_Index];
//	target.m_Weight = i_Weight;
//
//	if (l_pCharacterModel)
//	{
//		int num = target.m_Targets.size();
//		for (int i=0; i<num; i++)
//		{
//			TargetIndex &index = target.m_Targets[i];
//			l_pCharacterModel->SetMorphTargetWeight(index.m_SkinIndex, index.m_TargetIndex, i_Weight);
//		}
//	}
//}

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
	if (l_pEntity)
	{
		entEntityTemplate *pTemplate = l_pEntity->GetEntityTemplate();
		if (pTemplate)
		{
			const entAnimKeys* anim_keys = pTemplate->GetAnimKeys(i_AnimLocator);
			if (!anim_keys)
			{
				DBG_LOG0("Loading subanimation for expression");
				entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
				pTemplate->AddAnimKeys(new_keys, i_AnimLocator );
				anim_keys = new_keys;
			}

			entAnimation* animation = entImport::CreateAnimation(*anim_keys);
			pTemplate->AppendAnimation(animation);

			const bool bPreserve = true;
			entSubAnimation *sub_anim = l_pEntity->GetEntity()->AddSubAnimation(animation, appSimTime::GetTime(), bPreserve);
			// Start with 0.0 weight for expressions
			l_pEntity->GetEntity()->SetSubAnimationBlend(sub_anim, 0.0f);

			// add new target
			Expression exp(sub_anim, i_Name, i_AnimLocator);
			l_Expressions.push_back(exp);
		}
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
	if (l_pEntity)
	{
		l_pEntity->GetEntity()->SetSubAnimationBlend(exp.m_pAnim, i_Weight);
	}
}

//========================================================================
// Remove expression with given index.
//========================================================================
void chrLevel::DeleteExpression(int i_Index)
{
	if (!l_pEntity) return;

	DBG_ASSERT0(i_Index >= 0 && i_Index < l_Expressions.size(), "Index out of range");
	Expression &exp = l_Expressions[i_Index];
	l_pEntity->GetEntity()->RemoveSubAnimation(exp.m_pAnim);
	l_Expressions.erase(l_Expressions.begin() + i_Index);

	shift_group_indices(i_Index);
}

//========================================================================
// Return true if the expression with the given index is used
//	in a paired expression.
//========================================================================
bool chrLevel::IsExpressionGrouped(int i_Index)
{
	for (int i=0; i<l_GroupPairs.size(); i++)
	{
		if (l_GroupPairs[i].m_LeftIndex == i_Index 
			|| l_GroupPairs[i].m_RightIndex == i_Index)
			return true;
	}
	for (int i=0; i<l_GroupFours.size(); i++)
	{
		if (l_GroupFours[i].m_LeftIndex == i_Index 
			|| l_GroupFours[i].m_RightIndex == i_Index
			|| l_GroupFours[i].m_DownIndex == i_Index
			|| l_GroupFours[i].m_UpIndex == i_Index)
			return true;
	}
	return false;
}

//========================================================================
// Return names of expressions that are not grouped yet
//========================================================================
void chrLevel::GetUngroupedNames(std::vector<std::string> &o_Names)
{
	for (int i=0; i<l_Expressions.size(); i++)
	{
		if (!IsExpressionGrouped(i))
			o_Names.push_back(l_Expressions[i].m_Name);
	}
}

//========================================================================
//	Pair two expressions in order to control them with one slider
//========================================================================
void chrLevel::AddExpressionPair(const std::string &i_Name,
								 const std::string &i_Left, 
								 const std::string &i_Right)
{
	GroupPair pair;
	pair.m_Name = i_Name;
	pair.m_LeftIndex = find_name(i_Left);
	pair.m_RightIndex = find_name(i_Right);

	if ((pair.m_LeftIndex >= 0) && (pair.m_RightIndex >= 0))
	{
		pair.m_Weight = get_initial_group_weight(pair.m_LeftIndex, pair.m_RightIndex);
		l_GroupPairs.push_back( pair );
	}
}

//========================================================================
// Return number of grouped pairs of expressions
//========================================================================
int chrLevel::GetNumGroupPairs()
{
	return l_GroupPairs.size();
}

//========================================================================
// Return name of grouped pair with given index
//========================================================================
std::string chrLevel::GetGroupPairName(int i_Index)
{
	return l_GroupPairs[i_Index].m_Name;
}
void chrLevel::SetGroupPairName(int i_Index, const std::string& i_Name)
{
	l_GroupPairs[i_Index].m_Name = i_Name;
}

//========================================================================
// GroupPairWeight - usually number from -1..1 to set influence 
//	of the pair of expressions at once.
//========================================================================
float chrLevel::GetGroupPairWeight(int i_Index)
{
	return l_GroupPairs[i_Index].m_Weight;
}
void chrLevel::SetGroupPairWeight(int i_Index, float i_Weight)
{
	GroupPair &grp = l_GroupPairs[i_Index];
	grp.m_Weight = i_Weight;

	set_group_weight(i_Weight, grp.m_LeftIndex, grp.m_RightIndex);
}

//========================================================================
// Remove pair with given index.
//========================================================================
void chrLevel::DeleteGroupPair(int i_Index, bool i_bDeleteExpressions)
{
	if (!l_pEntity) return;

	DBG_ASSERT0(i_Index >= 0 && i_Index < l_GroupPairs.size(), "Index out of range");

	// Get expression indices it was using
	int left = l_GroupPairs[i_Index].m_LeftIndex;
	int right = l_GroupPairs[i_Index].m_RightIndex;

	// Remove the group
	l_GroupPairs.erase(l_GroupPairs.begin() + i_Index);

	if (i_bDeleteExpressions)
	{
		// Then remove the supporting expressions
		DeleteExpression(left);

		if (right != left)
		{
			// If right index was greater than left index, 
			// then have to shift this expression down one
			// in order to match the new index for that expression
			if (right > left)
				right--;

			DeleteExpression(right);
		}
	}
}


//========================================================================
//	Group four expressions in order to control them with one slider
//========================================================================
void chrLevel::AddExpressionFour(const std::string &i_Name,
								 const std::string &i_Left, 
								 const std::string &i_Right, 
								 const std::string &i_Down, 
								 const std::string &i_Up)
{
	GroupFour four;
	four.m_Name = i_Name;
	four.m_LeftIndex = find_name(i_Left);
	four.m_RightIndex = find_name(i_Right);
	four.m_DownIndex = find_name(i_Down);
	four.m_UpIndex = find_name(i_Up);

	if ((four.m_LeftIndex >= 0) && (four.m_RightIndex >= 0)
		&& (four.m_DownIndex >= 0) && (four.m_UpIndex >= 0))
	{
		four.m_Weight1 = get_initial_group_weight(four.m_LeftIndex, four.m_RightIndex);
		four.m_Weight2 = get_initial_group_weight(four.m_DownIndex, four.m_UpIndex);

		l_GroupFours.push_back( four );
	}
}

//========================================================================
// Return number of grouped fours of expressions
//========================================================================
int chrLevel::GetNumGroupFours()
{
	return l_GroupFours.size();
}

//========================================================================
// Return name of grouped four with given index
//========================================================================
std::string chrLevel::GetGroupFourName(int i_Index)
{
	return l_GroupFours[i_Index].m_Name;
}
void chrLevel::SetGroupFourName(int i_Index, const std::string& i_Name)
{
	l_GroupFours[i_Index].m_Name = i_Name;
}

//========================================================================
// GroupFourWeight - usually number from -1..1 to set influence 
//	of the four of expressions at once.
//========================================================================
void chrLevel::GetGroupFourWeight(int i_Index, float &o_Weight1, float &o_Weight2)
{
	o_Weight1 = l_GroupFours[i_Index].m_Weight1;
	o_Weight2 = l_GroupFours[i_Index].m_Weight2;
}
void chrLevel::SetGroupFourWeight(int i_Index, float i_Weight1, float i_Weight2)
{
	GroupFour &grp = l_GroupFours[i_Index];
	grp.m_Weight1 = i_Weight1;
	grp.m_Weight2 = i_Weight2;

	set_group_weight(i_Weight1, grp.m_LeftIndex, grp.m_RightIndex);
	set_group_weight(i_Weight2, grp.m_DownIndex, grp.m_UpIndex);
}

//========================================================================
// Remove group four with given index.
//========================================================================
void chrLevel::DeleteGroupFour(int i_Index, bool i_bDeleteExpressions)
{
	if (!l_pEntity) return;

	DBG_ASSERT0(i_Index >= 0 && i_Index < l_GroupPairs.size(), "Index out of range");

	// Get expression indices it was using
	std::vector<int> to_delete(4);
	to_delete[0] = l_GroupFours[i_Index].m_LeftIndex;
	to_delete[1] = l_GroupFours[i_Index].m_RightIndex;
	to_delete[2] = l_GroupFours[i_Index].m_DownIndex;
	to_delete[3] = l_GroupFours[i_Index].m_UpIndex;

	// sort list in order to erase indices in order without
	// needing to alter indices while we delete.
	std::sort(to_delete.begin(), to_delete.end()); 

	// Remove the group
	l_GroupFours.erase(l_GroupFours.begin() + i_Index);

	// Then remove the supporting expressions
	for (int i=3; i>=0; i--)
	{
		// Then remove the supporting expressions
		DeleteExpression(to_delete[i]);
	}
}
