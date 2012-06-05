/****************************************************************************\
**  fbxModelImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/fbx/import/private/fbxModelImport.hpp"
#include "ImportExport/fbx/import/private/fbxMeshImport.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Core/Ma/maRotation.hpp"
#include "Graphics/mdl/private/mdlRotationOrder.hpp"

#undef FindResource

#ifdef USE_FBX_IMPORTEXPORT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace fbxModelImport
{

	namespace
	{
		//------------------------------------------------------------------------
		//	convert matrix from Maya style to our style
		//------------------------------------------------------------------------
		void convert_matrix(const KFbxXMatrix& i_InMtx, maMatrix4x4 &o_OutMatx)
		{
			for (int row=0; row < 4; row++)
			{
				for (int col=0; col < 4; col++)
				{
					o_OutMatx(row, col) = float(i_InMtx.Get(row,col));
				}
			}
		}

		//------------------------------------------------------------------------
		//	convert vector from Maya style to our style
		//------------------------------------------------------------------------
		maVector3d convert_vector(const KFbxVector4& i_InVec)
		{
			return maVector3d( i_InVec[0], i_InVec[1], i_InVec[2] );
		}


		//------------------------------------------------------------------------
		// convert rotation order enumerations
		//------------------------------------------------------------------------
		mdlRotationOrder::RotationOrder get_rotation_order(ERotationOrder i_RotationOrder)
		{
			mdlRotationOrder::RotationOrder rot_order = mdlRotationOrder::e_XYZ;
			switch (i_RotationOrder)
			{
			default:
			case eSPHERIC_XYZ:
			case eEULER_XYZ: 
				rot_order = mdlRotationOrder::e_XYZ;
				break;
			case eEULER_XZY:
				rot_order = mdlRotationOrder::e_XZY;
				break;
			case eEULER_YZX:
				rot_order = mdlRotationOrder::e_YZX;
				break;
			case eEULER_YXZ:
				rot_order = mdlRotationOrder::e_YXZ;
				break;
			case eEULER_ZXY:
				rot_order = mdlRotationOrder::e_ZXY;
				break;
			case eEULER_ZYX:
				rot_order = mdlRotationOrder::e_ZYX;
				break;
			}
			return rot_order;
		}

		//------------------------------------------------------------------------
		//	get matrix from KFbxNode 
		//------------------------------------------------------------------------
		void get_xform_matrix(KFbxNode& i_Node, maMatrix4x4 &o_OutMatx)
		{
			//bga - Really need to consider pivot information here also?...
			KFbxVector4 temp_vec;

			// Are any of the target properties being set? Didn't seem to be.
			//if (i_Node.GetTarget())
			//	DBG_LOG("Target is set.");
			//maVector3d tgt_rot = convert_vector(i_Node.GetPostTargetRotation());
			//DBG_LOG("Target rotation: " << tgt_rot);
			//if (i_Node.GetTargetUp())
			//	DBG_LOG("Target Up is set.");
			//maVector3d tgt_up = convert_vector(i_Node.GetTargetUpVector());
			//DBG_LOG("Target up vector: " << tgt_up);


			// There are so many ways to get at the translation value, it is
			// hard to say which one we should read.  
			// The "Default" values seem to incorporate the unit conversions
			// while the "Local" functions do not.
			i_Node.GetDefaultT(temp_vec);
			maVector3d def_trans = convert_vector(temp_vec);
			//DBG_LOG("DefaultT: " << def_trans);
			//temp_vec = i_Node.GetLocalTFromDefaultTake();
			//maVector3d local_trans = convert_vector(temp_vec);
			//DBG_LOG("Local trans: " << local_trans);
			
			ERotationOrder lRotationOrder;
			i_Node.GetRotationOrder(KFbxNode::eSOURCE_SET, lRotationOrder);
			mdlRotationOrder::RotationOrder rot_order = get_rotation_order(lRotationOrder);

			i_Node.GetDefaultR(temp_vec);
			//temp_vec = i_Node.GetLocalRFromDefaultTake();
			maVector3d euler = convert_vector(temp_vec);
			//DBG_LOG("DefaultR: " << euler);
			maRotation rot;
			mdlRotationOrder::SetOrderedEuler(rot, rot_order, 
				euler.m_X*maConstants::c_fAngleToRad, 
				euler.m_Y*maConstants::c_fAngleToRad, 
				euler.m_Z*maConstants::c_fAngleToRad);

			// Pre-rotation - is this like our "Orientation" field?
			// In any case, we can just multiply them together since we aren't animating.
			temp_vec = i_Node.GetPreRotation(KFbxNode::eSOURCE_SET);
			maVector3d orient = convert_vector(temp_vec);
			//DBG_LOG("Pre-rotation: " << orient);
			maRotation pre_rot;
			mdlRotationOrder::SetOrderedEuler(pre_rot, rot_order, 
				orient.m_X*maConstants::c_fAngleToRad, 
				orient.m_Y*maConstants::c_fAngleToRad, 
				orient.m_Z*maConstants::c_fAngleToRad);

			//temp_vec = i_Node.GetLocalSFromDefaultTake();
			//maVector3d local_scale = convert_vector(temp_vec);
			//DBG_LOG("Local Scale: " << local_scale);
			i_Node.GetDefaultS(temp_vec);
			maVector3d def_scale = convert_vector(temp_vec);
			// DBG_LOG("DefaultS: " << def_scale);

			o_OutMatx.MakeScale( def_scale.m_X, def_scale.m_Y, def_scale.m_Z );
			o_OutMatx *= rot.GetMatrix();
			o_OutMatx *= pre_rot.GetMatrix();	//pre-rotation multiplied after, strangely enough
			o_OutMatx.TranslateBy( def_trans.m_X, def_trans.m_Y, def_trans.m_Z );
		}

		//------------------------------------------------------------------------
		//	get transformation that will be applied only to mesh
		//------------------------------------------------------------------------
		void get_geometric_xform(KFbxNode& i_Node, maMatrix4x4 &o_OutMatx)
		{
			maVector3d trans = convert_vector(i_Node.GetGeometricTranslation(KFbxNode::eSOURCE_SET));
			//DBG_LOG("Geometric Translation: " << trans);
			maVector3d scale = convert_vector(i_Node.GetGeometricScaling(KFbxNode::eSOURCE_SET));
			maVector3d euler = convert_vector(i_Node.GetGeometricRotation(KFbxNode::eSOURCE_SET));

			ERotationOrder lRotationOrder;
			i_Node.GetRotationOrder(KFbxNode::eSOURCE_SET, lRotationOrder);
			mdlRotationOrder::RotationOrder rot_order = get_rotation_order(lRotationOrder);

			maRotation rot;
			mdlRotationOrder::SetOrderedEuler(rot, rot_order, 
				euler.m_X*maConstants::c_fAngleToRad, 
				euler.m_Y*maConstants::c_fAngleToRad, 
				euler.m_Z*maConstants::c_fAngleToRad);

			o_OutMatx.MakeScale( scale.m_X, scale.m_Y, scale.m_Z );
			o_OutMatx *= rot.GetMatrix();
			o_OutMatx.TranslateBy( trans.m_X, trans.m_Y, trans.m_Z );
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void ConvertNode(KFbxNode* i_pNode,
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
			o_pSceneNode->SetName(i_pNode->GetName());

			if (i_pNode->GetNodeAttribute())
			{
				KFbxNodeAttribute::EAttributeType lAttributeType = (i_pNode->GetNodeAttribute()->GetAttributeType());
				switch (lAttributeType)
				{
				case KFbxNodeAttribute::eMESH: 
					//DBG_LOG("Got FBX mesh ");     
					KFbxMesh* pMesh = (KFbxMesh*) i_pNode->GetNodeAttribute ();
					if (pMesh)
					{
						// Create new node to hold the geometric transformation and the mesh
						g3dSceneNode *pMeshNode  = new g3dSceneNode();
						pMeshNode->SetName(	pMesh->GetName() );
						o_pSceneNode->AddChild(pMeshNode);

						maMatrix4x4 matx;
						get_geometric_xform(*i_pNode, matx);
						pMeshNode->SetTransform(matx);

						fbxMeshImport::ConvertMesh(*pMesh, pMeshNode, io_MaterialTable, o_Fragments, o_Materials, i_ContainingFile);
						//fbxMeshImport::ConvertMesh(*pMesh, pMeshNode, i_TextureFinder, io_MaterialTable, o_Fragments, o_Materials, o_Textures);
					}
					break;

					// Some other options...
				//case KFbxNodeAttribute::eMARKER: 
				//case KFbxNodeAttribute::eSKELETON: 
				//case KFbxNodeAttribute::eNURB:   
				//case KFbxNodeAttribute::ePATCH:  
				//case KFbxNodeAttribute::eCAMERA: 
				//case KFbxNodeAttribute::eLIGHT:  
				//	break;
				}   
			}

			// Construct the transform from individual translate, rotation, etc. values 
			maMatrix4x4 matx;
			get_xform_matrix(*i_pNode, matx);
			o_pSceneNode->SetTransform(matx);

			// Recurse on children
			for (int i = 0; i < i_pNode->GetChildCount(); i++)
			{
				g3dSceneNode* new_node = NULL;

				//ConvertNode(i_pNode->GetChild(i), new_node, i_TextureFinder, io_MaterialTable,
				//			o_Fragments, o_Materials, o_Textures);
				ConvertNode(i_pNode->GetChild(i), new_node, io_MaterialTable,
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
	void LoadModel(	KFbxScene* pScene,
					const fsLocator& i_Locator,
					//const fsResourceFinder& i_TextureFinder,
					g3dSceneNode*& o_pSceneNode,
					std::vector<mdlSkinInfo>& o_SkinData,
					std::vector<g3dFragment*>& o_Fragments,
					mdlMatInfoTable& o_MaterialTable,
					std::vector<matMaterial*>& o_Materials)
					//std::vector<matTexture*>& o_Textures)
	{		
		// Get some global properties from the scene
		KFbxGlobalSettings &settings = pScene->GetGlobalSettings();
		int sign = 1;
		if (settings.GetAxisSystem().GetUpVector(sign) == KFbxAxisSystem::YAxis)
			DBG_LOG("Before conversion, up axis is Y");
		else if (settings.GetAxisSystem().GetUpVector(sign) == KFbxAxisSystem::ZAxis)
			DBG_LOG("Before conversion, up axis is Z");

		KFbxSystemUnit units = settings.GetSystemUnit();
		DBG_LOG("Units: " << units.GetScaleFactorAsString());

		// It looks like there is a way within the FBX importer to 
		// just convert a scene to the axis system you want.
		// Let's give that a shot here..
		KFbxAxisSystem maya_axis_system(KFbxAxisSystem::eMayaYUp);
		//KFbxAxisSystem d3d_axis_system(KFbxAxisSystem::eDirectX);
		//KFbxAxisSystem ogl_axis_system(KFbxAxisSystem::eOpenGL);
		maya_axis_system.ConvertScene(pScene);
		//d3d_axis_system.ConvertScene(pScene);

		// Convert to centimeters also
		//KFbxSystemUnit centimeters(1.0);
		//centimeters.ConvertScene(pScene);
		//KFbxSystemUnit::KFbxUnitConversionOptions defaults = KFbxSystemUnit::DefaultConversionOptions;
		KFbxSystemUnit::cm.ConvertScene(pScene);

		//// Check results of conversions
		//KFbxGlobalSettings &after = pScene->GetGlobalSettings();
		//if (after.GetAxisSystem().GetUpVector(sign) == KFbxAxisSystem::YAxis)
		//	DBG_LOG("After conversion, up axis is Y");
		//else if (after.GetAxisSystem().GetUpVector(sign) == KFbxAxisSystem::ZAxis)
		//	DBG_LOG("After conversion, up axis is Z");
		//DBG_LOG("After conversion, units: " << after.GetSystemUnit().GetScaleFactorAsString());

		KFbxNode* pRootNode = pScene->GetRootNode();
		if (pRootNode)
		{
			// Use material table in order to share materials with the same name
			ConvertNode(pRootNode, o_pSceneNode, o_MaterialTable,
						o_Fragments, o_Materials, i_Locator);
			//ConvertNode(pRootNode, o_pSceneNode, i_TextureFinder, material_table,
			//			o_Fragments, o_Materials, o_Textures);
		}
	}

}	// end of namespace


#endif // USE_FBX_IMPORTEXPORT