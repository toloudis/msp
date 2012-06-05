/****************************************************************************\
**  mdlSkinParser.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlSkinParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
#include "Graphics/mdl/private/mdlMeshParser.hpp"
#include "Graphics/mdl/private/mdlSubdivParser.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

#include <algorithm>
#include <set>


//============================================================================
//	Any of these mdlSkinParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlSkinParser
{
	namespace
	{
		const chDefs::Name c_SKIN = chDefs::MakeName('S', 'K', 'I', 'N');
		const chDefs::Name c_INFT = chDefs::MakeName('I', 'N', 'F', 'T');
		const chDefs::Name c_JINF = chDefs::MakeName('J', 'I', 'N', 'F');
		//const chDefs::Name c_INFG = chDefs::MakeName('I', 'N', 'F', 'G');
		const chDefs::Name c_BNDP = chDefs::MakeName('B', 'N', 'D', 'P');
		const chDefs::Name c_BINF = chDefs::MakeName('B', 'I', 'N', 'F');
		const chDefs::Name c_SUBD = chDefs::MakeName('S', 'U', 'B', 'D');
		const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
		const chDefs::Name c_MRPH = chDefs::MakeName('M', 'R', 'P', 'H');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_WNAM = chDefs::MakeName('W', 'N', 'A', 'M');
		const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
		const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
		const chDefs::Name c_MDLT = chDefs::MakeName('M', 'D', 'L', 'T');
		const chDefs::Name c_MVER = chDefs::MakeName('M', 'V', 'E', 'R');

		//void remap_joint(smdlJoint* o_pJoint, std::multimap<int, int>& i_VertexRemap)
		//{
		//	std::vector<smdlJoint::Influence> new_influences;
		//	typedef std::multimap<int, int>::iterator MapIt;
		//
		//	int num_old_influences = o_pJoint->GetNumInfluences();
		//	int i;
		//
		//	for( i = 0 ; i < num_old_influences ; i++ )
		//	{
		//		const smdlJoint::Influence& old_influence = o_pJoint->GetInfluence(i);
		//		std::pair<MapIt, MapIt> range = i_VertexRemap.equal_range( old_influence.m_VertexNum );
		//		float weight = old_influence.m_Weight;
		//
		//		while( range.first != range.second )
		//		{
		//			smdlJoint::Influence new_influence;
		//			new_influence.m_VertexNum = range.first->second;
		//			new_influence.m_Weight = weight;
		//			new_influences.push_back(new_influence);
		//			++range.first;
		//		}
		//	}
		//
		//	o_pJoint->SetInfluences(&(new_influences[0]), new_influences.size());
		//
		//	int num_children = o_pJoint->GetNumChildren();
		//	for( i = 0 ; i < num_children ; i++ )
		//	{
		//		g3dTransformTreeNode* tree_node = o_pJoint->GetChild(i);
		//		smdlJoint* child_joint = dynamic_cast<smdlJoint*>(tree_node);
		//		DBG_ASSERT(child_joint, "expected smdlJoint");
		//		remap_joint(child_joint, i_VertexRemap);
		//	}
		//}

		//------------------------------------------------------------------------
		// build_morph_target - map the positions into offsets from the base mesh, 
		// remapping to the vertex/normal used in the fragment.
		//------------------------------------------------------------------------
		void build_morph_target(const std::vector<maPoint3d> &i_Vertices,
								const std::vector<envType::UInt32>& i_SparseIndices,
								const mdlSubdivInfo& i_SubdivInfo,
								smdlMorphTarget& o_MorphTarget)
		{
			int nVertices = i_SubdivInfo.m_Vertices.size();
			//DBG_ASSERT(nVertices == i_SubdivInfo.m_Normals.size(), "Num vertices don't match");

			o_MorphTarget.m_Offsets.resize( nVertices );

			const maPoint3d* pVertices = &i_Vertices[0];
			std::vector<maPoint3d> unsparse;
			if (!i_SparseIndices.empty())
			{
				unsparse.resize(nVertices, maPoint3d(0,0,0));
				for (int i=0; i<i_SparseIndices.size(); ++i)
				{
					unsparse[i_SparseIndices[i]] = i_Vertices[i];
				}
				pVertices = &unsparse[0];
			}


			std::multimap<int, int>::const_iterator it, end = i_SubdivInfo.m_VertexRemap.end();
			for( it = i_SubdivInfo.m_VertexRemap.begin(); it != end; ++it )
			{
				int nOldVertexIndex = it->first;
				int nNewVertexIndex = it->second;

				o_MorphTarget.m_Offsets[ nNewVertexIndex ] = pVertices[ nOldVertexIndex ];
			}
		}

		//------------------------------------------------------------------------
		// build_morph_target - map the positions into offsets from the base mesh, 
		// remapping to the vertex/normal used in the fragment.
		//------------------------------------------------------------------------
		void build_morph_target(const std::vector<maPoint3d> &i_Vertices,
								const std::vector<envType::UInt32>& i_SparseIndices,
								const mdlFragInfo& i_FragInfo,
								smdlMorphTarget& o_MorphTarget)
		{
			int nVertices = i_FragInfo.m_Vertices.size();
			DBG_ASSERT(nVertices == i_FragInfo.m_Normals.size(), "Number of vertices " << nVertices << " and normals " << i_FragInfo.m_Normals.size() << " don't match" );

			o_MorphTarget.m_Offsets.resize( nVertices );

			const maPoint3d* pVertices = &i_Vertices[0];
			std::vector<maPoint3d> unsparse;
			if (!i_SparseIndices.empty())
			{
				unsparse.resize(nVertices, maPoint3d(0,0,0));
				for (int i=0; i<i_SparseIndices.size(); ++i)
				{
					unsparse[i_SparseIndices[i]] = i_Vertices[i];
				}
				pVertices = &unsparse[0];
			}

			std::multimap<int, int>::const_iterator it;
			for( it = i_FragInfo.m_VertexRemap.begin(); it != i_FragInfo.m_VertexRemap.end(); ++it )
			{
				int nOldVertexIndex = it->first;
				int nNewVertexIndex = it->second;

				o_MorphTarget.m_Offsets[ nNewVertexIndex ] = pVertices[ nOldVertexIndex ];
			}	
			
			//for( it = i_FragInfo.m_NormalRemap.begin(); it != i_FragInfo.m_NormalRemap.end(); ++it )
			//{
			//	int nOldVertexIndex = it->first;
			//	int nNewVertexIndex = it->second;

			//	smdlMorphVertex& morph_vertex = o_MorphTarget.m_Offsets[ nNewVertexIndex ];
			//	morph_vertex.m_Normal = i_Normals[ nOldVertexIndex ];
			//}
		}

		//------------------------------------------------------------------------
		// Read in a morph target for a skin in a character model.
		//------------------------------------------------------------------------
		void read_MRPH(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						std::vector<maPoint3d> &o_Vertices,
						std::vector<envType::UInt32> &o_SparseIndices,
						bool &o_bReadDeltas,
						std::string &o_TargetName,
						std::string &o_AliasName)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			try
			{
				while( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if( name == c_NNAM )
					{
						i_Reader.Read(o_TargetName);
						//DBG_LOG("Read morph target name: " << o_TargetName.c_str());
					}
					else if( name == c_WNAM )
					{
						i_Reader.Read(o_AliasName);
						//DBG_LOG("Read morph aliased name: " << o_AliasName.c_str());
					}
					else if( name == c_MVER )
					{
						// MVER is newer version, adds boolean flag for whether vectors
						// are deltas or absolute positions.
						i_Reader.Read(o_bReadDeltas);
						envType::UInt32 num;
						i_Reader.Read(num);
						chChunkParserUtil::ReadArray(i_Reader, o_Vertices, num);
					}
					else if( name == c_GVER )
					{
						// GVER are always absolute
						o_bReadDeltas = false;
						envType::UInt16 num;
						i_Reader.Read(num);
						chChunkParserUtil::ReadArray(i_Reader, o_Vertices, num);
					}
					//else if( name == c_NVER )
					//{
					//	did_read_NVER = true;
					//	envType::UInt16 num;
					//	i_Reader.Read(num);
					//	o_Normals.resize(num);
					//	envType::UInt16 cur;
					//	for( cur = 0 ; cur < num ; cur++ )
					//	{
					//		chChunkParserUtil::Read(i_Reader, o_Normals[cur]);
					//		o_Normals[cur].Normalize();
					//	}
					//}
					else if( name == c_MDLT )
					{
						// Sparse Indices assign delta to a vertex index. If the delta is zero,
						// then the delta and index is not included.
						o_bReadDeltas = true;
						envType::UInt16 num;
						i_Reader.Read(num);
						chChunkParserUtil::ReadArray(i_Reader, o_Vertices, num);
						chChunkParserUtil::ReadArray(i_Reader, o_SparseIndices, num);
					}

					i_Reader.FinishChunk();
				}

			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in mdlImport::read_MRPH");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}
	}

	//------------------------------------------------------------------------
	// Read in a morph target for a subdivision surface skin in a 
	// character model.
	//------------------------------------------------------------------------
	void ReadMRPH(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					const mdlSubdivInfo& i_SubdivInfo,
					smdlMorphTarget& o_MorphTarget)
	{
		std::vector<maPoint3d> orig_vertices;
		std::vector<envType::UInt32> sparse_indices;

		o_MorphTarget.m_bOffsetsAreDeltas = false;
		
		// With i_Version==0, the morph target was based on the old indices from Maya.
		// After that version, the vertices have been sorted to already include the
		// VertexRemap within the surface info.
		if (i_Version == 0)
		{
			read_MRPH(	i_Reader, i_Version, i_Size,
						orig_vertices, 
						sparse_indices,
						o_MorphTarget.m_bOffsetsAreDeltas, 
						o_MorphTarget.m_Name, 
						o_MorphTarget.m_Alias);

			//	Now map the positions into offsets from the base mesh, remapping to
			//  the vertex/normal used in the fragment.
			DBG_ASSERT(!orig_vertices.empty(), "Did not read position data needed for making morph target.");
			build_morph_target( orig_vertices, sparse_indices, i_SubdivInfo, o_MorphTarget );
		}
		else
		{
			read_MRPH(	i_Reader, i_Version, i_Size,
						o_MorphTarget.m_Offsets, 
						sparse_indices,
						o_MorphTarget.m_bOffsetsAreDeltas, 
						o_MorphTarget.m_Name, 
						o_MorphTarget.m_Alias);
			DBG_ASSERT(sparse_indices.empty(), "Should not have read sparse indices in version>0 of morph target.");
		}
	}

	//------------------------------------------------------------------------
	// Read in a morph target for a polygon mesh skin in a 
	// character model.
	//------------------------------------------------------------------------
	void ReadMRPH(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					const mdlFragInfo& i_FragInfo,
					smdlMorphTarget& o_MorphTarget)
	{
		std::vector<maPoint3d> orig_vertices;
		std::vector<envType::UInt32> sparse_indices;

		o_MorphTarget.m_bOffsetsAreDeltas = false;
		// With i_Version==0, the morph target was based on the old indices from Maya.
		// After that version, the vertices have been sorted to already include the
		// VertexRemap within the surface info.
		if (i_Version == 0)
		{
			read_MRPH(	i_Reader, i_Version, i_Size,
						orig_vertices,  
						sparse_indices,
						o_MorphTarget.m_bOffsetsAreDeltas, 
						o_MorphTarget.m_Name, 
						o_MorphTarget.m_Alias);

			//	Now map the positions into offsets from the base mesh, remapping to
			//  the vertex/normal used in the fragment.
			DBG_ASSERT(!orig_vertices.empty(), "Did not read position data needed for making morph target.");
			//DBG_ASSERT(!orig_normals.empty(), "Did not read normal data needed for making morph target.");
			build_morph_target( orig_vertices, sparse_indices, i_FragInfo, o_MorphTarget );
		}
		else
		{
			read_MRPH(	i_Reader, i_Version, i_Size,
						o_MorphTarget.m_Offsets, 
						sparse_indices,
						o_MorphTarget.m_bOffsetsAreDeltas, 
						o_MorphTarget.m_Name, 
						o_MorphTarget.m_Alias);
			DBG_ASSERT(sparse_indices.empty(), "Should not have read sparse indices in version>0 of morph target.");
		}
	}

	//------------------------------------------------------------------------
	// Read tree of how joints influence a mesh
	//------------------------------------------------------------------------
	void ReadINFT(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					int& io_nJointIndex,
					std::vector<mdlSkinUtil::JointInfluence>& o_JointInfluences )
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if( name == c_JINF )
				{
					envType::UInt16 i, num_influences;
					num_influences = mdlIndexUtil::ReadNumber(i_Reader, (version>=1));
					//DBG_LOG2("Read joint index %d num influences: %d", io_nJointIndex, num_influences);

					for( i = 0 ; i < num_influences ; i++ )
					{
						mdlSkinUtil::JointInfluence joint_infl;
						joint_infl.m_nJointIndex = io_nJointIndex;
						joint_infl.m_nVertexIndex = mdlIndexUtil::ReadNumber(i_Reader, (version>=1));
						i_Reader.Read( joint_infl.m_fWeight );
						//DBG_LOG2("Read influence: %d %f", joint_infl.m_nVertexIndex, joint_infl.m_fWeight);
						o_JointInfluences.push_back( joint_infl );
					}
				}
				else if( name == c_INFT )
				{
					ReadINFT( i_Reader, version, size, ++io_nJointIndex, o_JointInfluences );
				}

				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in mdlImport::read_INFT");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}
	}

	//------------------------------------------------------------------------
	// Read vector of influences of how joints influence a surface
	//------------------------------------------------------------------------
	void ReadBINF(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					std::vector<smdlBoneVertex>& o_BoneVertices )
	{
		// version 0 was a different format used when testing.
		// That style is completely unsupported now.
		if (i_Version < 1)
			throw mdlInvalidModelFileX(i_Reader.GetLocator());

		envType::UInt32 numVerts = 0;
		i_Reader.Read( numVerts );
		o_BoneVertices.resize( numVerts );

		envType::UInt16 numInfls;
		for(int i = 0; i < numVerts; i++) 
		{
			i_Reader.Read( numInfls );
			o_BoneVertices[i].m_Influences.resize(numInfls);
		}

		//bga - I would prefer to read this into one large array and then
		// leave it in that array. That is why the file format is organized
		// this way. But, I will make that optimization later.
		envType::UInt32 totalNumInfluences = 0;
		i_Reader.Read( totalNumInfluences ); // not using this right now
		for(int i = 0; i < numVerts; i++) 
		{
			envType::UInt16 numInfls = o_BoneVertices[i].m_Influences.size();
			//DBG_LOG2("Vertex #%d : NumInfls: %d", i, numInfls);
			for(int j = 0; j < numInfls; j++) 
			{
				i_Reader.Read(o_BoneVertices[i].m_Influences[j].m_BoneIndex); 
				i_Reader.Read(o_BoneVertices[i].m_Influences[j].m_fWeight); 
				//DBG_LOG2("BoneIndex: %d Weight: %f", o_BoneVertices[i].m_Influences[j].m_BoneIndex, o_BoneVertices[i].m_Influences[j].m_fWeight);	
			}
		}
	}

	//------------------------------------------------------------------------
	// Read in a skin in multiple skin-single skeleton character model.
	// The shared_ptr arguments should be NULL when being passed in,
	// depending on what is read, they will be filled in with data.
	// Either a subdiv or a frag info will be read, not both.
	//------------------------------------------------------------------------
	void ReadSKIN(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					smdlCharacterSkin &o_Skin,
					shared_ptr<mdlSubdivInfo>& o_SubdivInfo,
					shared_ptr<mdlFragInfo>& o_MeshInfo,
					mdlMatInfoTable &i_MaterialTable )
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		try
		{
			std::vector<mdlSkinUtil::JointInfluence> joint_influences;
			int nJointIndex = 0;
			bool did_read_SUBD = false, did_read_GFRG = false;

			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if( name == c_SUBD )
				{
	//				DBG_LOG("read_SKIN load c_SUBD" );
					DBG_ASSERT( !did_read_GFRG && !did_read_SUBD, "Too many fragments in skin." );
					did_read_SUBD = true;

					o_SubdivInfo.reset(new mdlSubdivInfo());
					const bool fill_in_vertex_remap = true;
					mdlSubdivParser::ReadSUBD(	i_Reader,
											version,
											size,
											*o_SubdivInfo,
											fill_in_vertex_remap,
											&i_MaterialTable );

		//			DBG_LOG("Done mdlSubdivParser::ReadSUBD" );
				}
				else if( name == c_GFRG )
				{
	//				DBG_LOG("read_SKIN load c_GFRG" );
					DBG_ASSERT( !did_read_GFRG && !did_read_SUBD, "Too many fragments in skin." );
					did_read_GFRG = true;

					o_MeshInfo.reset(new mdlFragInfo());
					const bool fill_in_vertex_remap = true;
					mdlMeshParser::ReadGFRG(i_Reader,
											version,
											size,
											*o_MeshInfo,
											fill_in_vertex_remap,
											&i_MaterialTable );
					
		//			DBG_LOG("Done mdlMeshParser::ReadGFRG" );
				}
				else if( name == c_MRPH )
				{
	//				DBG_LOG("read_SKIN load c_MRPH" );
					DBG_ASSERT( did_read_SUBD || did_read_GFRG, "Have to read basic fragment info before reading morph targets." );

					shared_ptr<smdlMorphTarget> morph_target(new smdlMorphTarget());
					if (did_read_SUBD)
					{
						mdlSkinParser::ReadMRPH( i_Reader,
									version,
									size,
									*o_SubdivInfo,
									*morph_target );
					}
					else if (did_read_GFRG)
					{
						mdlSkinParser::ReadMRPH(	i_Reader,
									version,
									size,
									*o_MeshInfo,
									*morph_target );
					}

	//				DBG_LOG("Done read_MRPH" );

					// Add this frag info to the list to be joined into bone vertex list later
					o_Skin.m_MorphTargets.push_back( morph_target );
				}
				else if( name == c_INFT )
				{
	//				DBG_LOG("read_SKIN load c_INFT, influence tree" );
					mdlSkinParser::ReadINFT(  i_Reader, 
								version, 
								size, 
								nJointIndex, 
								joint_influences);
	//				DBG_LOG("Done mdlSkinParser::ReadINFT" );
				} 
				else if( name == c_BINF )
				{
	//				DBG_LOG("read_SKIN load c_BINF, influence vectors" );
					mdlSkinParser::ReadBINF(  i_Reader, 
								version, 
								size, 
								o_Skin.m_BoneVertices);
	//				DBG_LOG("Done mdlSkinParser::ReadBINF" );
				} 
				else if( name == c_BNDP )
				{
	//				DBG_LOG("read_SKIN load c_BNDP, bind pose" );
					chChunkParserUtil::Read(i_Reader, o_Skin.m_BindPose);
				} 

				i_Reader.FinishChunk();
			}
		
			//	Now build the bone vertices
			if (!joint_influences.empty())
			{
				DBG_ASSERT(did_read_SUBD || did_read_GFRG, "Did not read a geometry model in order to build joint data.");
				if (did_read_SUBD)
				{
					//mdlSkinUtil::BuildBoneVertices( joint_influences, *o_SubdivInfo, o_Skin.m_BoneVertices );
					mdlSkinUtil::BuildBoneVertices( joint_influences,
								o_SubdivInfo->m_Vertices.size(),
								o_SubdivInfo->m_VertexRemap,
								o_Skin.m_BoneVertices );
								//*o_Skin.m_SkinInfo );
				}
				else if (did_read_GFRG)
				{
					//mdlSkinUtil::BuildBoneVertices( joint_influences, *o_MeshInfo, o_Skin.m_BoneVertices );
					mdlSkinUtil::BuildBoneVertices( joint_influences,
								o_MeshInfo->m_Vertices.size(),
								o_MeshInfo->m_VertexRemap,
								o_Skin.m_BoneVertices );
								//*o_Skin.m_SkinInfo );
				}
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in mdlImport::read_SKIN");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}
		
	}

	//------------------------------------------------------------------------
	// Write joint influences for a surface.
	//------------------------------------------------------------------------
	//void WriteINFG(	chWriter& o_Writer,
	//				const mdlSkinWeights &i_SkinInfo )
	//{
	//	o_Writer.WriteChunkHeader(c_INFG, 0, false);

	//	// Number of vertices (used to determine size of weights array below)
	//	o_Writer.Write(i_SkinInfo.m_NumVertices); 

	//	// Write array of joint indices that influence this surface
	//	envType::UInt32 numBones = i_SkinInfo.m_BoneIndices.size();
	//	o_Writer.Write(numBones); 
	//	for(int i = 0; i < numBones; i++) 
	//	{
	//		o_Writer.Write(i_SkinInfo.m_BoneIndices[i]); 
	//	}

	//	// Write large array of float weights data (m_NumVertices * numBones)
	//	envType::UInt32 numWeights = i_SkinInfo.m_NumVertices*numBones;
	//	DBG_ASSERT(i_SkinInfo.m_Weights.size() == numWeights, "Weights array not correctly sized");
	//	DBG_LOG("Size of weights array: " << numWeights*sizeof(float));
	//	for(int i = 0; i < numWeights; i++) 
	//	{
	//		o_Writer.Write(i_SkinInfo.m_Weights[i]); 
	//	}

	//	o_Writer.FinishChunk(); // c_INFG

	//}
	//------------------------------------------------------------------------
	// Write joint influences for a surface.
	//------------------------------------------------------------------------
	void WriteBINF(	chWriter& o_Writer,
					const std::vector<smdlBoneVertex> &i_BoneVertices )
	{	
		// Bone Influences chunk 
		// version 1 writes numInfls per vertex all at first 
		// and then writes the influences themselves. Makes it easier
		// to read in large arrays at once from the file.
		o_Writer.WriteChunkHeader(c_BINF, 1, true);

		envType::UInt32 numVerts = i_BoneVertices.size();
		o_Writer.Write(numVerts); 
		// Write num influences per vertex first
		envType::UInt32 totalNumInfls = 0;
		for (int i = 0; i < numVerts; i++) 
		{
			envType::UInt16 numInfls = i_BoneVertices[i].m_Influences.size();
			o_Writer.Write(numInfls); 
			totalNumInfls += numInfls;
		}
		// Then write the influences themselves, allows us to read them
		// into one large array later.
		o_Writer.Write(totalNumInfls); 
		for (int i = 0; i < numVerts; i++) 
		{
			envType::UInt16 numInfls = i_BoneVertices[i].m_Influences.size();
			for(int j = 0; j < numInfls; j++) 
			{
				o_Writer.Write(i_BoneVertices[i].m_Influences[j].m_BoneIndex); 
				o_Writer.Write(i_BoneVertices[i].m_Influences[j].m_fWeight); 
			}
		}

		o_Writer.FinishChunk(); // c_BINF
	}

	//------------------------------------------------------------------------
	// Write a morph target for a surface
	//------------------------------------------------------------------------
	void WriteMRPH(	chWriter& o_Writer,
					const smdlMorphTarget &i_MorphTarget )
	{
		// Morph Target chunk
		// Raised to version 1 when switching to use pre-sorted vertices
		o_Writer.WriteChunkHeader(c_MRPH, 1, true);

		// Name of blend shape node
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		o_Writer.Write(i_MorphTarget.m_Name); 
		o_Writer.FinishChunk();

		// Aliased name of blend shape node
		o_Writer.WriteChunkHeader(c_WNAM, 0, false);
		o_Writer.Write(i_MorphTarget.m_Alias); 
		o_Writer.FinishChunk();

		// Positions or Deltas (use boolean flag to determine which)
		o_Writer.WriteChunkHeader(c_MVER, 0, false);
		o_Writer.Write(i_MorphTarget.m_bOffsetsAreDeltas);
		envType::UInt32 nVerts = i_MorphTarget.m_Offsets.size();
		o_Writer.Write(nVerts);
		for(int i = 0; i < nVerts; i++) 
		{
			chChunkParserUtil::Write(o_Writer, i_MorphTarget.m_Offsets[i]);
		}
		o_Writer.FinishChunk();

		o_Writer.FinishChunk(); // c_MRPH
	}

	//------------------------------------------------------------------------
	// Write skinning information and a surface. One function for meshes
	//	and one function for subdivs.
	//------------------------------------------------------------------------
	void WriteSKIN(	chWriter& o_Writer,
					const smdlCharacterSkin &i_Skin,
					const mdlSubdivInfo &i_SubdivInfo )
	{
		// Skinning information chunk
		// Raised to version 2 when switching to GHX file format and using INFG instead of INFT
		o_Writer.WriteChunkHeader(c_SKIN, 2, true);

		// Write the surface first.
		mdlSubdivParser::WriteSUBD(o_Writer, i_SubdivInfo);

		// Write bind pose only if non-identity
		if (!i_Skin.m_BindPose.IsIdentity())
		{
			o_Writer.WriteChunkHeader(c_BNDP, 0, false);
			chChunkParserUtil::Write(o_Writer, i_Skin.m_BindPose);
			o_Writer.FinishChunk(); // c_BNDP
		}

		// Write the joint influences as a vector of influence vectors, not a tree anymore
		if (!i_Skin.m_BoneVertices.empty())
		{
			mdlSkinParser::WriteBINF(o_Writer, i_Skin.m_BoneVertices);
		}

		// Write morph targets in separate chunks
		for (int i=0; i<i_Skin.m_MorphTargets.size(); ++i)
		{
			if (i_Skin.m_MorphTargets[i])
			{	
				mdlSkinParser::WriteMRPH(o_Writer, *i_Skin.m_MorphTargets[i]);
			}
		}

		o_Writer.FinishChunk(); // c_SKIN
	}
	void WriteSKIN(	chWriter& o_Writer,
					const smdlCharacterSkin &i_Skin,
					const mdlFragInfo &i_MeshInfo )
	{
		// Skinning information chunk
		// Raised to version 2 when switching to GHX file format and using INFG instead of INFT
		o_Writer.WriteChunkHeader(c_SKIN, 2, true);

		// Write the surface first.
		mdlMeshParser::WriteGFRG(o_Writer, i_MeshInfo);

		// Write bind pose only if non-identity
		if (!i_Skin.m_BindPose.IsIdentity())
		{
			o_Writer.WriteChunkHeader(c_BNDP, 0, false);
			chChunkParserUtil::Write(o_Writer, i_Skin.m_BindPose);
			o_Writer.FinishChunk(); // c_BNDP
		}

		// Write the joint influences as a vector of influence vectors, not a tree anymore
		if (!i_Skin.m_BoneVertices.empty())
		{
			mdlSkinParser::WriteBINF(o_Writer, i_Skin.m_BoneVertices);
		}

		// Write morph targets in separate chunks
		for (int i=0; i<i_Skin.m_MorphTargets.size(); ++i)
		{
			if (i_Skin.m_MorphTargets[i])
			{	
				mdlSkinParser::WriteMRPH(o_Writer, *i_Skin.m_MorphTargets[i]);
			}
		}

		o_Writer.FinishChunk(); // c_SKIN
	}
}

