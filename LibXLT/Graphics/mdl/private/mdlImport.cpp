/****************************************************************************\
**	mdlImport.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlImport.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlHierarchyImportParser.hpp"
#include "Graphics/mdl/private/mdlImportUtil.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"
#include "Graphics/mdl/private/mdlMeshParser.hpp"
#include "Graphics/mdl/private/mdlRotationOrder.hpp"
#include "Graphics/mdl/private/mdlSkinParser.hpp"
#include "Graphics/mdl/private/mdlSubdivParser.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <set>
#include <string>

#undef FindResource


//============================================================================
//============================================================================
namespace mdlImport
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
const bool l_bDebugNodes = false;
//const bool l_bDebugAnimNodes = false;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_MHIE = chDefs::MakeName('M', 'H', 'I', 'E');
const chDefs::Name c_MSSK = chDefs::MakeName('M', 'S', 'S', 'K');
const chDefs::Name c_MCHR = chDefs::MakeName('M', 'C', 'H', 'R');

const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');
const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
const chDefs::Name c_SKIN = chDefs::MakeName('S', 'K', 'I', 'N');
const chDefs::Name c_MRPH = chDefs::MakeName('M', 'R', 'P', 'H');

const chDefs::Name c_SUBD = chDefs::MakeName('S', 'U', 'B', 'D');
const chDefs::Name c_GFAC = chDefs::MakeName('G', 'F', 'A', 'C');

const chDefs::Name c_JOIN = chDefs::MakeName('J', 'O', 'I', 'N');
const chDefs::Name c_JODA = chDefs::MakeName('J', 'O', 'D', 'A');
const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
const chDefs::Name c_JINF = chDefs::MakeName('J', 'I', 'N', 'F');
const chDefs::Name c_INFT = chDefs::MakeName('I', 'N', 'F', 'T');
const chDefs::Name c_DELT = chDefs::MakeName('D', 'E', 'L', 'T');

const chDefs::Name c_HLEV = chDefs::MakeName('H', 'L', 'E', 'V');

//----------------------------------------------------------------------------
// Internal structure for information related to creating
// fragments that are found in the file format in the hierarchy.
// Grouped into a struct here just to keep the parameter lists
// from getting too long.
//----------------------------------------------------------------------------
struct FragmentLoadingInfo
{
	//const fsResourceFinder& i_TextureFinder;
	mdlMatInfoTable &i_MaterialTable;

	std::vector<matMaterial*>& o_Materials;
	//std::vector<matTexture*>& o_Textures;
	entFragInfoSink* o_Sink;
};


//------------------------------------------------------------------------
// loads materials from material table, adding them to given lists.
// returns false if no materials were in table
//------------------------------------------------------------------------
bool create_material_table( mdlMatInfoTable &i_MaterialTable,
							//const fsResourceFinder& i_TextureFinder,
							std::vector<matMaterial*>& o_Materials)
							//std::vector<matTexture*>& o_Textures)
{
	if (i_MaterialTable.empty())
		return false;

	mdlMatInfoTable::iterator it = i_MaterialTable.begin();
	for (; it != i_MaterialTable.end(); ++it)
	{
		mdlMatInfo& mat_info = (*it->second);
		//bga - Not loading textures anymore, just creating the material pointer
		//mat_info.LoadTextures(i_TextureFinder, o_Textures);
		mat_info.CreateMaterial();
		o_Materials.push_back(mat_info.m_pMaterial);
	}
	return true;
}

//------------------------------------------------------------------------
// Reads in the geometry fragment chunk and creates g3dFragments
// as needed. Returns true if the fragments should be used in 
// the integrated low-res model .
//------------------------------------------------------------------------
g3dSceneNode::Resolution read_GFRG_and_make_fragments(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									FragmentLoadingInfo& io_LoadInfo,
									std::vector<g3dFragment*>& o_Fragments,
									bool i_Morphable)
{
	mdlFragInfo frag_info;

	// If mdlMeshParser::ReadGFRG returns false, then it means that the 
	// fragment was skipped because it had a resolution level that we
	// are skipping.
	if (mdlMeshParser::ReadGFRG(i_Reader,
						i_Version,
						i_Size,
						frag_info,
						false,
						&io_LoadInfo.i_MaterialTable))
	{

		bool create_materials = io_LoadInfo.i_MaterialTable.empty();

		mdlImportUtil::MakeFragments(	frag_info,
						//io_LoadInfo.i_TextureFinder,
						o_Fragments,
						io_LoadInfo.o_Materials,
						//io_LoadInfo.o_Textures,
						io_LoadInfo.o_Sink,
						i_Morphable,
						create_materials);
	}

	switch (frag_info.m_ResolutionLevel)
	{
	default:
	case 0:
		return g3dSceneNode::e_Mixed;
	case 1:
		return g3dSceneNode::e_LowRes;
	case 2:
		return g3dSceneNode::e_HighRes;
	}
}

//------------------------------------------------------------------------
// read tree of joints and data
//------------------------------------------------------------------------
void read_JOIN(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				g3dSceneNode*& o_pJoint,
				int& io_nJointIndex,
				std::vector<mdlSkinUtil::JointInfluence>& o_JointInfluences,
				int i_Depth,
				FragmentLoadingInfo& io_LoadInfo,
				std::vector<g3dFragment*>& o_Fragments)
{
	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	o_pJoint = new g3dSceneNode();
	o_pJoint->SetIsJoint(true);

	try
	{
		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			//DBG_LOG("read_JOIN, got chunk: " << (char*)(&name));
			if( name == c_NNAM )
			{
				std::string node_name;
				i_Reader.Read(node_name);
				o_pJoint->SetName( node_name.c_str() );

				if (l_bDebugNodes)
				{
					std::string tabs;
					for (int i=0; i<i_Depth; i++)
						tabs += " ";
					DBG_LOG("Read joint: " << tabs.c_str() << node_name.c_str());
				}
			}
			else if( name == c_JODA )
			{
				maVector3d translation, rotation, orientation, init_scale;
				chChunkParserUtil::Read(i_Reader, translation);
				chChunkParserUtil::Read(i_Reader, rotation);
				chChunkParserUtil::Read(i_Reader, orientation);
				chChunkParserUtil::Read(i_Reader, init_scale);

				if ( (orientation.m_X != 0) ||
					 (orientation.m_Y != 0) ||
					 (orientation.m_Z != 0) )
				{
					o_pJoint->SetOrientation( orientation.m_X, orientation.m_Y, orientation.m_Z );
				}

				maMatrix4x4 bind_pose;
				chChunkParserUtil::Read(i_Reader, bind_pose);
				bind_pose.Invert();
				o_pJoint->SetInvBindPose( bind_pose );

				// The influences have been moved to their own chunk in version 1
				if (version < 1)
				{
					envType::UInt16 i, num_influences;
					i_Reader.Read( num_influences );

					for( i = 0 ; i < num_influences ; i++ )
					{
						mdlSkinUtil::JointInfluence joint_infl;
						joint_infl.m_nJointIndex = io_nJointIndex;
						envType::UInt16 old_index; // old version only does 16-bit
						i_Reader.Read( old_index );
						joint_infl.m_nVertexIndex = old_index;
						i_Reader.Read( joint_infl.m_fWeight );
						o_JointInfluences.push_back( joint_infl );
					}
				}

				if (version >= 2)
				{
					// Later versions put the transformation matrix directly at the
					// end of the chunk so we don't have to recompose it.
					maMatrix4x4 transform;
					chChunkParserUtil::Read(i_Reader, transform);
					o_pJoint->SetTransform(transform);

					// Also adds scaleOrientation rotation
					maVector3d scaleOrient;
					chChunkParserUtil::Read(i_Reader, scaleOrient);
					if ( (scaleOrient.m_X != 0) ||
						 (scaleOrient.m_Y != 0) ||
						 (scaleOrient.m_Z != 0) )
					{
						o_pJoint->SetScaleOrientation( scaleOrient.m_X, scaleOrient.m_Y, scaleOrient.m_Z );
					}
				}
				else
				{
					maMatrix4x4 transform;
					maRotation base_rot;
					base_rot.SetEuler(rotation.m_X, rotation.m_Y, rotation.m_Z);
					//transform.MakeScale( init_scale.m_X, init_scale.m_Y, init_scale.m_Z );
					if (o_pJoint->GetOrientation())
						transform = base_rot.GetMatrix() * (*o_pJoint->GetOrientation());
					else
						transform = base_rot.GetMatrix();
					transform.TranslateBy( translation );
					o_pJoint->SetTransform(transform);
				}
			}
			else if( name == c_JINF )
			{
				envType::UInt32 i, num_influences;
				num_influences = mdlIndexUtil::ReadNumber(i_Reader, (version>=1));
				//DBG_LOG2("Read joint index %d num influences: %d", io_nJointIndex, num_influences);

				for( i = 0 ; i < num_influences ; i++ )
				{
					mdlSkinUtil::JointInfluence joint_infl;
					joint_infl.m_nJointIndex = io_nJointIndex;
					joint_infl.m_nVertexIndex = mdlIndexUtil::ReadNumber(i_Reader, (version>=1));
					i_Reader.Read( joint_infl.m_fWeight );
					o_JointInfluences.push_back( joint_infl );
				}
			}
			else if( name == c_JOIN )
			{
				DBG_ASSERT( o_pJoint, "Invalid model file" );
				g3dSceneNode* new_joint = NULL;

				read_JOIN( i_Reader, version, size, 
					new_joint, ++io_nJointIndex, o_JointInfluences, i_Depth+1,
					io_LoadInfo, o_Fragments);

				if (new_joint && o_pJoint)
					o_pJoint->AddChild(new_joint);
			}
			else if (name == c_HLEV)
			{
				// A transformation node under a joint tree is where
				// we put the static meshes and embedded level of details
				
				// Use a hierarchy parser to read this section in
				std::vector<mdlSkinInfo> skinned_surfaces;
				std::vector< shared_ptr<mdlHairInfo> > hair_surfaces;
				mdlHierarchyImportParser parser(i_Reader.GetLocator(), 
											    //io_LoadInfo.i_TextureFinder, 
												io_LoadInfo.i_MaterialTable,
												skinned_surfaces,
												hair_surfaces,
												o_Fragments,
												io_LoadInfo.o_Materials,
												//io_LoadInfo.o_Textures,
												io_LoadInfo.o_Sink);
				mdlHierarchyParser::LoadHierarchy(parser, i_Reader, name, version, size);
				DBG_ASSERT(skinned_surfaces.empty(), "Should not have been skinned surfaces in old file formats.");
				DBG_ASSERT(hair_surfaces.empty(), "Should not have been hair surfaces in old file formats.");
				if (parser.GetRootNode())
					o_pJoint->AddChild( parser.GetRootNode() );
			}

			i_Reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlImport::read_JOIN");
		throw mdlInvalidModelFileX(i_Reader.GetLocator());
	}
}


//------------------------------------------------------------------------
// remap_skin_data - A character skin for a single mdlFragInfo
// needs to be resorted into separate skins when there are
// multiple materials.
//------------------------------------------------------------------------
//void remap_skin_data(const smdlCharacterSkin &i_Skin, 
//					 const std::vector<mdlSplitFragInfo> &i_SplitFrags, 
//					 std::vector<smdlCharacterSkin>& o_SkinData)
//{
//	int nSplits = i_SplitFrags.size();
////	DBG_LOG("Num split fragments created: " << nSplits);
//
//	if (nSplits == 1)
//	{
//		o_SkinData.push_back(i_Skin);
//		return;
//	}
//
//	int nMorphs = i_Skin.m_MorphTargets.size();
//	int m = 0;
//
//	for (int f=0; f<nSplits; f++)
//	{
//		const mdlSplitFragInfo& split_frag = i_SplitFrags[f];
//		int nVerts = split_frag.m_RemapArray.size();
//		DBG_ASSERT(nVerts == split_frag.m_Vertices.size(), "Remap and Vertex array should be same length?");
//
//		smdlCharacterSkin skin;
//		skin.m_BoneVertices.resize(nVerts);
//		skin.m_MorphTargets.resize(nMorphs);
//		for (m=0; m<nMorphs; m++)
//		{
//			skin.m_MorphTargets[m].m_Name = i_Skin.m_MorphTargets[m].m_Name;
//			skin.m_MorphTargets[m].m_Offsets.resize(nVerts);
//		}
//
//		for (int v=0; v<nVerts; v++)
//		{
//			int remap = split_frag.m_RemapArray[v];
//			skin.m_BoneVertices[v] = i_Skin.m_BoneVertices[remap];
//
//			for (m=0; m<nMorphs; m++)
//			{
//				skin.m_MorphTargets[m].m_Offsets[v] = i_Skin.m_MorphTargets[m].m_Offsets[remap];
//			}
//		}
//		o_SkinData.push_back(skin);
//	}
//}

}	// end of local namespace



//------------------------------------------------------------------------
//	LoadWorldFragments loads a bunch of fragments which are already
//	placed in the world, and their associated textures and materials.
//------------------------------------------------------------------------
void LoadWorldFragments(const fsLocator& i_File,
						//const fsResourceFinder& i_TextureFinder,
						std::vector<g3dFragment*>& o_Fragments,
						std::vector<g3dFragment*>& o_LowResFragments,
						std::vector<g3dFragment*>& o_HighResFragments,
						mdlMatInfoTable& o_MaterialTable,
						std::vector<matMaterial*>& o_Materials,
						//std::vector<matTexture*>& o_Textures,
						entFragInfoSink* o_Sink)
{
	gfFileBin file(i_File, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	//mdlMatInfoTable material_table;

	// usean internal structure for holding the function args:
	FragmentLoadingInfo load_info =
	//{ i_TextureFinder, material_table, o_Materials, o_Textures, o_Sink };
	{ o_MaterialTable, o_Materials, o_Sink };

	try
	{
		while( reader.ReadChunkHeader(name, version, size) )
		{
			if (name == c_GFRG)
			{
				std::vector<g3dFragment*> new_fragments;
				g3dSceneNode::Resolution resolution
						= read_GFRG_and_make_fragments(	reader,
												version,
												size,
												load_info,
												new_fragments,
												false);

				if (resolution == g3dSceneNode::e_LowRes)
					std::copy(new_fragments.begin(), new_fragments.end(), 
								std::back_inserter(o_LowResFragments));
				else if (resolution == g3dSceneNode::e_HighRes)
					std::copy(new_fragments.begin(), new_fragments.end(), 
								std::back_inserter(o_HighResFragments));
				else
					std::copy(new_fragments.begin(), new_fragments.end(), 
								std::back_inserter(o_Fragments));
			}
			else if (name == c_MTBL)
			{
				// material table has shared materials for
				// fragments, has to be read before GFRG chunks
				mdlMaterialParser::ReadMaterialTable(   reader,
												version,
												size,
												o_MaterialTable);

				create_material_table(  o_MaterialTable,
									  //i_TextureFinder,
									  o_Materials);
									  //o_Textures);
			}
			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlImport::LoadWorldFragments");
		throw mdlInvalidModelFileX(i_File);
	}
}

//------------------------------------------------------------------------
//	LoadFragInfos loads fragment info and materials from a file, but
//  does not split or create fragments.
//------------------------------------------------------------------------
void LoadFragInfos(	const fsLocator& i_File,
					//const fsResourceFinder& i_TextureFinder,
					std::vector<mdlFragInfo>& o_FragInfos,
					mdlMatInfoTable& o_MaterialTable,
					std::vector<matMaterial*>& o_Materials,
					//std::vector<matTexture*>& o_Textures,
					entFragInfoSink* o_Sink )
{
	DBG_LOG("In LoadFragInfos" );

	gfFileBin file(i_File, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	//mdlMatInfoTable material_table;

	try
	{
		while( reader.ReadChunkHeader(name, version, size) )
		{
			if( name == c_GFRG )
			{
				DBG_LOG("LoadFragInfos load c_GFRG" );

				mdlFragInfo frag_info;
				mdlMeshParser::ReadGFRG(reader,
										version,
										size,
										frag_info,
										true,
										&o_MaterialTable);

				DBG_LOG("Done mdlMeshParser::ReadGFRG" );

				if (o_MaterialTable.empty())
				{
					DBG_WARNING("Old file format, material table is now required: " << reader.GetLocator());
					throw mdlInvalidModelFileX(reader.GetLocator());

					//int i;
					//int num_materials = frag_info.m_Materials.size();
					//for( i = 0 ; i < num_materials ; ++i )
					//{
					//	mdlMatInfo& mat_info = (*frag_info.m_Materials[i]);
					//	mat_info.LoadTextures(i_TextureFinder, o_Textures);
					//	o_Materials.push_back(mat_info.m_pMaterial);
					//}

					//DBG_LOG("Done load textures" );
				}

				// Add to returned fragment info list
				o_FragInfos.push_back(frag_info);

				// Submit frag info to sink, but use empty array,
				// since no fragment has been created yet.
				std::vector<g3dFragment*> fragments;
				if( o_Sink )
					o_Sink->ReceiveFragInfo( fragments, frag_info );
			}
			else if (name == c_MTBL)
			{
				// material table has shared materials for
				// fragments, has to be read before GFRG chunks
				mdlMaterialParser::ReadMaterialTable(   reader,
												version,
												size,
												o_MaterialTable);

				create_material_table(  o_MaterialTable,
									  //i_TextureFinder,
									  o_Materials);
									  //o_Textures);
			}

			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlImport::LoadFragInfos");
		throw mdlInvalidModelFileX(i_File);
	}

}

//------------------------------------------------------------------------
//	LoadModel loads a hierarchical model from a file.  It will create
//	fragments and assign materials to them as specified in the file.
//------------------------------------------------------------------------
//void LoadHierarchicalModel(	const fsLocator& i_Locator,
//							const fsResourceFinder& i_TextureFinder,
//							g3dSceneNode*& o_pSceneNode,
//							std::vector<g3dFragment*>& o_Fragments,
//							std::vector<matMaterial*>& o_Materials,
//							std::vector<matTexture*>& o_Textures,
//							entFragInfoSink* o_Sink)
//{
//	mdlMatInfoTable material_table;
//	mdlHierarchyImportParser parser(i_Locator, i_TextureFinder, material_table,
//				o_Fragments, o_Materials, o_Textures, o_Sink);
//	mdlHierarchyParser::LoadModel(i_Locator, parser);
//	o_pSceneNode = parser.GetRootNode();
//}


//------------------------------------------------------------------------
//	LoadModel loads a single-skin model from a file.  
//------------------------------------------------------------------------
void LoadSingleSkinModel(	const fsLocator& i_File,
							//const fsResourceFinder& i_TextureFinder,
							g3dSceneNode*& o_pJointTree,
							std::vector<smdlBoneVertex>& o_BoneData,
							std::vector<mdlFragInfo>& o_FragInfos,
							mdlMatInfoTable& o_MaterialTable,
							std::vector<matMaterial*>& o_Materials,
							//std::vector<matTexture*>& o_Textures,
							entFragInfoSink* o_Sink )
{
	DBG_LOG("In LoadSingleSkinModel" );

	gfFileBin file(i_File, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	reader.ReadChunkHeader(name, version, size);
	DBG_ASSERT(name == c_MSSK, "Expected MSSK chunk for single-skin model");

	int num_join = 0;
	mdlFragInfo frag_info;

	std::vector<mdlSkinUtil::JointInfluence> joint_influences;
	int nJointIndex = 0;
	g3dFragment *pFragment = NULL;

	// The single skin format is no longer being exported. It will
	// be going through the character file format now.  However,
	// there are still JNX files out there and in order to use the
	// changed supporting functions we need to have a material table 
	// and a list of fragments. However, the JNX format should not
	// have any of these, so we will assert and the end of the file.
	//mdlMatInfoTable unused_material_table;
	std::vector<g3dFragment*> unused_fragments;

	// usean internal structure for holding the function args:
	FragmentLoadingInfo load_info =
	//{ i_TextureFinder, unused_material_table, o_Materials, o_Textures, o_Sink };
	{ o_MaterialTable, o_Materials, o_Sink };

	try
	{
		while( reader.ReadChunkHeader(name, version, size) )
		{
			if( name == c_JOIN )
			{
				DBG_LOG("LoadSingleSkinModel load c_JOIN" );
				++num_join;
				const int depth = 0;
				read_JOIN(	reader,
							version,
							size,
							o_pJointTree,
							nJointIndex,
							joint_influences,
							depth,
							load_info,
							unused_fragments);
			}
			else if( name == c_GFRG )
			{
				DBG_LOG("LoadSingleSkinModel load c_GFRG" );
				DBG_ASSERT( pFragment == NULL, "Too many fragments in single skin model" );

				mdlMeshParser::ReadGFRG(	reader,
										version,
										size,
										frag_info,
										true );

				DBG_LOG("Done mdlMeshParser::ReadGFRG" );

				int i;
				int num_materials = frag_info.m_Materials.size();
				for( i = 0 ; i < num_materials ; ++i )
				{
					mdlMatInfo& mat_info = (*frag_info.m_Materials[i]);
					// No material tables in this file format, have to add
					// it ourselves.
					o_MaterialTable[mat_info.m_Info.GetMaterialName()] = frag_info.m_Materials[i];
					//bga - Not loading textures anymore, just creating the material pointer
					//mat_info.LoadTextures(i_TextureFinder, o_Textures);
					mat_info.CreateMaterial();
					o_Materials.push_back(mat_info.m_pMaterial);
				}

				DBG_LOG("Done load textures" );

				// Our current smdlSingleSkinObject implementation cannot handle 
				// fragments without texture coordinates. So, introduce texture coords 
				// if necessary (all with (0.0, 0.0) value.
				if (frag_info.m_UVs.empty())
				{
					frag_info.m_UVs.resize(frag_info.m_Vertices.size(), maPoint2d(0.0f, 0.0f));

				}

				// switched from split frag infos to just frag info
				//mdlFragUtil::SplitFragments(frag_info, o_FragInfos);
				o_FragInfos.push_back( frag_info );

				//o_Vertices = frag_info.m_Vertices;
				//o_Normals = frag_info.m_Normals;
				//o_TextureVertices = frag_info.m_UVs;
				//o_Indices = frag_info.m_Indices;

				DBG_LOG("Done SplitFragments" );

				// Submit frag info to sink, but use empty array,
				// since no fragment has been created yet.
				std::vector<g3dFragment*> fragments;
				if( o_Sink )
					o_Sink->ReceiveFragInfo( fragments, frag_info );
			}

			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlImport::LoadSingleSkinModel");
		throw mdlInvalidModelFileX(i_File);
	}

	DBG_ASSERT(num_join == 1, "Wrong number of JOIN chunks in MSSK chunk");
	
	DBG_ASSERT(unused_fragments.empty(), "Can't have embedded fragments in single skin model.");
	//DBG_ASSERT(unused_material_table.empty(), "Can't have material table in single skin model.");

	//	Now build the bone vertices
	mdlSkinUtil::BuildBoneVertices( joint_influences, 
									  frag_info.m_Vertices.size(),
									  frag_info.m_VertexRemap,
									  o_BoneData );
}


//------------------------------------------------------------------------
//	LoadCharacterModel loads information for a compound character model.
//		Character models have multiple skins attached to a single 
//		skeleton and express emotions through morph targets.
//------------------------------------------------------------------------
void LoadCharacterModel(	const fsLocator& i_File,
							//const fsResourceFinder& i_TextureFinder,
							g3dSceneNode*& o_pJointTree,
							std::vector<smdlCharacterSkin>& o_SkinData,
							std::vector<mdlSubdivInfo>& o_SubdivInfos,
							std::vector<mdlFragInfo>& o_FragInfos,
							std::vector<g3dFragment*>& o_Fragments,
							mdlMatInfoTable& o_MaterialTable,
							std::vector<matMaterial*>& o_Materials,
							//std::vector<matTexture*>& o_Textures,
							entFragInfoSink* o_Sink )
{
//	DBG_LOG("In LoadCharacterModel" );

	gfFileBin file(i_File, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	reader.ReadChunkHeader(name, version, size);
	DBG_ASSERT(name == c_MCHR, "Expected MCHR chunk for compound character model");

	int num_join = 0;
	std::vector<mdlSkinUtil::JointInfluence> joint_influences;
	int nJointIndex = 0;
	mdlMatInfoTable material_table;
	std::vector<smdlCharacterSkin> mesh_skin_data_list;	// have to put all mesh data at the end

	// usean internal structure for holding the function args:
	FragmentLoadingInfo load_info =
	//{ i_TextureFinder, material_table, o_Materials, o_Textures, o_Sink };
	{ material_table, o_Materials, o_Sink };

	try
	{
		while( reader.ReadChunkHeader(name, version, size) )
		{
//			DBG_LOG("LoadCharacterModel, got chunk: " << (char*)(&name));
			if (name == c_MTBL)
			{
				// material table has shared materials for
				// skins, has to be read before SKIN chunks
				mdlMaterialParser::ReadMaterialTable(   reader,
												version,
												size,
												material_table);

				create_material_table(  material_table,
									  //i_TextureFinder,
									  o_Materials);
									  //o_Textures);
			}
			else if( name == c_JOIN )
			{
//				DBG_LOG("LoadCharacterModel load c_JOIN" );
				++num_join;
				const int depth = 0;
				read_JOIN(	reader,
							version,
							size,
							o_pJointTree,
							nJointIndex,
							joint_influences,
							depth,
							load_info,
							o_Fragments);
				DBG_ASSERT(joint_influences.empty(), "New format requires joint influences be written into SKIN chunk.");
			}
			else if( name == c_SKIN )
			{
				// version 0 was a polygon based character system.
				// That style is completely unsupported now.
				if (version < 1)
					throw mdlInvalidModelFileX(i_File);

				// A Skin consists of a subdivision or mesh surface with joint influences 
				// and morph targets that is attached to the joint skeleton read in the 
				// JOIN chunk above.
				//
				shared_ptr<mdlSubdivInfo> subdiv_info;
				shared_ptr<mdlFragInfo> frag_info;
				smdlCharacterSkin skin;
				mdlSkinParser::ReadSKIN(	reader,
											version,
											size,
											skin,
											subdiv_info,
											frag_info,
											material_table);

				if (subdiv_info)
				{
					o_SubdivInfos.push_back( *subdiv_info );
					o_SkinData.push_back(skin);

					// Submit subdiv info to sink, but use empty array,
					// since no fragment has been created yet.
					std::vector<g3dFragment*> fragments;
					if( o_Sink )
						o_Sink->ReceiveFragInfo( fragments, *subdiv_info );
				}
				else if (frag_info)
				{
					o_FragInfos.push_back( *frag_info );

					// Put mesh data at end of list
					mesh_skin_data_list.push_back(skin);

					// Submit frag info to sink, but use empty array,
					// since no fragment has been created yet.
					std::vector<g3dFragment*> fragments;
					if( o_Sink )
						o_Sink->ReceiveFragInfo( fragments, *frag_info );
				}
				else
				{
					DBG_WARNING("Expected either subdiv or mesh info in SKIN chunk");
				}
			}

			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in mdlImport::LoadCharacterModel");
		throw mdlInvalidModelFileX(i_File);
	}

	DBG_ASSERT(num_join == 1, "Wrong number of JOIN chunks in MCHR chunk");

	// Put all skin data for mesh fragments at end of the skin data list
	std::copy(mesh_skin_data_list.begin(), mesh_skin_data_list.end(), std::back_inserter(o_SkinData));	
}


//------------------------------------------------------------------------
//	LoadGeneralHierarchicalModel loads a generalized hierarchical model 
//	from a file.  It handles nodes and joints, static fragments and
//	skined surfaces.  Static fragments are created directly in the
//	node graph, skinned surfaces are returned in the mdlSkinInfo vector
//	and need to be created by the object.
//------------------------------------------------------------------------
void LoadGeneralHierarchicalModel(	const fsLocator& i_Locator,
//							const fsResourceFinder& i_TextureFinder,
							g3dSceneNode*& o_pSceneNode,
							//std::vector<smdlJoint*>& o_JointRoots,
							std::vector<mdlSkinInfo>& o_SkinData,
							std::vector< shared_ptr<mdlHairInfo> >& o_HairData,
							std::vector<g3dFragment*>& o_Fragments,
							mdlMatInfoTable& o_MaterialTable,
							std::vector<matMaterial*>& o_Materials,
//							std::vector<matTexture*>& o_Textures,
							entFragInfoSink* o_Sink)
{
	mdlHierarchyImportParser parser(i_Locator, o_MaterialTable,
				o_SkinData, o_HairData, o_Fragments, o_Materials, o_Sink);
	mdlHierarchyParser::LoadModel(i_Locator, parser);
	o_pSceneNode = parser.GetRootNode();
	//o_JointRoots = parser.GetRootJoints();
}


}	// end of namespace

