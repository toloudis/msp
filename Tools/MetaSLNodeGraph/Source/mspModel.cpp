/*****************************************************************************
**	mspModel.cpp
**
**	 mspModel represents the currently viewed model and animation.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "mspModel.hpp"
#include "mspViewSettings.hpp"

#include "Core/App/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/Ent/entEntity.hpp"
#include "Graphics/Ent/entAnimation.hpp"
#include "Graphics/Ent/entAnimInstance.hpp"
#include "Graphics/Ent/entAnimTemplate.hpp"
#include "Graphics/Ent/entImport.hpp"
#include "Graphics/Ent/entModelInstance.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gui/guiMainWindow.hpp"
#include "Tool/gui/guiMessageBox.hpp"

namespace
{
	fsLocator l_Filename;
	fsLocator l_AnimLocator;
	api3dObject *l_pObject = NULL;
	api3dObjectEntity *l_pEntity = NULL; // simply cast of the l_pObject, not separate object
	entAnimTemplate *l_pAnimTemplate = NULL;
	smdlSubdivCharacter *l_pCharacterModel = NULL; // simply cast of the l_pObject, can be NULL
}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  mspModel::Clear()
{
	l_Filename.Clear();
	l_AnimLocator.Clear();

	if (l_pObject)
		api3dScene::RemoveObject(l_pObject);
	delete l_pObject;
	l_pObject = NULL;
	l_pEntity = NULL;
	l_pCharacterModel = NULL;
	delete l_pAnimTemplate;
	l_pAnimTemplate = NULL;

	mspViewSettings::sm_CurrentFrame.SetValue(0);
	mspViewSettings::sm_ModelAnimNumFrames.SetValue(0);
	if (mspViewSettings::sm_UseModelRange.GetValue())
	{
		mspViewSettings::sm_PlaybackBegin.SetValue(0);
		mspViewSettings::sm_PlaybackEnd.SetValue(0);
	}
}

//--------------------------------------------------------------------
// LoadModel
//--------------------------------------------------------------------
bool  mspModel::LoadModel(const fsLocator &i_Locator)
{
	// If this asserts, then we have to call Clear() sometime (after safe load?)
	DBG_ASSERT(l_pObject == NULL, "Safe to assume that Clear() was already called?");

	l_pObject = api3dImport::LoadObject(i_Locator);

	if (l_pObject)
	{
		// Attempt cast to entity object to see if the model is animatable
		l_pEntity = dynamic_cast<api3dObjectEntity*>(l_pObject);
		if (l_pEntity)
		{
			// Create an anim template to handle loading of animations
			l_pAnimTemplate = new entAnimTemplate();

			// Cast to character model to handle subdiv levels
			l_pCharacterModel = dynamic_cast<smdlSubdivCharacter*>(l_pEntity->Object());
		}
		else
			l_pCharacterModel = NULL;

		api3dScene::AddObject(l_pObject);

		// scale the model down if it is "too big"
		if (l_pObject->GetWorldBox().GetRadius() > 1000)
		{
			l_pObject->SetScale(l_pObject->GetScale() * 0.01f);
		}

		mspModel::FocusCamera();
		l_Filename = i_Locator;

		return true;
	}
	else
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Locator, filename);
		DBG_WARNING("NULL object returned from LoadObject: %s" << filename );
		std::string msg = "File is not a supported geometry file format: " + filename;
		guiMessageBox::Show(msg.c_str(), "Error");
	}

	return false;
}

//--------------------------------------------------------------------
// LoadAnimation
//--------------------------------------------------------------------
bool  mspModel::LoadAnimation(const fsLocator &i_AnimLocator )
{
	if (!l_pObject) return false;

	api3dObjectEntity *pEntity = dynamic_cast<api3dObjectEntity*>(l_pObject);
	if (pEntity)
	{
		DBG_ASSERT(l_pAnimTemplate, "Assuming anim template created when entity was loaded");
		const entAnimKeys* anim_keys = l_pAnimTemplate->GetAnimKeys(i_AnimLocator);
		if (!anim_keys)
		{
			entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
			l_pAnimTemplate->AddAnimKeys(new_keys, i_AnimLocator );
			anim_keys = new_keys;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);
		l_pAnimTemplate->AppendAnimation(animation);

		if (pEntity->GetEntity()->CheckAnimation(animation))
		{
			pEntity->GetEntity()->ClearAnimation();

			appSimTime::ResetTime();
			pEntity->GetEntity()->SetAnimation(animation, appSimTime::GetTime());

			l_AnimLocator = i_AnimLocator;
			mspViewSettings::sm_CurrentFrame.SetValue(0);
			mspViewSettings::sm_ModelAnimFilename.SetValue(i_AnimLocator.GetLastName());
			mspViewSettings::sm_ModelAnimNumFrames.SetValue(animation->GetNumFrames());
			if (mspViewSettings::sm_UseModelRange.GetValue())
			{		
				float anim_shift = mspViewSettings::sm_ModelAnimShift.GetValue();
				mspViewSettings::sm_PlaybackBegin.SetValue(anim_shift);
				mspViewSettings::sm_PlaybackEnd.SetValue(animation->GetNumFrames() + anim_shift);
			}

			// Force through one frame of animation in order to 
			// get an updated bounding box
			pEntity->AnimateSingleFrame();
			mspModel::FocusCamera();
			return true;
		}
		else
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_AnimLocator, filename);
			std::string msg = "Animation file is not compatible with geometry: " + filename;
			guiMessageBox::Show(msg.c_str(), "Incompatible Animation");
		}
	
	}

	return false;
}

//--------------------------------------------------------------------
// ModelFilename
//--------------------------------------------------------------------
const fsLocator& mspModel::GetModelFilename()
{
	return l_Filename;
}

//--------------------------------------------------------------------
// Returns directory to use as initial location of animation
//	open file dialog.
//--------------------------------------------------------------------
fsLocator mspModel::GetAnimationDir()
{
	fsLocator init_dir;
	if (l_AnimLocator.GetNumNames() > 1)
	{
		// If we have already loaded an animation, then
		// look for the next animation in the same directory
		init_dir = l_AnimLocator;
		init_dir.Pop();
	}
	else if (l_Filename.GetNumNames() > 1)
	{
		// Otherwise, look for the next animation in directory
		// where the geometry was loaded.
		init_dir = l_Filename;
		init_dir.Pop();
	}
	return init_dir;
}

//--------------------------------------------------------------------
// FocusCamera - center camera on model
//--------------------------------------------------------------------
void mspModel::FocusCamera()
{
	if (l_pObject)
		cam3dMgr::FocusCamera(l_pObject->GetWorldBox());
}

//--------------------------------------------------------------------
// Returns true if model is loaded and is an animatable model
//--------------------------------------------------------------------
bool mspModel::CanLoadAnimation()
{
	return (l_pEntity != NULL);
}

//--------------------------------------------------------------------
// Returns true if model is loaded.
//--------------------------------------------------------------------
bool mspModel::HasModel()
{
	return (l_pObject != NULL);
}

//--------------------------------------------------------------------
// Returns true if a model with subdivision surfaces is loaded.
//--------------------------------------------------------------------
bool mspModel::HasSubdivModel()
{
	return (l_pCharacterModel != NULL);
}

//--------------------------------------------------------------------
// Returns true if animation is loaded
//--------------------------------------------------------------------
bool mspModel::HasAnimation()
{
	// could store a boolean for this flag
	return (l_AnimLocator.GetNumNames() > 0);
}

//--------------------------------------------------------------------
// Return the frame of the animation for the given time
//--------------------------------------------------------------------
float mspModel::ComputeFrame(float i_Time)
{
	if (l_pEntity && l_pEntity->GetEntity()->GetAnimInstance())
	{
		entAnimInstance *pAnimInstance = l_pEntity->GetEntity()->GetAnimInstance();
		anFrameAnimation &animation = const_cast<anFrameAnimation&>(pAnimInstance->GetAnim());

		// Combine the model animation shift here
		float anim_shift =  mspViewSettings::sm_ModelAnimShift.GetValue();

		// Pass playback range to animation
		animation.SetStartFrame(mspViewSettings::sm_PlaybackBegin.GetValue() - anim_shift);
		animation.SetEndFrame(mspViewSettings::sm_PlaybackEnd.GetValue() - anim_shift);

		return (pAnimInstance->ComputeFrame(i_Time) + anim_shift);
	}

	return 0.0f;
}

//--------------------------------------------------------------------
// Return the simulation time such that the animation will play 
//	the given frame number
//--------------------------------------------------------------------
float mspModel::ComputeTime(float i_Frame)
{
	if (l_pEntity && l_pEntity->GetEntity()->GetAnimInstance())
	{
		// Combine the model animation shift here
		float anim_shift =  mspViewSettings::sm_ModelAnimShift.GetValue();

		float time = (i_Frame - anim_shift) / l_pEntity->GetEntity()->GetAnimInstance()->GetFrameRate();
		return time;
	}
	return 0.0f;
}

//--------------------------------------------------------------------
// Control over low resolution model display
//--------------------------------------------------------------------
bool mspModel::HasLowResModel()
{
	api3dObjectSingle *pSingle = dynamic_cast<api3dObjectSingle*>(l_pObject);
	return (pSingle) ? pSingle->Object()->HasLowResolutionModel() : false;
}
bool mspModel::GetUseLowResModel()
{
	return (l_pObject) ? l_pObject->GetLowResolution() : false;
}
void mspModel::SetUseLowResModel(bool i_bLowRes)
{
	if (l_pObject)
		l_pObject->SetLowResolution(i_bLowRes);
}


//--------------------------------------------------------------------
// Return current subdivision level being used.
//--------------------------------------------------------------------
int mspModel::GetCurrentSubdivLevel()
{
	return (l_pCharacterModel) ? l_pCharacterModel->GetCurrentSubdivLevel() : 0;

}

//--------------------------------------------------------------------
// Set the current subdivision level being used, this should be 
// a level less than or equal to the return value of 
// GetMaxSubdivLevel()
//--------------------------------------------------------------------
void mspModel::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	if (l_pCharacterModel)
		l_pCharacterModel->SetCurrentSubdivLevel(i_SubdivLevel);
}

//--------------------------------------------------------------------
// Return material used by the model by name
//--------------------------------------------------------------------
matMaterial* mspModel::GetMaterialByName(const std::string &i_MaterialName)
{
	if (l_pEntity)
	{
		entModelInstance* pInstance = l_pEntity->GetModelInstance();
		std::vector<matMaterial*>& materials = pInstance->Materials();
		for (int i=0; i<materials.size(); ++i)
		{
			if (materials[i]->GetName() == i_MaterialName)
				return materials[i];
		}
	}
	return NULL;
}