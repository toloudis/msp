/*****************************************************************************
**	smdlSubdivCharacter.cpp
**
**		smdlSubdivCharacter represents an object which is made up of
**	multiple fragments all controlled by a single jointed skeleton.
**	Each fragment has morph targets that control states for the skin
**	before the skeleton deforms it.
**
**		This subdivision version of the character model alters the
**	base mesh with the jointed animation and morpth targets and
**	then updates and renders a subdivided level of the mesh.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlSubdivCharacter.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/env/envThreadGroup.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/ma/maSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dRenderingHints.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"
#include "Graphics/mdl/mdlSkinInfo.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "Graphics/Sc/scThreadGroup.hpp"
#include "Graphics/smdl/private/smdlBoneDisplay.hpp"
#include "Graphics/smdl/private/smdlHairSurface.hpp"
#include "Graphics/smdl/private/smdlMeshAutoLowRes.hpp"
#include "Graphics/smdl/private/smdlMeshGPUSkin.hpp"
#include "Graphics/smdl/private/smdlMeshSingleSkin.hpp"
#include "Graphics/smdl/private/smdlMeshSkinGroup.hpp"
#include "Graphics/smdl/private/smdlMeshStaticGroup.hpp"
#include "Graphics/smdl/private/smdlSubAnimation.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"
#include "Graphics/smdl/private/smdlSubdivNetworkMgr.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"
#include "Graphics/smdl/smdlCharacterAnimation.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"


//============================================================================
//============================================================================
namespace
{
	smdlSubdivCharacter::SubdivMode l_SubdivMode = smdlSubdivCharacter::e_SubdivCatmullClark;
	int l_MaxSubdivLevel = 3; //3; // number of levels of subdivision to generate
	int l_InitialSubdivLevel = 0; // could be set lower than max
	bool l_bAutoGenerateLowRes = true;

	//--------------------------------------------------------------------
	//  transform_normal
	//--------------------------------------------------------------------
	inline maVector3d transform_normal( const maMatrix4x4& i_pTransform, const maVector3d& i_Normal )
	{
		return maVector3d(	( i_pTransform.m_Mat[0] * i_Normal.m_X + i_pTransform.m_Mat[4] * i_Normal.m_Y + i_pTransform.m_Mat[8] * i_Normal.m_Z ),
							( i_pTransform.m_Mat[1] * i_Normal.m_X + i_pTransform.m_Mat[5] * i_Normal.m_Y + i_pTransform.m_Mat[9] * i_Normal.m_Z ),
							( i_pTransform.m_Mat[2] * i_Normal.m_X + i_pTransform.m_Mat[6] * i_Normal.m_Y + i_pTransform.m_Mat[10] * i_Normal.m_Z ) );
	}

	//--------------------------------------------------------------------
	// add any morph animation into "weights array" that should be 
	//	initialized to default beacuse it will not alter the weight
	//  data if there is no animation
	//--------------------------------------------------------------------
	void gather_weights(	const std::vector<smdlMorphTarget> &i_MorphTargets, 
							const smdlCharacterAnimInstance* i_pMorphAnim, 
							float i_AnimTime, 
							std::vector<float> &o_Weights )
	{
		const int nWeights = i_MorphTargets.size();
		DBG_ASSERT(nWeights == o_Weights.size(), "Weights array not allocated correctly");
		if (nWeights != o_Weights.size())
			return;
		for (int w=0; w<nWeights; w++)
		{
			if (i_pMorphAnim)
			{
				const smdlMorphAnimKeys* morph_keys = NULL;
				// First, try to use alias to get morph animation.
				// This is the newer format and supports more than
				// one weights per blend shape node.
				if (!i_MorphTargets[w].m_Alias.empty())
				{
					i_pMorphAnim->GetMorphKeysForTarget(i_MorphTargets[w].m_Alias);
				}
				// If we didn't get a morph animation channel through the
				// alis, try to use the blend shape node (the older format).
				if (!morph_keys)
				{
					i_pMorphAnim->GetMorphKeysForTarget(i_MorphTargets[w].m_Name);
				}
				if (morph_keys)
				{
					o_Weights[w] = morph_keys->GetBlend(i_AnimTime);
				}
			}
		}
	}

	//--------------------------------------------------------------------
	// return true if the flags on the given scene node mean that the
	//	high resolution version of the model will be needed.
	//--------------------------------------------------------------------
	inline bool need_highres( const g3dSceneNode& i_Node,
							  smdlSubdivCharacter::JointDisplay i_JointDisplay )
	{
		// High-res not needed if all render panels are low-res
		if (g3dRenderingHints::GetAllLowResolution())
			return false;
		// High-res not needed if invisible or if forced to low-res
		else if ( !i_Node.GetRenderable() ||
			 i_Node.GetForceLowResolution() )
			 return false;
		// High-res not needed if we are only rendering joints
		else if (i_JointDisplay == smdlSubdivCharacter::e_JointsOnly)
			return false;
		else 
			return true;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void set_node_resolution(g3dSceneNode *i_pNode, 
							 int i_MeshResolutionLevel,
							 bool i_bHaveLowRes)
	{
		// "low-resolution" flag is 3-way state
		//  0 - appear in all resolutions
		//	1 - appear only in low resolutions
		//	2 - appear only in high resolutions
		switch ( i_MeshResolutionLevel )
		{
		case 0:
			if (i_bHaveLowRes)
				i_pNode->SetContentResolution(g3dSceneNode::e_HighRes);
			break;
		case 1:
			i_pNode->SetContentResolution(g3dSceneNode::e_LowRes);
			break;
		case 2:
			i_pNode->SetContentResolution(g3dSceneNode::e_HighRes);
			break;
		}
	}


	//--------------------------------------------------------------------
	// get scene node for shape, or return root node
	//--------------------------------------------------------------------
	g3dSceneNode* get_shape_node(g3dSceneNode* i_pRoot, 
								 const std::string & i_ShapeNodeName)
	{
		if (i_ShapeNodeName.length() > 0)
		{
			if (g3dSceneNode *pShapeNode = i_pRoot->GetNamedNode(i_ShapeNodeName.c_str()))
				return pShapeNode;
			DBG_ERROR("Could not find shape node by name: " << i_ShapeNodeName.c_str());
		}
		// Otherwise return root
		return i_pRoot;
	}
	
	//--------------------------------------------------------------------
	// See if all of the vertices are influenced by the same joint
	//	and therefore could really be a static mesh.
	//--------------------------------------------------------------------
	bool check_skinning(const std::vector<smdlBoneVertex>& i_BoneVertices,
						int &o_BoneIndex)
	{
		std::vector<smdlBoneVertex>::const_iterator it;
		int bone_index = -1;
		bool have_bone = false;
		for (it = i_BoneVertices.begin(); it != i_BoneVertices.end(); ++it)
		{
			if (it->m_Influences.size() != 1)
				return false;

			// See if all bone indices are the same
			if (bone_index < 0)
			{
				bone_index = it->m_Influences[0].m_BoneIndex;
				have_bone = true;
			}
			else if (bone_index != it->m_Influences[0].m_BoneIndex)
				return false;
		}
		if (have_bone)
		{
			o_BoneIndex = bone_index;
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	// return true if the flags on the given scene node mean that the
	//	high resolution version of the model will be needed.
	//--------------------------------------------------------------------
	inline bool check_casts_shadow( g3dSceneNode& i_Node )
	{
		bool bCastsShadow = false;

		if (i_Node.GetFragment())
		{
			bCastsShadow |= i_Node.GetFragment()->GetCastsShadow();
		}
		
		const int num_children = i_Node.GetNumChildren();
		for (int i=0; i<num_children; ++i)
		{
			bCastsShadow |= check_casts_shadow(*i_Node.GetChild(i));
		}

		i_Node.SetCastsShadow( bCastsShadow );

		return bCastsShadow;
	}
		
	//--------------------------------------------------------------------
	// Function object for animating a subdivision surface in a thread
	//--------------------------------------------------------------------
	struct sSubdivAnimThread
	{
		smdlEnhancedSurface* m_pSurface;
		const smdlCharacterAnimInstance* m_pCharAnim;
		float m_AnimFrame;
		const maMatrix4x4* m_BoneMatrices;
		const std::vector<float>& m_Weights;

		void operator()()
		{
			// First, look to see if we have vertex animation on this surface
			const smdlVertexAnimKeys *vert_keys = 
				(m_pCharAnim) ? m_pCharAnim->GetVertexKeysForSurface(m_pSurface->GetSubdivName()) : NULL;
			if (vert_keys)
			{
				// Do vertex animation
				m_pSurface->VertexTransformSubdiv(m_AnimFrame, *vert_keys);
			}
			else
			{
				// Do jointed animation
				m_pSurface->TransformSubdiv(m_BoneMatrices, m_Weights);
			}
		}
	};

	//--------------------------------------------------------------------
	// Function object for animating a single skin mesh in a thread
	//--------------------------------------------------------------------
	struct sSingleSkinAnimThread
	{
		smdlSkinnedMeshSurface* m_pSingleSkin;
		const maMatrix4x4* m_BoneMatrices;

		void operator()()
		{
			m_pSingleSkin->TransformMesh(m_BoneMatrices);
		}

	};

	//--------------------------------------------------------------------
	// Function object for animating a mesh skin group in a thread
	//--------------------------------------------------------------------
	struct sMeshSkinGroupAnimThread
	{
		smdlMeshSkinGroup* m_pMeshGroup;
		const smdlCharacterAnimInstance* m_pCharAnim;
		float m_AnimFrame;
		const maMatrix4x4* m_BoneMatrices;
		const std::vector<float>& m_Weights;

		void operator()()
		{
			// First, look to see if we have vertex animation on this surface
			const smdlVertexAnimKeys *vert_keys = 
				(m_pCharAnim) ? m_pCharAnim->GetVertexKeysForSurface(m_pMeshGroup->GetMeshName()) : NULL;
			if (vert_keys)
			{
				// Do vertex animation
				m_pMeshGroup->VertexTransformMesh(m_AnimFrame, *vert_keys);
			}
			else
			{
				// Do jointed animation
				m_pMeshGroup->TransformMesh(m_BoneMatrices, m_Weights);
			}
		}
	};

	//--------------------------------------------------------------------
	// Function object for animating a mesh skin group in a thread
	//--------------------------------------------------------------------
	struct sHairAnimThread
	{
		smdlHairSurface* m_pHairSurface;
		const smdlCharacterAnimInstance* m_pCharAnim;
		float m_AnimFrame;

		void operator()()
		{
			// First, look to see if we have vertex animation on this surface
			const smdlVertexAnimKeys *vert_keys = 
				(m_pCharAnim) ? m_pCharAnim->GetVertexKeysForSurface(m_pHairSurface->GetHairName()) : NULL;
			if (vert_keys)
			{
				// Do vertex animation
				m_pHairSurface->VertexTransformHair(m_AnimFrame, *vert_keys);
			}
		}
	};

} // end of namespace


//--------------------------------------------------------------------
// Set and return tessellation method to use for subdivision surfaces
//--------------------------------------------------------------------
//static 
smdlSubdivCharacter::SubdivMode smdlSubdivCharacter::GetSubdivMode()
{
	return l_SubdivMode;
}
//static 
void smdlSubdivCharacter::SetSubdivMode(smdlSubdivCharacter::SubdivMode i_SubdivMode)
{
	DBG_ASSERT( (i_SubdivMode == e_SubdivCatmullClark || i_SubdivMode == e_SubdivPNTriangle || i_SubdivMode == e_SubdivPatch), "Unsupported subdivision mode");
	if (!((i_SubdivMode == e_SubdivCatmullClark || i_SubdivMode == e_SubdivPNTriangle || i_SubdivMode == e_SubdivPatch)))
		return;
	l_SubdivMode = i_SubdivMode;
}


//--------------------------------------------------------------------
// Set and return maximum level of subdivision for which the 
//	subdivision networks should be created
//--------------------------------------------------------------------
//static 
int smdlSubdivCharacter::GetMaxSubdivLevel()
{
	return l_MaxSubdivLevel;
}
//static 
void smdlSubdivCharacter::SetMaxSubdivLevel(int i_MaxSubdivLevel)
{
	l_MaxSubdivLevel = i_MaxSubdivLevel;
}

//--------------------------------------------------------------------
// Set and return initial level of subdivision at which the 
//	subdivision networks should be created
//--------------------------------------------------------------------
//static 
int smdlSubdivCharacter::GetInitialSubdivLevel()
{
	return l_InitialSubdivLevel;
}
//static 
void smdlSubdivCharacter::SetInitialSubdivLevel(int i_InitialSubdivLevel)
{
	l_InitialSubdivLevel = i_InitialSubdivLevel;
}

//--------------------------------------------------------------------
// Set whether a low resolution version should bo auto-generated.
//--------------------------------------------------------------------
//static 
void smdlSubdivCharacter::SetAutoGenerateLowRes(bool i_bAutoGen)
{
	l_bAutoGenerateLowRes = i_bAutoGen;
}

//--------------------------------------------------------------------
//	smdlSubdivCharacter requires the joint and scene graph hierarchies,
//	and the skin (influences and morph targets) definitions.  
//	The scene graph hierarchy should be set into the i_pRootNode.
//
//	Assumes ownership of the joint hierarchy. The Skin data
//	is not owned so that it can be shared.
//--------------------------------------------------------------------
smdlSubdivCharacter::smdlSubdivCharacter(	g3dSceneNode* i_pRootNode,
					const std::vector<mdlSkinInfo>& i_SkinnedSurfaces,
					const std::vector< shared_ptr<mdlHairInfo> >& i_HairSurfaces,
					const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping,
					const fsLocator& i_CharLocator )
:	smdlSkeletonObject( i_pRootNode ),
//	m_Skins( i_Skins ),
	m_bMorphDirty( true ),
	m_bHighResDirty( false ),
	m_CurSubdivLevel( l_InitialSubdivLevel ),
	m_JointDisplay( e_ModelOnly ),
	m_pAutoLowRes( NULL ),
	m_pBoneDisplay( NULL )
{
	// See if there are low-res models in hierarchy,
	// StaticMeshes were gathered in smdlHierarchyObject's constructor
	bool bHaveLowRes = GetStaticMeshes()->HasLowResolution();

	// Also look for low-res meshes in the skinnned list 
	// (can have vertex animating low resolution meshes)
	for (int i=0; i<i_SkinnedSurfaces.size(); i++)
	{
		if (i_SkinnedSurfaces[i].m_MeshInfo)
		{
			const int c_LowResolutionIndex = 1; // defined in mdlFragInfo
			if (i_SkinnedSurfaces[i].m_MeshInfo->m_ResolutionLevel == c_LowResolutionIndex)
			{
				bHaveLowRes = true;
				break;
			}
		}
	}

	// Generate Low Resolution model from skinned surfaces
	if (l_bAutoGenerateLowRes)
	{
		// Automatically generate low resolution if we don't have one in the file format
		//if (!bHaveLowRes && !i_SkinnedSurfaces.empty())
		// Now, it is possible to have some low res meshes and still have some
		// subdivs that need auto-gen. The flag for auto-gen low res is per surface now.
		if (!i_SkinnedSurfaces.empty())
		{
			m_pAutoLowRes = new smdlMeshAutoLowRes(i_pRootNode, i_SkinnedSurfaces);
			bHaveLowRes |= m_pAutoLowRes->HasLowResolution();
		}
	}

	// Then, create bone display for the joints in the hierarchy.
	// This inserts new fragments into hierarchy.
	m_pBoneDisplay = new smdlBoneDisplay( i_pRootNode );

	// Add fragments in small hierarchy under base node,
	// keep track of how many children our root node had at start
	m_BaseChildOffset = GetBase()->GetNumChildren();

	// Sort out the subdivision skins into a separate list so that
	// we can do them all first and then do the fragments without
	// needing to interlace the techniques. 
	std::vector<mdlSkinInfo> subdiv_skins, mesh_skins;
	std::vector< shared_ptr<mdlSubdivInfo> > subdiv_infos;
	for (int i=0; i<i_SkinnedSurfaces.size(); i++)
	{
		if (i_SkinnedSurfaces[i].m_SubdivInfo)
		{
			subdiv_skins.push_back(i_SkinnedSurfaces[i]);
			subdiv_infos.push_back(i_SkinnedSurfaces[i].m_SubdivInfo);
		}
		else if (i_SkinnedSurfaces[i].m_MeshInfo)
			mesh_skins.push_back(i_SkinnedSurfaces[i]);
		else
			DBG_ERROR("Expected either a mesh or subdivision surface.");
	}

	// Create or Share subdivision networks from the subdiv infos
	m_pSharedNetworkSet = smdlSubdivNetworkMgr::CreateNetworks(i_CharLocator, 
															subdiv_infos,  
															l_InitialSubdivLevel,
															l_MaxSubdivLevel);

	// Create surfaces for subdivisions
	const int num_subdivs = maFunctions::Lowest(subdiv_skins.size(), m_pSharedNetworkSet->m_SubdivNetworks.size());
	for (int i=0; i<num_subdivs; i++)
	{
		const mdlSkinInfo &skin_info = subdiv_skins[i];

		// get subdiv network
		shared_ptr<smdlSubdivNetwork> pNetwork = m_pSharedNetworkSet->m_SubdivNetworks[i];

		// Look for material override for this instance
		//matMaterial *pMaterial = skin_info.m_SubdivInfo->m_Material->m_pMaterial;
		//std::map<const matMaterial*,matMaterial*>::const_iterator it = i_MaterialRemapping.find(pMaterial);
		//if (it != i_MaterialRemapping.end())
		//{
		//	pMaterial = it->second;
		//}

		// create the surface to manage this subdiv and its fragment
		DBG_ASSERT(skin_info.m_SkinInfo, "Subdivision surface requires skin information.");
		if (!skin_info.m_SkinInfo)
			continue;

		smdlEnhancedSurface* pSurface = NULL;
		//if( l_SubdivMode == e_SubdivPatch )
		//{
		//	pSurface = new smdlPatchSurface(*skin_info.m_SubdivInfo, pNetwork, pMaterial, *skin_info.m_SkinInfo );
		//	m_SubdivSurfaces.push_back( pSurface );
		//}
		//else if( l_SubdivMode == e_SubdivPNTriangle )
		//{
		//	pSurface = new smdlCCPNSurface(*skin_info.m_SubdivInfo, pNetwork, pMaterial, *skin_info.m_SkinInfo );
		//	m_SubdivSurfaces.push_back( pSurface );
		//}
		//else if( l_SubdivMode == e_SubdivCatmullClark )
		{
			pSurface = new smdlSubdivSurface(skin_info.m_SubdivInfo, pNetwork, i_MaterialRemapping, *skin_info.m_SkinInfo );
			m_SubdivSurfaces.push_back( pSurface );
		}

		if (pSurface)
		{
			// Add this surface's node to our scene graph
			g3dSceneNode *pShapeNode = get_shape_node(GetBase(), skin_info.m_SceneNodeName);
			pShapeNode->AddChild( pSurface->RootNode() );

			// Store the transform node for this skin so we can animate it later
			m_SkinNodes[skin_info.m_SubdivInfo->m_Name] = pSurface->RootNode();

			// If we have a low resolution version in the main hierarchy,
			// then we can say that our subdivs should only render in high-resolution
			if (bHaveLowRes)
			{
				pSurface->RootNode()->SetContentResolution(g3dSceneNode::e_HighRes);
			}
		}
	}


	// Create skin groups for meshes
	const int num_frags = mesh_skins.size();
	for (int i=0; i<num_frags; i++)
	{		
		const mdlSkinInfo &skin_info = mesh_skins[i];
		const mdlFragInfo &mesh_info = *mesh_skins[i].m_MeshInfo;
		DBG_ASSERT(skin_info.m_SkinInfo, "Skinned mesh surfaces require skin information.");
		if (!skin_info.m_SkinInfo)
			continue;
		
		// See if this "skinned" mesh is really a static mesh completely 
		// influenced by a single joint. If true, then the index of the 
		// joint is returned
		int bone_index = 0;
		bool bUselessSkinning = check_skinning(skin_info.m_SkinInfo->m_BoneVertices, bone_index);
		if (bUselessSkinning
			&& (skin_info.m_SceneNodeName.empty())
			&& (skin_info.m_SkinInfo->m_MorphTargets.empty())
			&& (!mesh_info.m_Flags.m_bVertexAnimation))
		{
			// This fragment can be inserted into the hierarchy directly instead
			// of using the skinning animation interface
			g3dSceneNode *pJoint = this->GetJointByIndex( bone_index );
			if (pJoint)
			{
				std::vector<mdlSplitFragInfo> split_frags;
				mdlFragUtil::SplitFragments(mesh_info, split_frags, i_MaterialRemapping);

				for (int fi=0; fi<split_frags.size(); fi++)
				{
					mdlSplitFragInfo &split_frag = split_frags[fi];

					// Optimize the split fragment
					mdlFragCreate::OptimizeFragment(split_frag);

					// Transform the vertices by the bind pose matrix in order
					// to allow them to be altered by the joint matrices
					maPointTransformer4x4InPlace xformer( pJoint->GetInvBindPose() );
					envSTLHelpers::ForAll(split_frag.m_Vertices, xformer);

					// Transform normals
					maMatrix3x3 sub_mtx = pJoint->GetInvBindPose().GetSubMatrix(3,3);
					maPointTransformer3x3InPlace nformer( sub_mtx );
					envSTLHelpers::ForAll(split_frag.m_Normals, nformer);

					// We need to keep track of this fragment ourselves because
					// it won't be going into a skin group surface
					g3dFragment *pFragment = mdlFragCreate::CreateFragment(split_frag);
					m_Fragments.push_back( pFragment );

					// Create a node for our fragment 
					g3dSceneNode *pNode = new g3dSceneNode( pFragment );
					pNode->SetName( mesh_info.m_Name.c_str() );
					pNode->SetSkipAnim(true);
					//if (bHaveLowRes)
					//	pNode->SetContentResolution(g3dSceneNode::e_HighRes);
					set_node_resolution(pNode, mesh_info.m_ResolutionLevel, bHaveLowRes);

					// Add to joint's node
					pJoint->AddChild( pNode );

					// Store the transform node for this skin so we can animate it later
					m_SkinNodes[mesh_info.m_Name] = pNode;

					// Let our static mesh group manage this node, since it is
					// now in the main hierarchy
					this->StaticMeshes()->AddNode( pNode );
				}
			}
		}
		// If we have just one skin and no morph targets and no vertex anim
		// then, we can use a single skin mesh, which is a little more efficient.
		else if ((mesh_info.m_Materials.size() == 1) 
			&& (skin_info.m_SkinInfo->m_MorphTargets.empty())
			&& (!mesh_info.m_Flags.m_bVertexAnimation))
		{
			std::vector<mdlSplitFragInfo> split_frags;
			mdlFragUtil::SplitFragments(mesh_info, split_frags, i_MaterialRemapping);

			// Create single skin mesh surface
			//NOTE: change the commenting here to try out GPU skinning:
			smdlMeshSingleSkin *pSingleSkin = new smdlMeshSingleSkin(split_frags[0], 
			//smdlMeshGPUSkin *pSingleSkin = new smdlMeshGPUSkin(split_frags[0], 
																	 skin_info.m_SkinInfo->m_BoneVertices,
																	 skin_info.m_SkinInfo->m_BindPose);
			m_SingleSkins.push_back( pSingleSkin );

			// Add fragment to our scene graph
			pSingleSkin->RootNode()->SetName( mesh_info.m_Name.c_str() );
			g3dSceneNode *pShapeNode = get_shape_node(GetBase(), skin_info.m_SceneNodeName);
			pShapeNode->AddChild( pSingleSkin->RootNode() );

			// Store the transform node for this skin so we can animate it later
			m_SkinNodes[mesh_info.m_Name] = pSingleSkin->RootNode();

			// If we have a low resolution version in the main hierarchy,
			// then we can say that our subdivs should only render in high-resolution
			//if (bHaveLowRes)
			//{
			//	pSingleSkin->RootNode()->SetContentResolution(g3dSceneNode::e_HighRes);
			//}
			set_node_resolution(pSingleSkin->RootNode(), mesh_info.m_ResolutionLevel, bHaveLowRes);
		}
		else
		{
			// Create a mesh group. 
			smdlMeshSkinGroup* pMeshGroup = new smdlMeshSkinGroup(mesh_info, *skin_info.m_SkinInfo, i_MaterialRemapping);
			m_MeshGroups.push_back( pMeshGroup );

			// Add fragment to our scene graph
			g3dSceneNode *pShapeNode = get_shape_node(GetBase(), skin_info.m_SceneNodeName);
			pShapeNode->AddChild( pMeshGroup->RootNode() );

			// Store the transform node for this skin so we can animate it later
			m_SkinNodes[mesh_info.m_Name] = pMeshGroup->RootNode();

			// If we have a low resolution version in the main hierarchy,
			// then we can say that our subdivs should only render in high-resolution
			//if (bHaveLowRes)
			//{
			//	pMeshGroup->RootNode()->SetContentResolution(g3dSceneNode::e_HighRes);
			//}
			set_node_resolution(pMeshGroup->RootNode(), mesh_info.m_ResolutionLevel, bHaveLowRes);
		}
	}

	// Create hair surfaces
	const int num_hairs = i_HairSurfaces.size();
	for (int i=0; i<num_hairs; i++)
	{		
		const shared_ptr<mdlHairInfo> &hair_info = i_HairSurfaces[i];

		// Look for material override for this instance
		matMaterial *pMaterial = NULL;
		if (hair_info->m_Material)
		{
			pMaterial = hair_info->m_Material->m_pMaterial;
			std::map<const matMaterial*,matMaterial*>::const_iterator it = i_MaterialRemapping.find(pMaterial);
			if (it != i_MaterialRemapping.end())
			{
				pMaterial = it->second;
			}
		}

		smdlHairSurface *pHairSurface = new smdlHairSurface(*hair_info, pMaterial);
		m_HairSurfaces.push_back( pHairSurface );

		// Add fragment to our scene graph
		pHairSurface->RootNode()->SetName( hair_info->m_HairName.c_str() );
		g3dSceneNode *pShapeNode = get_shape_node(GetBase(), hair_info->m_SceneNodeName ); 
		pShapeNode->AddChild( pHairSurface->RootNode() );

		// Store the transform node for this skin so we can animate it later
		//m_SkinNodes[mesh_info.m_Name] = pSingleSkin->RootNode();
	}

	// set up weights array
	int num_skins = m_SubdivSurfaces.size() + m_MeshGroups.size();
	m_Weights.resize(num_skins);
	int w = 0;
	for (int i=0; i<m_SubdivSurfaces.size(); i++)
	{
		m_Weights[w++].resize(m_SubdivSurfaces[i]->GetCharacterSkin().m_MorphTargets.size(), 0.0f); // init weight values to 0.0
	}
	for (int i=0; i<m_MeshGroups.size(); i++)
	{
		m_Weights[w++].resize(m_MeshGroups[i]->GetCharacterSkin().m_MorphTargets.size(), 0.0f); // init weight values to 0.0
	}

	// Debug the names of the root children
	//int num_kids = this->GetBase()->GetNumChildren();
	//DBG_LOG("Num root children: " << num_kids);
	//for (int k=0; k< num_kids; k++)
	//{
	//	DBG_LOG("Kid named: " << this->GetBase()->GetChild(k)->GetName());
	//}
	
	// Look through the joint hierarchy and try to set the
	// casts shadow flag in the nodes only where needed.
	// This helps speed up the depth map rendering.
	bool bJointsCastShadow = check_casts_shadow( *i_pRootNode );
	//DBG_LOG("Character joints casts shadow: " << bJointsCastShadow);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlSubdivCharacter::~smdlSubdivCharacter()
{
	// Surfaces (subdiv and polygons) are owned by this object
	envSTLHelpers::DeleteContainer(m_SubdivSurfaces);
	smdlSubdivNetworkMgr::ReleaseNetworks(m_pSharedNetworkSet);
	envSTLHelpers::DeleteContainer(m_SingleSkins);
	envSTLHelpers::DeleteContainer(m_MeshGroups);
	envSTLHelpers::DeleteContainer(m_HairSurfaces);
	envSTLHelpers::DeleteContainer(m_Fragments);
	delete m_pBoneDisplay;
	delete m_pAutoLowRes;
}

//--------------------------------------------------------------------
// Overrides scObject's function. Returns true if we have a 
//	low-resolution variation of the model.
//--------------------------------------------------------------------
//virtual 
bool smdlSubdivCharacter::HasLowResolutionModel() const
{
	return (GetStaticMeshes()->HasLowResolution() ||
		(m_pAutoLowRes && m_pAutoLowRes->HasLowResolution()) );
}

//--------------------------------------------------------------------
//	Returns true if this model has subdivision surfaces. This model
//	might just have skinned or baked vertex animation on polygons.
//--------------------------------------------------------------------
bool smdlSubdivCharacter::HasSubdivisionSurfaces() const
{
	return (!m_SubdivSurfaces.empty());
}

//--------------------------------------------------------------------
// Return current subdivision level being used.
//--------------------------------------------------------------------
int smdlSubdivCharacter::GetCurrentSubdivLevel() const
{
	return this->m_CurSubdivLevel;
}

//--------------------------------------------------------------------
// Set the current subdivision level being used, this should be 
// a level less than or equal to the return value of 
// GetMaxSubdivLevel()
//--------------------------------------------------------------------
void smdlSubdivCharacter::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	// Cap maximum
	int subdiv_level = maFunctions::Lowest(i_SubdivLevel, l_MaxSubdivLevel);

//	if (m_CurSubdivLevel != subdiv_level)
	{
		const int num_subdivs = m_SubdivSurfaces.size();
		for (int i=0; i<num_subdivs; i++)
		{
			try
			{
				m_SubdivSurfaces[i]->SetCurrentSubdivLevel(subdiv_level);
			}
			catch (const std::bad_alloc&)
			{
				DBG_ERROR("Out of memory when setting subdivision level on surface " << 
					m_SubdivSurfaces[i]->GetSubdivName().c_str());
				throw;
			}
		}

		m_CurSubdivLevel = subdiv_level;
		m_bMorphDirty = true;
	}
}

//--------------------------------------------------------------------
// Return number of vertices in the subdivided model
// at the current subdivision level, used for sizing
// the array for GetSubdivVertices()
//--------------------------------------------------------------------
int smdlSubdivCharacter::GetNumFacesAtLevel(int i_SubdivLevel) const
{
	int total = 0;
	const int num_infos = m_SubdivSurfaces.size();
	for (int i=0; i<num_infos; i++)
	{
		total += m_SubdivSurfaces[i]->GetNumFacesAtLevel(i_SubdivLevel);
	}
	return total;
}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model.  It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
//virtual 
bool smdlSubdivCharacter::CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const
{
	if (!smdlHierarchyObject::CheckAnimation(i_GeoAnimation))
		return false;

	const smdlCharacterAnimation *pAnimation = dynamic_cast<const smdlCharacterAnimation*>( &i_GeoAnimation );
	if (pAnimation)
	{
		// Check the subdivs
		for (int s=0; s<m_SubdivSurfaces.size(); s++)
		{
			const smdlVertexAnimKeys *vert_keys = pAnimation->GetVertexKeysForSurface(m_SubdivSurfaces[s]->GetSubdivName());
			if (vert_keys)
			{
				// Check vertex animation
				if (!m_SubdivSurfaces[s]->CheckAnimation(*vert_keys))
					return false;
			}
		}

		// Check the mesh groups
		for (int m=0; m<m_MeshGroups.size(); m++)
		{
			const smdlVertexAnimKeys *vert_keys = pAnimation->GetVertexKeysForSurface(m_MeshGroups[m]->GetMeshName());
			if (vert_keys)
			{
				// Check vertex animation
				if (!m_MeshGroups[m]->CheckAnimation(*vert_keys))
					return false;
			}
		}

		// Check the hairs
		for (int s=0; s<m_HairSurfaces.size(); s++)
		{
			const smdlVertexAnimKeys *vert_keys = pAnimation->GetVertexKeysForSurface(m_HairSurfaces[s]->GetHairName());
			if (vert_keys)
			{
				// Check vertex animation
				if (!m_HairSurfaces[s]->CheckAnimation(*vert_keys))
					return false;
			}
		}

	}

	return true;
}

//--------------------------------------------------------------------
//	Animate
//--------------------------------------------------------------------
void smdlSubdivCharacter::Animate( float i_fSimulationTime )
{
	bool bHighResDisplay = need_highres( *this->GetBase(), m_JointDisplay );

	if ( m_bMorphDirty 
		|| (bHighResDisplay && m_bHighResDirty)
		|| this->CheckDirty(i_fSimulationTime))
	{
		m_bMorphDirty = false;

		// First let the smdlHierarchyObject do its animation; this will move the
		// scene graph nodes around properly.
		smdlHierarchyObject::Animate( i_fSimulationTime );

		// Get casted animation in order to check for vertex and skin animation
		const smdlCharacterAnimInstance* char_anim = (this->GetCurAnimInstance() != NULL) ?
			dynamic_cast<const smdlCharacterAnimInstance*>(this->GetCurAnimInstance()) : NULL;
		float anim_frame = (char_anim) ? char_anim->ComputeFrame(i_fSimulationTime) : 0.0f;

		// Handle visibility of the skins
		if (char_anim)
		{
			int num_skins = m_SkinNodes.size();
			//DBG_LOG2("Skin animation frame %f, num_skins: %d", anim_frame, num_skins);
			std::map<std::string, g3dSceneNode*>::iterator skin_it;
			for (skin_it = m_SkinNodes.begin(); skin_it != m_SkinNodes.end(); ++skin_it)
			{
				const smdlGeoAnimKeys* skin_keys = char_anim->GetSkinKeysForSurface( skin_it->first );
				if (skin_keys && skin_it->second)
				{
					
					if (skin_keys->GetVisible() != NULL)
					{
						bool bVisible = skin_keys->GetVisible( anim_frame );
						skin_it->second->SetRenderable( bVisible );
						//DBG_LOG2("Skin %s visible: %d", skin_it->first.c_str(), bVisible);
					}
				}
			}
		}

		// Don't do any skinning animation if we don't need the high-res display
		if (bHighResDisplay)
		{
			m_bHighResDirty = false;

			// Build the joint matrices
			maMatrix4x4* bone_matrices = this->BuildMatrices();

			// Check for morph animations, gather weights
			this->gather_morph_weights(i_fSimulationTime, m_AnimWeights);

			// Thread group for animating surfaces concurrently
			//envThreadGroup animation_threads;

			int wi = 0;
			// Transform the subdivs
			for (int s=0; s<m_SubdivSurfaces.size(); s++, wi++)
			{
				// Subdiv surface animation each goes in its own thread task
				sSubdivAnimThread subdiv_thread = {m_SubdivSurfaces[s], char_anim, anim_frame, bone_matrices, m_AnimWeights[wi]};
				//animation_threads.AddThread( subdiv_thread );

				// Use the scene level thread group now in order
				// to get objects to animate in parallel also, not just
				// surfaces within one object.
				scThreadGroup::AddThread(subdiv_thread);
			}

			//animation_threads.WaitForAll();

			// Transform the single skins (they don't need the weights)
			for (int k=0; k<m_SingleSkins.size(); k++)
			{
				sSingleSkinAnimThread skin_thread = {m_SingleSkins[k], bone_matrices};
				scThreadGroup::AddThread(skin_thread);
				
				// non-threaded way
				//m_SingleSkins[k]->TransformMesh(bone_matrices);
			}

			// Transform the mesh groups
			for (int m=0; m<m_MeshGroups.size(); m++, wi++)
			{
				sMeshSkinGroupAnimThread meshgroup_thread = {m_MeshGroups[m], char_anim, anim_frame, bone_matrices, m_AnimWeights[wi]};
				scThreadGroup::AddThread(meshgroup_thread);
			}

			// Transform the hairs
			for (int h=0; h<m_HairSurfaces.size(); h++)
			{
				// Hair surface animation each goes in its own thread task
				sHairAnimThread hair_thread = {m_HairSurfaces[h], char_anim, anim_frame};
				//animation_threads.AddThread( hair_thread );

				// Use the scene level thread group now in order
				// to get objects to animate in parallel also, not just
				// surfaces within one object.
				scThreadGroup::AddThread(hair_thread);
			}

		}
		else
		{	
			// Mark that the high resolution needs animation from here on out,
			// even if the timeline hasn't moved.
			m_bHighResDirty = true;
		}
	}
}


//--------------------------------------------------------------------
//	Animate only the scene graph nodes, 
//  fragments should not be animated yet.
//	This is for preparation for attachments that need the
//	transformations in the nodes, but not the bounding boxes.
//--------------------------------------------------------------------
void smdlSubdivCharacter::AnimateMatrices(float i_SimulationTime)
{
	if (this->CheckDirty(i_SimulationTime))
	{
		// Only need to the smdlHierarchyObject do its animation; this will move the
		// scene graph nodes around properly.
		smdlHierarchyObject::Animate( i_SimulationTime );

		// Look for vertex animations that have also baked the bounding boxes of
		// the surfaces. This helps with attachments and culling that happen
		// before the full vertex animation is done.
		const smdlCharacterAnimInstance* char_anim = (this->GetCurAnimInstance() != NULL) ?
			dynamic_cast<const smdlCharacterAnimInstance*>(this->GetCurAnimInstance()) : NULL;
		if (char_anim)
		{
			float anim_frame = char_anim->ComputeFrame(i_SimulationTime);

			// Update BBox for subdivs that are vertex animated
			for (int s=0; s<m_SubdivSurfaces.size(); s++)
			{
				const smdlVertexAnimKeys *vert_keys = 
					char_anim->GetVertexKeysForSurface(m_SubdivSurfaces[s]->GetSubdivName());
				if (vert_keys && (*vert_keys)->HasBBoxAnimation())
				{
					// Set pre-computed bounding box into fragments
					m_SubdivSurfaces[s]->BBoxTransform((*vert_keys)->GetBBox(anim_frame));
				}
			}

			// Update BBox for mesh groups that are vertex animated
			for (int m=0; m<m_MeshGroups.size(); m++)
			{
				const smdlVertexAnimKeys *vert_keys = 
					char_anim->GetVertexKeysForSurface(m_MeshGroups[m]->GetMeshName());
				if (vert_keys && (*vert_keys)->HasBBoxAnimation())
				{
					// Set pre-computed bounding box into fragments
					m_MeshGroups[m]->BBoxTransform((*vert_keys)->GetBBox(anim_frame));
				}
			}

			// Update BBox for hairs that are vertex animated
			for (int s=0; s<m_HairSurfaces.size(); s++)
			{
				const smdlVertexAnimKeys *vert_keys = 
					char_anim->GetVertexKeysForSurface(m_HairSurfaces[s]->GetHairName());
				if (vert_keys && (*vert_keys)->HasBBoxAnimation())
				{
					// Set pre-computed bounding box into fragments
					m_HairSurfaces[s]->BBoxTransform((*vert_keys)->GetBBox(anim_frame));
				}
			}
		}

		// Mark that the high resolution needs animation from here on out
		m_bHighResDirty = true;
	}
}

//--------------------------------------------------------------------
// Return number of skins in this model
//--------------------------------------------------------------------
int smdlSubdivCharacter::GetNumSkins()
{
	return m_Weights.size();
}

//--------------------------------------------------------------------
// Return number of morph target for the skin with the given index
//--------------------------------------------------------------------
int smdlSubdivCharacter::GetNumMorphTargets(int i_SkinIndex)
{
	DBG_ASSERT(i_SkinIndex < m_Weights.size(), "Skin Index out of range: " << i_SkinIndex << " < " << m_Weights.size());
	if (i_SkinIndex >= m_Weights.size())
		return 0;
	return m_Weights[i_SkinIndex].size();
}

//--------------------------------------------------------------------
// Return number of morph target for the skin with the given index
//--------------------------------------------------------------------
//std::string smdlSubdivCharacter::GetMorphTargetName(int i_SkinIndex, int i_MorphTargetIndex)
//{
//	DBG_ASSERT(i_SkinIndex < m_Skins.size(), "Skin Index out of range: %d of %d", i_SkinIndex, m_Skins.size());
//	const smdlCharacterSkin &skin = m_Skins[i_SkinIndex];
//	DBG_ASSERT2(i_MorphTargetIndex < skin.m_MorphTargets.size(), "Morph Target Index out of range: %d of %d", i_MorphTargetIndex, skin.m_MorphTargets.size());
//	const smdlMorphTarget &target = skin.m_MorphTargets[i_MorphTargetIndex];
//	return target.m_Name;
//}

//--------------------------------------------------------------------
// Weight is number, usually from 0-1, that controls influence
//	of the given morph target
//--------------------------------------------------------------------
float smdlSubdivCharacter::GetMorphTargetWeight(int i_SkinIndex, int i_MorphTargetIndex)
{
	return 0.0f;
}
void smdlSubdivCharacter::SetMorphTargetWeight(int i_SkinIndex, int i_MorphTargetIndex, float i_Weight)
{
	m_bMorphDirty = true;
	m_Weights[i_SkinIndex][i_MorphTargetIndex] = i_Weight;
}

//--------------------------------------------------------------------
// JointDisplay - control display of joints and model
//--------------------------------------------------------------------
void smdlSubdivCharacter::SetJointDisplay(JointDisplay i_Display)
{
	m_JointDisplay = i_Display;

	bool bShowJoints = (i_Display != e_ModelOnly);
	if (m_pBoneDisplay)
		m_pBoneDisplay->SetVisible( bShowJoints );

	bool bShowModel = (i_Display != e_JointsOnly);
	StaticMeshes()->SetVisible( bShowModel );
	if (m_pAutoLowRes)
		m_pAutoLowRes->SetVisible( bShowModel );
	
	std::for_each( m_SubdivSurfaces.begin(), m_SubdivSurfaces.end(),
		std::bind2nd( std::mem_fun( &smdlSurface::SetVisible ), bShowModel ) );
	std::for_each( m_SingleSkins.begin(), m_SingleSkins.end(),
		std::bind2nd( std::mem_fun( &smdlSurface::SetVisible ), bShowModel ) );
	std::for_each( m_MeshGroups.begin(), m_MeshGroups.end(),
		std::bind2nd( std::mem_fun( &smdlSurface::SetVisible ), bShowModel ) );
}
smdlSubdivCharacter::JointDisplay smdlSubdivCharacter::GetJointDisplay() const
{
	return m_JointDisplay;
}

//--------------------------------------------------------------------
// private function supporting Animate()
// Gather the weights for each morph target using animation data.
//--------------------------------------------------------------------
void smdlSubdivCharacter::gather_morph_weights(float i_fSimulationTime,
											   std::vector<std::vector<float> > &o_Weights)
{
	// Use copy to allocate weights array and set up defaults in one step
	o_Weights = m_Weights;

	const smdlCharacterAnimInstance* morph_anim = 
		dynamic_cast<const smdlCharacterAnimInstance*>(this->GetCurAnimInstance());
	float anim_frame = (morph_anim) ? morph_anim->ComputeFrame(i_fSimulationTime) : 0.0f;

	// Look for subanimations that have morph target data
	int num_subanims = this->GetNumSubAnims();
	std::vector<const smdlCharacterAnimInstance*> subanims;
	std::vector<float> subanim_times;
	std::vector<float> subanim_blends;
	for (int i=0; i<num_subanims; i++)
	{
		const smdlCharacterAnimInstance* morph_subanim = 
			dynamic_cast<const smdlCharacterAnimInstance*>(this->GetSubAnimation(i)->GetAnimInstance());
	
		if (morph_subanim)
		{
			subanims.push_back(morph_subanim);
			subanim_times.push_back( morph_subanim->ComputeFrame(i_fSimulationTime) );
			subanim_blends.push_back( this->GetSubAnimation(i)->GetBlend() );
		}
	}
	const int nSubAnims = subanims.size();


	// handle each skin
	int numSkins = m_Weights.size();
	const int numSubdivs = m_SubdivSurfaces.size();
	for (int s=0; s<numSkins; s++)
	{
		const smdlCharacterSkin &skin = (s < numSubdivs) ? 
				m_SubdivSurfaces[s]->GetCharacterSkin() : 
				m_MeshGroups[s - numSubdivs]->GetCharacterSkin();

		const int nWeights = skin.m_MorphTargets.size();
		DBG_ASSERT(nWeights == o_Weights[s].size(), "Weights array not allocated correctly");
		if (nWeights != o_Weights[s].size())
			continue;
		for (int w=0; w<nWeights; w++)
		{
			float &weight = o_Weights[s][w];
			const std::string& morph_name = skin.m_MorphTargets[w]->m_Name;
			const std::string& alias_name = skin.m_MorphTargets[w]->m_Alias;

			if (morph_anim)
			{
				const smdlMorphAnimKeys* morph_keys = NULL;
				// First, try to use alias to get morph animation.
				// This is the newer format and supports more than
				// one weights per blend shape node.
				if (!alias_name.empty())
				{
					morph_keys = morph_anim->GetMorphKeysForTarget(alias_name);
				}
				// If we didn't get a morph animation channel through the
				// alis, try to use the blend shape node (the older format).
				if (!morph_keys)
				{
					morph_keys = morph_anim->GetMorphKeysForTarget(morph_name);
				}
				if (morph_keys)
				{
					weight = morph_keys->GetBlend(anim_frame);
				}
			}

			for (int sa=0; sa<nSubAnims; sa++)
			{
				const smdlMorphAnimKeys* morph_keys = NULL;
				if (!alias_name.empty())
					morph_keys = subanims[sa]->GetMorphKeysForTarget(alias_name);
				if (!morph_keys)
					morph_keys = subanims[sa]->GetMorphKeysForTarget(morph_name);

				if (morph_keys)
				{
					weight += subanim_blends[sa] * morph_keys->GetBlend(subanim_times[sa]);
				}	
			}
		}
	}
}

//--------------------------------------------------------------------
// GetSubdivSurfaces()
//--------------------------------------------------------------------
const std::vector<smdlEnhancedSurface*>& smdlSubdivCharacter::GetSubdivSurfaces()
{
	return m_SubdivSurfaces;
}

//--------------------------------------------------------------------
// GetNumSubdivSurfaces()
//--------------------------------------------------------------------
int smdlSubdivCharacter::GetNumSubdivSurfaces()
{
	return m_SubdivSurfaces.size();
}

//--------------------------------------------------------------------
// GetMeshGroups()
//--------------------------------------------------------------------
const std::vector<smdlMeshSkinGroup*>& smdlSubdivCharacter::GetMeshGroups()
{
	return m_MeshGroups;
}

//--------------------------------------------------------------------
// GetNumMeshGroups()
//--------------------------------------------------------------------
int smdlSubdivCharacter::GetNumMeshGroups()
{
	return m_MeshGroups.size();
}


