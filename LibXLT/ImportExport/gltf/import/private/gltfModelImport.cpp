/****************************************************************************\
**  gltfModelImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfModelImport.hpp"
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Core/Ma/maRotation.hpp"
#include "Graphics/mdl/private/mdlRotationOrder.hpp"

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
						gltfMeshImport::ConvertMesh(i_pModel, pMesh, o_pSceneNode, io_MaterialTable, o_Fragments, o_Materials, i_ContainingFile);
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

}	// end of namespace

