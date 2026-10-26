/****************************************************************************\
**  gltfModelImport.cpp
**
**	LoadNodeMesh: the template of one mesh object
**
**	- Skinned (an e_SkinnedMesh whose skeleton gltfSkeletonRead reads): a
**	  base node (identity) whose one child is the skeleton, built from
**	  gltfSkeleton::m_Nodes in order (glTF names, local TRS, joints marked
**	  with their inverse bind matrices), so bones count in that order. The
**	  mesh is one skinned surface (mdlSkinInfo) on the character's root;
**	  its morph targets are kept when an animation sets the node's
**	  weights, else its default weights are baked into the positions.
**	- Morph-animated (an e_Mesh with clips: an animation sets its
**	  weights): the base node carries the baked scale and is the one
**	  joint, with an identity inverse bind pose, so bone 0's matrix is the
**	  baked scale; every vertex is on bone 0 with weight 1, and the mesh's
**	  targets are morph targets (weights 0 outside a clip).
**	- Transform-clip (an e_Mesh whose transform animation can't be keys,
**	  gltfAnimTracks.cpp): as morph-animated, but the base node carries
**	  the node's whole local transform (non-uniform scale included), which
**	  its clips animate; the object sits at its parent.
**	- Static: as before, with the default weights (node.weights, else
**	  mesh.weights) baked into the positions. A skinned mesh whose
**	  skeleton can't be read comes in this way, with a warning.
**	- The baked scale: a non-uniform object scale goes into the template
**	  (the character's scale is one float); else (1, 1, 1).
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfModelImport.hpp"
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#include "ImportExport/gltf/import/private/gltfAnimRead.hpp"
#include "ImportExport/gltf/import/private/gltfSkeletonRead.hpp"
#include "ImportExport/gltf/import/private/gltfSkinRead.hpp"
#ifndef GLTF_NODETRANSFORM_HPP
#include "ImportExport/gltf/import/private/gltfNodeTransform.hpp"
#endif

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#ifndef MA_ROTATION_HPP
#include "Core/Ma/maRotation.hpp"
#endif
#include "Graphics/mdl/private/mdlRotationOrder.hpp"
#ifndef SMDL_SKININFO_HPP
#include "Graphics/mdl/mdlSkinInfo.hpp"
#endif

#undef FindResource


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfModelImport
{

	namespace
	{

		//------------------------------------------------------------------------
		//	get matrix from KFbxNode 
		//------------------------------------------------------------------------
		void get_xform_matrix(tinygltf::Node& i_Node, maMatrix4x4& o_OutMatx)
		{
			// glTF matrices are column-major with column vectors, which has the
			// same memory layout as our row-major, row-vector matrices.
			// Translations are scaled from meters to centimeters to match
			// the scaled vertex positions.
			if (i_Node.matrix.size() == 16)
			{
				for (int i = 0; i < 16; ++i) {
					o_OutMatx.Ptr()[i] = (float)i_Node.matrix[i];
				}
				o_OutMatx.Ptr()[12] *= gltfMeshImport::c_UnitScale;
				o_OutMatx.Ptr()[13] *= gltfMeshImport::c_UnitScale;
				o_OutMatx.Ptr()[14] *= gltfMeshImport::c_UnitScale;
				return;
			}

			float sx = 1, sy = 1, sz = 1;
			if (i_Node.scale.size() == 3)
			{
				sx = i_Node.scale[0];
				sy = i_Node.scale[1];
				sz = i_Node.scale[2];
			}
			maRotation rot;
			if (i_Node.rotation.size() == 4) {
				rot = maRotation(i_Node.rotation[0], i_Node.rotation[1], i_Node.rotation[2], i_Node.rotation[3]);
			}
			maVector3d trans;
			if (i_Node.translation.size() == 3){
				trans = maVector3d(i_Node.translation[0], i_Node.translation[1], i_Node.translation[2]) * gltfMeshImport::c_UnitScale;
			}

			o_OutMatx.MakeScale( sx, sy, sz );
			o_OutMatx *= rot.GetMatrix();
			o_OutMatx.TranslateBy( trans.m_X, trans.m_Y, trans.m_Z );
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void ConvertNode(tinygltf::Model* i_pModel, tinygltf::Node* i_pNode,
						 g3dSceneNode*& o_pSceneNode,
						 //const fsResourceFinder& i_TextureFinder,
						 mdlMatInfoTable& io_MaterialTable,
						 std::vector<g3dFragment*>& o_Fragments,
						 std::vector<matMaterial*>& o_Materials,
						 //std::vector<matTexture*>& o_Textures,
						 const fsLocator& i_ContainingFile	)
		{


			o_pSceneNode = new g3dSceneNode();

			//DBG_LOG("Got FBX node named: " << i_pNode->GetName());
			o_pSceneNode->SetName(i_pNode->name.c_str());

			// Construct the transform from individual translate, rotation, etc. values 
			maMatrix4x4 matx;
			get_xform_matrix(*i_pNode, matx);
			o_pSceneNode->SetTransform(matx);

			if (i_pNode->mesh >= 0 && i_pNode->mesh < (int)i_pModel->meshes.size()) 
			{
					//DBG_LOG("Got FBX mesh ");     
				tinygltf::Mesh* pMesh = &i_pModel->meshes[i_pNode->mesh];
					if (pMesh)
					{
						gltfMeshImport::ConvertMesh(i_pModel, pMesh, o_pSceneNode, io_MaterialTable, o_Fragments, o_Materials, i_ContainingFile,
													gltfSkinRead::DefaultWeights(*pMesh, *i_pNode));
					}
			}


			// Recurse on children
			for (int i = 0; i < (int)i_pNode->children.size(); i++)
			{
				const int child = i_pNode->children[i];
				if (child < 0 || child >= (int)i_pModel->nodes.size())
					continue;

				g3dSceneNode* new_node = NULL;

				ConvertNode(i_pModel, &i_pModel->nodes[child], new_node, io_MaterialTable,
							o_Fragments, o_Materials, i_ContainingFile);

				if (new_node)
					o_pSceneNode->AddChild(new_node);
			}
		}

		//------------------------------------------------------------------------
		// the skeleton's nodes under i_pBase, in m_Nodes order
		//------------------------------------------------------------------------
		void build_skeleton(const tinygltf::Model* i_pModel, const gltfSkeleton& i_Skeleton, g3dSceneNode* i_pBase)
		{
			std::vector<g3dSceneNode*> nodes;
			for (const gltfSkeletonNode& skeleton_node : i_Skeleton.m_Nodes)
			{
				g3dSceneNode* pNode = new g3dSceneNode();
				pNode->SetName(i_pModel->nodes[skeleton_node.m_Node].name.c_str());

				maMatrix4x4 matx;
				matx.MakeScale(skeleton_node.m_Scale.m_X, skeleton_node.m_Scale.m_Y, skeleton_node.m_Scale.m_Z);
				matx *= skeleton_node.m_Rotation.GetMatrix();
				matx.TranslateBy(skeleton_node.m_Translation.m_X, skeleton_node.m_Translation.m_Y, skeleton_node.m_Translation.m_Z);
				pNode->SetTransform(matx);

				if (skeleton_node.m_bJoint)
				{
					pNode->SetIsJoint(true);
					pNode->SetInvBindPose(skeleton_node.m_InvBindPose);
				}

				g3dSceneNode* pParent = (skeleton_node.m_Parent < 0) ? i_pBase : nodes[skeleton_node.m_Parent];
				pParent->AddChild(pNode);
				nodes.push_back(pNode);
			}
		}

	}	// end of local namespace


	//------------------------------------------------------------------------
	//	LoadModel converts the geometry in the scene from the FBX SDK 
	//	into our scene graph.
	//------------------------------------------------------------------------
	void LoadModel(	tinygltf::Model* pScene,
					const fsLocator& i_Locator,
					//const fsResourceFinder& i_TextureFinder,
					g3dSceneNode*& o_pSceneNode,
					std::vector<mdlSkinInfo>& o_SkinData,
					std::vector<g3dFragment*>& o_Fragments,
					mdlMatInfoTable& o_MaterialTable,
					std::vector<matMaterial*>& o_Materials)
					//std::vector<matTexture*>& o_Textures)
	{	
		// for MSP: 
		// Y is up
		// unit is cm
		// 

		// TODO : maybe use first node of default scene instead of creating an initial fake root node?

		// we shall treat Scene as a root node to begin:
		tinygltf::Node rootNode;

		// The default scene is optional; fall back to the first scene.
		// A file with no scenes at all just has an empty root.
		int scene_index = pScene->defaultScene;
		if (scene_index < 0 || scene_index >= (int)pScene->scenes.size())
			scene_index = 0;
		if (scene_index < (int)pScene->scenes.size())
		{
			const tinygltf::Scene& scene = pScene->scenes[scene_index];
			rootNode.children = scene.nodes;
			rootNode.name = scene.name;
		}

		// Use material table in order to share materials with the same name
			ConvertNode(pScene, &rootNode, o_pSceneNode, o_MaterialTable,
						o_Fragments, o_Materials, i_Locator);
			//ConvertNode(pRootNode, o_pSceneNode, i_TextureFinder, material_table,
			//			o_Fragments, o_Materials, o_Textures);
	}

	//------------------------------------------------------------------------
	//	LoadNodeMesh converts one node's mesh into our scene graph.
	//------------------------------------------------------------------------
	void LoadNodeMesh(	const tinygltf::Model* i_pScene,
						const gltfSceneObject& i_Object,
						const fsLocator& i_Locator,
						g3dSceneNode*& o_pSceneNode,
						std::vector<mdlSkinInfo>& o_SkinData,
						std::vector<g3dFragment*>& o_Fragments,
						mdlMatInfoTable& o_MaterialTable,
						std::vector<matMaterial*>& o_Materials)
	{
		const tinygltf::Node& node = i_pScene->nodes[i_Object.m_Node];

		o_pSceneNode = new g3dSceneNode();
		o_pSceneNode->SetName(node.name.c_str());

		if (node.mesh < 0 || node.mesh >= (int)i_pScene->meshes.size())
			return;
		const tinygltf::Mesh& mesh = i_pScene->meshes[node.mesh];
		const std::vector<float> default_weights = gltfSkinRead::DefaultWeights(mesh, node);
		const std::vector<float> no_weights;

		// skinned: the skeleton under an identity base, the mesh one skinned surface
		gltfSkeleton skeleton;
		if (i_Object.m_Kind == gltfSceneObject::e_SkinnedMesh)
		{
			if (gltfSkeletonRead::Read(*i_pScene, i_Object.m_Skin, skeleton))
			{
				build_skeleton(i_pScene, skeleton, o_pSceneNode);

				const bool bMorphing = gltfAnimRead::AnimatesWeights(*i_pScene, i_Object.m_Node);
				mdlSkinInfo skin;
				gltfMeshImport::ConvertSkinnedMesh(i_pScene, &mesh, node, &skeleton.m_BoneOfJoint,
												   bMorphing ? no_weights : default_weights,
												   o_MaterialTable, o_Materials, i_Locator, skin);
				if (skin.m_MeshInfo)
					o_SkinData.push_back(skin);
				return;
			}
			DBG_WARNING("glTF mesh " << i_Object.m_Identity << ": its skeleton can't be read; it comes in static, in its bind pose");
		}

		// the scale the object can't carry; a transform-clip mesh: the node's transform
		maMatrix4x4 matx;
		if (i_Object.m_Kind == gltfSceneObject::e_Mesh && i_Object.m_bTransformClip)
		{
			matx = gltfNodeTransform::NodeMatrix(node);
		}
		else
		{
			const maVector3d baked_scale = i_Object.m_bUniformScale ? maVector3d(1, 1, 1) : i_Object.m_Scale;
			matx.MakeScale(baked_scale.m_X, baked_scale.m_Y, baked_scale.m_Z);
		}
		o_pSceneNode->SetTransform(matx);

		// morph-animated: the base node is bone 0, its matrix the baked scale
		if (i_Object.m_Kind == gltfSceneObject::e_Mesh && !i_Object.m_Clips.empty())
		{
			o_pSceneNode->SetIsJoint(true);
			o_pSceneNode->SetInvBindPose(maMatrix4x4());

			mdlSkinInfo skin;
			gltfMeshImport::ConvertSkinnedMesh(i_pScene, &mesh, node, NULL, no_weights,
											   o_MaterialTable, o_Materials, i_Locator, skin);
			if (skin.m_MeshInfo)
				o_SkinData.push_back(skin);
			return;
		}

		// static
		gltfMeshImport::ConvertMesh(i_pScene, &mesh, o_pSceneNode, o_MaterialTable,
									o_Fragments, o_Materials, i_Locator, default_weights);
	}

}	// end of namespace

