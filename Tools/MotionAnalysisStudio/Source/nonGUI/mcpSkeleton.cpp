/********************************************************************************************\
**  mcpSkeleton.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "mcpSkeleton.hpp"

#include "chrLevel.hpp"
#include "mcpBoneFragment.hpp"
#include "mcpDirectController.hpp"
#include "mcpErrorHandler.hpp"
#include "mcpExceptionX.hpp"
#include "mcpLightMgr.hpp"
#include "mcpHTRParser.hpp"

#include "api3dObjectEntity.hpp"
#include "api3dObjectSimple.hpp"
#include "api3dShape.hpp"
#include "api3dScene.hpp"
#include "appSimTime.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "emdlAnimKeys.hpp"
#include "emdlHierTemplate.hpp"
#include "entAnimation.hpp"
#include "entEntity.hpp"
#include "entImport.hpp"
#include "envSTLHelpers.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "fsResourceFinderDir.hpp"
#include "g3dSceneNode.hpp"
#include "maConstants.hpp"
#include "matTextureMgr.hpp"
#include "mayExceptionX.hpp"
#include "mayFragInfo.hpp"
#include "itStringUtil.hpp"
#include "scObject.hpp"
#include "smdlHierarchyObject.hpp"

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
	api3dObjectSimple*			l_pGridObject = NULL;

	api3dObjectEntity*			l_pSkeleton = NULL;
	emdlHierTemplate*			l_pEntityTemplate = NULL;

	mcpModelChangeCallback *		l_ChangeCallback = NULL;
	mcpDirectController*			l_pController = NULL;

	// Set values from rotation and translation into matrix in node
	void set_xform_values(g3dSceneNode* io_pSceneNode,
						const maRotation &i_Rotate,
						const maVector3d &i_Translate)
	{
		maMatrix4x4& transform = io_pSceneNode->GetTransform();
		transform = i_Rotate.GetMatrix();

		//const maMatrix4x4* pOrientation = io_pSceneNode->GetOrientation();
		//// Check for HJoint, need to use orientation here
		//if (pOrientation)
		//{
		//	transform *= *pOrientation;
		//}

		transform.TranslateBy(i_Translate.m_X, i_Translate.m_Y, i_Translate.m_Z);
	}

	// Create hierarchy object to display skeleton
	void create_skeleton(const std::vector<mcpHTRSegmentData> &i_Segments)
	{
		DBG_ASSERT0(l_pSkeleton==NULL, "Need to free old skeleton");
		const int num_nodes = i_Segments.size();
		if (num_nodes == 0) return;

		// First build hierarchy without fragments
		std::vector<g3dSceneNode*> nodes(num_nodes);
		for (int i=0; i<num_nodes; ++i)
		{
			// First create the nodes
			const mcpHTRSegmentData &segment = i_Segments[i];
			nodes[i] = new g3dSceneNode();
			set_xform_values(nodes[i], segment.m_Rotation, segment.m_Position);
		}
		for (int i=0; i<num_nodes; ++i)
		{
			// Then hook up the children to the parent
			const mcpHTRSegmentData &segment = i_Segments[i];
			if (segment.m_ParentIndex >= 0)
			{
				nodes[ segment.m_ParentIndex ]->AddChild( nodes[i] );
			}	
		}

		// Now insert new nodes to handle fragment and the bone length
		for (int i=0; i<num_nodes; ++i)
		{
			const mcpHTRSegmentData &segment = i_Segments[i];
			g3dSceneNode *pNode = new g3dSceneNode(mcpBoneFragment::GetSharedFragment());
			pNode->SetDrawStyle(g3dSceneNode::e_Wireframe);

			pNode->GetTransform().ScaleBy(1, segment.m_BoneLength, 1);
			nodes[ i ]->AddChild( pNode );
		}

		l_pEntityTemplate = new emdlHierTemplate();
		l_pEntityTemplate->SetModel(nodes[0]);

		entEntity* pEnt = new entEntity( *l_pEntityTemplate );
		l_pSkeleton = new api3dObjectEntity(l_pEntityTemplate, pEnt); 
		api3dScene::AddObject(l_pSkeleton);

		mcpLightMgr::SetCenter(l_pSkeleton->GetWorldBox());
		mcpSkeleton::FocusCamera();

		// Create the controller
		g3dSceneNode *pBase = l_pSkeleton->Object()->GetBase();
		if (pBase->GetNumChildren() > 0)
		{
			// The base node was added from the hierarchy object. We want
			// to connect to its child in order to 
			l_pController = new mcpDirectController(i_Segments, pBase->GetChild(0));
			DBG_LOG2("Good connections: %d out of %d", l_pController->GetNumGoodConnections(), i_Segments.size());
		}
	}
		
	// clear skeleton related data
	void local_clear()
	{
		if (l_pSkeleton)
			api3dScene::RemoveObject(l_pSkeleton);
		delete l_pSkeleton;
		l_pSkeleton = NULL;
		if (l_pController)
			delete l_pController;
		l_pController = NULL;
		l_pEntityTemplate = NULL;	// owned by api3dObject

		l_ModelLocator.Clear();
		l_AnimLocator.Clear();
	}

} // end of namespace

//========================================================================
//	Initialize()
//========================================================================
void
mcpSkeleton::Initialize()
{
	matTextureMgr::SetAllowNullTextures(true);

	mcpBoneFragment::Initialize();

	l_pGridObject = api3dShape::CreateRectangle(maFloatRGBA(1, 1, 1, 1), 100, 100, 16, 16, false);
	l_pGridObject->SetOrientation(maRotation(-maConstants::c_fPI_Div_2, 0, 0));
	l_pGridObject->SetWireframe(true);
	api3dScene::AddObject(l_pGridObject);
}


//========================================================================
//	DeInitialize()
//========================================================================
void
mcpSkeleton::DeInitialize()
{
	Clear();
	mcpBoneFragment::DeInitialize();
	
	api3dScene::RemoveObject(l_pGridObject);
	delete l_pGridObject;
}

//============================================================================
//	SetModelChangeCallback
//============================================================================
void	mcpSkeleton::SetModelChangeCallback(mcpModelChangeCallback *i_Callback)
{
	l_ChangeCallback = i_Callback;
}

//============================================================================
//	Think - Handle material animation timing
//============================================================================
void	mcpSkeleton::Think()
{
}

//========================================================================
//	Clear
//========================================================================
void
mcpSkeleton::Clear()
{
	chrLevel::Clear();

	// clear just skeleton data
	local_clear();

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
}


//========================================================================
//	LoadModel loads geometry from the given locator
//========================================================================
void 
mcpSkeleton::LoadModel(const fsLocator& i_Locator)
{
	// clear just skeleton data
	local_clear();

	itString filename = i_Locator.GetLastName();
	if (filename.HasSubString(itString(".htr")) ||
		filename.HasSubString(itString(".HTR")))
	{
		try
		{
			mcpHTRData htr_data;
			mcpHTRParser::ReadData(i_Locator, htr_data);

			// Create scene node hierarchy for htr skeleton just read
			create_skeleton(htr_data.m_Segments);

			// Resize floor grid
			//if (l_pSkeleton)
			//{
			//	l_pGridObject->SetUniformScale(l_pSkeleton->GetWorldBox().GetRadius() / 10.0f);
			//}

			// [bga] - temping out load of animation until HTR files
			// work on models also.

			// If htr file contains animation, then create anim keys
			//if (l_pEntityTemplate && htr_data.m_bContainsAnimation)
			//{
			//	// Hierarchical animation
			//	emdlAnimKeys* anim_keys = new emdlAnimKeys;
			//	mcpHTRParser::ConvertAnimation(htr_data,
			//								anim_keys->Keys(),
			//								anim_keys->TranslateChannels(),
			//								anim_keys->RotateChannels(),
			//								anim_keys->ScaleChannels());
			//	//keys->SetNameOfRoot(root_name);
			//	l_pEntityTemplate->AddAnimKeys(anim_keys, i_Locator);

			//	entAnimation* animation = entImport::CreateAnimation(*anim_keys);
			//	if (htr_data.m_HeaderData.m_FrameRate > 0)
			//		animation->SetFrameRate( (float) htr_data.m_HeaderData.m_FrameRate );
			//	l_pEntityTemplate->AppendAnimation(animation);

			//	l_pSkeleton->GetEntity()->SetAnimation(animation, appSimTime::GetTime());
			//}
		}
		catch( const fsFileDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			mcpErrorHandler *handler = mcpErrorHandler::GetErrorHandler();
			if (handler)
				handler->FileDoesntExist(filename.c_str());
		}
		catch( const mcpInvalidModelFileX& i_Ex )
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
	else
	{
		chrLevel::LoadModel(i_Locator);
	}

	// Call user callback to update interface
	if (l_ChangeCallback)
		l_ChangeCallback->ModelChange();
	
}

//========================================================================
//	LoadAnimation loads animation for given model.
//========================================================================
void	mcpSkeleton::LoadAnimation(const fsLocator& i_AnimLocator)
{
	if (l_pEntityTemplate)
	{
		const entAnimKeys* anim_keys = l_pEntityTemplate->GetAnimKeys(i_AnimLocator);
		if (!anim_keys)
		{
			entAnimKeys* new_keys = entImport::LoadAnimKeys( i_AnimLocator );
			l_pEntityTemplate->AddAnimKeys(new_keys, i_AnimLocator );
			anim_keys = new_keys;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);
		l_pEntityTemplate->AppendAnimation(animation);
		l_pSkeleton->GetEntity()->SetAnimation(animation, appSimTime::GetTime());

		l_AnimLocator = i_AnimLocator;
	}
	else
	{
		chrLevel::LoadAnimation( i_AnimLocator );
	}
}


//========================================================================
//	Return directory from which to load textures
//========================================================================
const fsLocator&	mcpSkeleton::GetTextureDir()
{
	return l_TextureDirectory;
}


//========================================================================
// Focus camera on bounding box of object.
//========================================================================
void mcpSkeleton::FocusCamera()
{
	if (l_pSkeleton)
	{
		DBG_LOG1("Skeleton size: %f", l_pSkeleton->GetWorldBox().GetRadius() );
		cam3dMgr::FocusCamera(l_pSkeleton->GetWorldBox());
	}
	else
	{
		chrLevel::FocusCamera();
	}
}


//========================================================================
// Returns true if model is loaded.
//========================================================================
bool mcpSkeleton::HasModel()
{
	return (l_pSkeleton != NULL);
}

//========================================================================
// Create a skeleton from segment data
//========================================================================
void mcpSkeleton::CreateSkeleton(const std::vector<mcpHTRSegmentData> &i_Segments)
{
	create_skeleton(i_Segments);
}

//========================================================================
// Update hierarchy of skeleton using segment data
//========================================================================
void mcpSkeleton::UpdateHierarchy(const std::vector<mcpHTRSegmentData> &i_Segments)
{
	if (l_pController)
	{
		l_pController->Update(i_Segments);

		/*if (l_pSkeleton)
		{
			if (smdlGeoAnimatableObject *pAnimObj = dynamic_cast<smdlGeoAnimatableObject*>(l_pSkeleton->Object()))
				pAnimObj->MarkDirty();
		}*/
	}
}


