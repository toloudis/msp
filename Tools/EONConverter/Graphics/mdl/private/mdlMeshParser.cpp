/****************************************************************************\
**  mdlMeshParser.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlMeshParser.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
//#include "Graphics/mdl/private/mdlMaterialParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
//#include "Graphics/Mat/matMaterial.hpp"

#include <set>
#include <algorithm>

//----------------------------------------------------------------------------
//	Any of these mdlMeshParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace mdlMeshParser
{
	namespace
	{
		const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
		const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
		const chDefs::Name c_TVER = chDefs::MakeName('T', 'V', 'E', 'R');
		const chDefs::Name c_VCOL = chDefs::MakeName('V', 'C', 'O', 'L');
		const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');
		const chDefs::Name c_MATR = chDefs::MakeName('M', 'A', 'T', 'R');
		const chDefs::Name c_MTID = chDefs::MakeName('M', 'T', 'I', 'D');
		const chDefs::Name c_GIND = chDefs::MakeName('G', 'I', 'N', 'D');
		const chDefs::Name c_NIND = chDefs::MakeName('N', 'I', 'N', 'D');
		const chDefs::Name c_TIND = chDefs::MakeName('T', 'I', 'N', 'D');
		const chDefs::Name c_CIND = chDefs::MakeName('C', 'I', 'N', 'D');
		const chDefs::Name c_SWLD = chDefs::MakeName('S', 'W', 'L', 'D');
		const chDefs::Name c_FLAG = chDefs::MakeName('F', 'L', 'A', 'G');
		const chDefs::Name c_GFAC = chDefs::MakeName('G', 'F', 'A', 'C');
		const chDefs::Name c_CRSV = chDefs::MakeName('C', 'R', 'S', 'V');
		const chDefs::Name c_CRSE = chDefs::MakeName('C', 'R', 'S', 'E');

		bool l_bSkipLowRes = false;
		bool l_bSkipHighRes = false;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		struct VertexInfo
		{
			VertexInfo() :	m_NormalIndex(-1),
							m_UVIndex(-1),
							m_VertexIndex(-1){}

			VertexInfo(	int i_VertexIndex,
						int i_NormalIndex,
						int i_UVIndex,
						int i_NewIndex) :	m_VertexIndex(i_VertexIndex),
											m_NormalIndex(i_NormalIndex),
											m_UVIndex(i_UVIndex),
											m_NewIndex(i_NewIndex) {}

			bool operator == (const VertexInfo& i_Vertex) const {	return	(m_NormalIndex == i_Vertex.m_NormalIndex) &&
																			(m_UVIndex == i_Vertex.m_UVIndex) &&
																			(m_VertexIndex == i_Vertex.m_VertexIndex); }

			bool operator < (const VertexInfo& i_Vertex) const
			{
				if( m_VertexIndex == i_Vertex.m_VertexIndex )
				{
					if( m_NormalIndex == i_Vertex.m_NormalIndex )
						return m_UVIndex < i_Vertex.m_UVIndex;
					else
						return m_NormalIndex < i_Vertex.m_NormalIndex;
				}
				else
					return m_VertexIndex < i_Vertex.m_VertexIndex;
			}

			int m_NormalIndex;
			int m_UVIndex;
			int m_VertexIndex;
			int m_NewIndex;
		};

	}

	//----------------------------------------------------------------------------
	// Set the loader to skip the loading of low resolution or high resolution
	//	models. Can save memory when it is known a certain version isn't needed.
	//----------------------------------------------------------------------------
	void SetSkipLowRes(bool i_bSkip)
	{
		l_bSkipLowRes = i_bSkip;
	}
	void SetSkipHighRes(bool i_bSkip)
	{
		l_bSkipHighRes = i_bSkip;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool ReadGFRG(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlFragInfo& o_FragInfo,
					bool i_FillInVertexRemap) //,
					//mdlMatInfoTable *i_MaterialTable)
	{
		if( mdlDefs::GetVerboseMode() )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			dbgLog::Write("");
			dbgLog::Write("mdlMeshParser::ReadGFRG loading from %s", filename.c_str());
		}

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		std::vector<maPoint3d> orig_vertices;
		std::vector<maVector3d> orig_normals;
		std::vector<maPoint2d> orig_uvs;
		std::vector<maFloatRGBA> orig_colors;
		//std::vector<matMaterial*> materials;

		// With version 1 of GFRG, the size of the indices in bytes
		// is written to each index chunk. And the "number of"
		// values are now 32-bit integers.
		bool bNeedIndexSize = (i_Version >= 1);
		typedef std::vector<envType::UInt32> IndexSet;

		std::vector<IndexSet> gindices, nindices, tindices, cindices;
		IndexSet weld_geom, weld_norm, weld_uv;

		bool did_read_GVER = false, did_read_NVER = false, did_read_GIND = false;
		bool did_read_NIND = false, did_read_VCOL = false, did_read_CIND = false;
		bool did_read_SWLD = false;
		bool has_texture_indices = false;
		bool shadow_hull = false, casts_shadows = true, receives_shadows = true;
		bool double_sided = false, triangle_sort = false, cloth = false;
		int low_res = -1;

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if( name == c_NNAM )
				{
					i_Reader.Read(o_FragInfo.m_Name);
				}
				else if( name == c_GVER )
				{
					did_read_GVER = true;
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					orig_vertices.resize(num);

					if (mdlDefs::c_ReadArraysAtOnce)
					{
						if (num > 0)
						{
							// Read the whole array at once. 
							i_Reader.Read(&orig_vertices[0], num * sizeof(maPoint3d));
						}
					}
					else
					{
						// Individual vector read
						envType::UInt32 cur;
						for( cur = 0 ; cur < num ; cur++ )
							chChunkParserUtil::Read(i_Reader, orig_vertices[cur]);
					}

				}
				else if( name == c_NVER )
				{
					did_read_NVER = true;
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					orig_normals.resize(num);
					
					if (mdlDefs::c_ReadArraysAtOnce)
					{
						if (num > 0)
						{
							// Read the whole array at once. 
							i_Reader.Read(&orig_normals[0], num * sizeof(maVector3d));
						}
					}
					else
					{
						// Individual vector read
						envType::UInt32 cur;
						for( cur = 0 ; cur < num ; cur++ )
						{
							chChunkParserUtil::Read(i_Reader, orig_normals[cur]);
							orig_normals[cur].Normalize();
						}
					}
				}
				else if( name == c_TVER )
				{
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					orig_uvs.resize(num);

					if (mdlDefs::c_ReadArraysAtOnce)
					{
						if (num > 0)
						{
							// Read the whole array at once. 
							i_Reader.Read(&orig_uvs[0], num * sizeof(maPoint2d));

							// Have to flip the Y coordinate
							envType::UInt32 cur;
							for( cur = 0 ; cur < num ; cur++ )
							{
								orig_uvs[cur].m_Y = 1 - orig_uvs[cur].m_Y;
							}
						}
					}
					else
					{
						// Individual vector read
						envType::UInt32 cur;
						for( cur = 0 ; cur < num ; cur++ )
						{
							chChunkParserUtil::Read(i_Reader, orig_uvs[cur]);
							orig_uvs[cur].m_Y = 1 - orig_uvs[cur].m_Y;
						}
					}
				}
				else if( name == c_VCOL )
				{
					did_read_VCOL = true;

					// vertex colors, RGB (0-1)
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					orig_colors.resize(num);
					envType::UInt32 cur;
					maVector3d colorRGB;
					for( cur = 0 ; cur < num ; cur++ )
					{
						chChunkParserUtil::Read(i_Reader, colorRGB);
						//DBG_LOG3("Read color %f %f %f", colorRGB.m_X, colorRGB.m_Y, colorRGB.m_Z);
						orig_colors[cur].Set(colorRGB.m_X, colorRGB.m_Y, colorRGB.m_Z, 1.0f);
					}
				}
				else if( name == c_MATR )
				{
					// Only read one material at a time so that
					// we don't confuse the effect and shader code.
					//envScopedLock material_lock(l_Mutex);

					//shared_ptr<mdlMatInfo> new_mat_info(new mdlMatInfo());
					//mdlMaterialParser::ReadMATR(i_Reader, version, size, new_mat_info->m_Info);
					//o_FragInfo.m_Materials.push_back(new_mat_info);

					DBG_WARNING0("File is not using shared material table, not supported");
				}
				else if( name == c_MTID)
				{
					std::string material_name;
					i_Reader.Read(material_name);
					o_FragInfo.m_MaterialNames.push_back(material_name);

					//DBG_LOG1("MTID material name %s", material_name.c_str() );

					//if (i_MaterialTable)
					//{
					//	mdlMatInfoTable::const_iterator it = i_MaterialTable->find(material_name);
					//	if (it != i_MaterialTable->end())
					//		o_FragInfo.m_Materials.push_back(it->second);
					//	else
					//	{
					//		std::string filename;
					//		fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
					//		DBG_LOG2("mdlImport::read_GFRG(): Did not find material id '%s' in table in %s", material_name.c_str(), filename.c_str());
					//		throw mdlInvalidModelFileX(i_Reader.GetLocator());
					//	}
					//}
					//else
					//{
					//	std::string filename;
					//	fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
					//	DBG_LOG2("mdlImport::read_GFRG(): Using material ids (%s), but no material table given in %s", material_name.c_str(), filename.c_str());
					//	throw mdlInvalidModelFileX(i_Reader.GetLocator());
					//}
				}
				else if( name == c_GIND )
				{
					did_read_GIND = true;
					envType::UInt32 num_tris = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					int num_indices = num_tris * 3;
					IndexSet temp(num_indices);
					bool b32Bit = false;
					if (bNeedIndexSize) 
						b32Bit = mdlIndexUtil::ReadIndexSize(i_Reader);

					mdlIndexUtil::ReadIndexSet(i_Reader, temp, num_indices, b32Bit);

					gindices.push_back(temp);
				}
				else if( name == c_NIND )
				{
					did_read_NIND = true;
					envType::UInt32 num_tris = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					int num_indices = num_tris * 3;
					IndexSet temp(num_indices);
					bool b32Bit = false;
					if (bNeedIndexSize) 
						b32Bit = mdlIndexUtil::ReadIndexSize(i_Reader);

					mdlIndexUtil::ReadIndexSet(i_Reader, temp, num_indices, b32Bit);

					nindices.push_back(temp);
				}
				else if( name == c_TIND )
				{
					envType::UInt16 texture_layer;
					i_Reader.Read(texture_layer);

					if( texture_layer == 0 )
					{
						//	This importer only handles 1 set of texture vertices right now, so no sense
						//	reading more
						has_texture_indices = true;
						envType::UInt32 num_tris = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
						int num_indices = num_tris * 3;
						IndexSet temp(num_indices);
						bool b32Bit = false;
						if (bNeedIndexSize) 
							b32Bit = mdlIndexUtil::ReadIndexSize(i_Reader);
					
						mdlIndexUtil::ReadIndexSet(i_Reader, temp, num_indices, b32Bit);

						tindices.push_back(temp);
					}
				}
				else if( name == c_CIND )
				{
					did_read_CIND = true;
					envType::UInt32 num_tris = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					int num_indices = num_tris * 3;
					IndexSet temp(num_indices);
					bool b32Bit = false;
					if (bNeedIndexSize) 
						b32Bit = mdlIndexUtil::ReadIndexSize(i_Reader);
					mdlIndexUtil::ReadIndexSet(i_Reader, temp, num_indices, b32Bit);

					cindices.push_back(temp);
				}
				else if( name == c_SWLD )
				{
/* bga - not using shadow welding now, so just wasting memory and making it
			harder to optimize if we read in the chunk
					// stencil shadow welding
					did_read_SWLD = true;

					envType::UInt16 num_inds, num_sets;
					i_Reader.Read(num_inds);
					i_Reader.Read(num_sets); // assuming this is 3 for now
					DBG_ASSERT0(num_sets >= 3, "Need 3 sets of indices for welding");

					weld_geom.resize(num_inds);
					weld_norm.resize(num_inds);
					weld_uv.resize(num_inds);
					bool b32Bit = false; // shadow welding doesn't have 32-bit version yet
					int cur;
					for( cur = 0 ; cur < num_inds ; ++cur )
						weld_geom[cur] = mdlIndexUtil::ReadNumber(i_Reader, b32Bit);
					for( cur = 0 ; cur < num_inds ; ++cur )
						weld_norm[cur] = mdlIndexUtil::ReadNumber(i_Reader, b32Bit);
					for( cur = 0 ; cur < num_inds ; ++cur )
						weld_uv[cur] = mdlIndexUtil::ReadNumber(i_Reader, b32Bit);
*/
				}
				else if( name == c_FLAG )
				{
					// fragment flags
					envType::Int8 flag_val;

					if (version >= 2)
					{
						// Version 2 has 5 flags
						i_Reader.Read(flag_val);
						double_sided = (flag_val != 0);
						i_Reader.Read(flag_val);
						triangle_sort = (flag_val != 0);
						i_Reader.Read(flag_val);
						casts_shadows = (flag_val != 0);
						i_Reader.Read(flag_val);
						receives_shadows = (flag_val != 0);
						i_Reader.Read(flag_val);
						shadow_hull = (flag_val != 0);

						// Version 3 adds low_res
						if (version >= 3)
						{
							// Version 5 makes resolution a 3-way state
							//  0 - appear in all resolutions
							//	1 - appear only in low resolutions
							//	2 - appear only in high resolutions
							i_Reader.Read(flag_val);
							low_res = flag_val;
							// Try to restore old behavior in older file formats, 
							// even if not really correct. (There was no mixed state
							// in older files, just low-res or high-res.)
							if ((version < 5) && (low_res == 0))
								low_res = 2; 

							// Version 4 adds cloth
							if (version >= 4)
							{
								i_Reader.Read(flag_val);
								cloth = (flag_val != 0);
							}
						}
					}
					else if (version == 1)
					{
						// Version 1 has only 2 flags, adding shadow hull flag
						i_Reader.Read(flag_val);
						casts_shadows = (flag_val != 0);
						i_Reader.Read(flag_val);
						shadow_hull = (flag_val != 0);
					}
					else if (version == 0)
					{
						// Version 0 has 4 flags:
						i_Reader.Read(flag_val);
						casts_shadows = (flag_val != 0);
						i_Reader.Read(flag_val);
						receives_shadows = (flag_val != 0);
						i_Reader.Read(flag_val);
						double_sided = (flag_val != 0);
						//i_Reader.Read(flag_val);
						//primaryVisibility = (flag_val != 0);
					}	 		
				}

				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;

			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_LOG1("mdlImport::read_GFRG(): Invalid chunk in %s", filename.c_str());
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

		DBG_ASSERT0(did_read_GVER, "Didn't find required GVER chunk in mdlImport::read_GFRG");
		DBG_ASSERT0(did_read_NVER, "Didn't find required NVER chunk in mdlImport::read_GFRG");
		DBG_ASSERT0(did_read_GIND, "Didn't find required GIND chunk in mdlImport::read_GFRG");
		DBG_ASSERT0(did_read_NIND, "Didn't find required NIND chunk in mdlImport::read_GFRG");
		DBG_ASSERT0(did_read_VCOL == did_read_CIND, "Need color indices if vertex colors are given");

		//	Now we must expand the vertices to Terawatt format, which doesn't share normals or
		//	texture vertices.
		//	Each unique conbination of geometry vertex index, normal index, and texture vertex index
		//	will turn into one combined vertex.
		std::set<VertexInfo> vertex_set;

		//	we know we'll have at least this many vertices
		int min_num = gindices[0].size() / 3;
		o_FragInfo.m_Vertices.reserve(min_num);
		o_FragInfo.m_Normals.reserve(min_num);
		o_FragInfo.m_UVs.reserve(min_num);
		if (did_read_VCOL)
			o_FragInfo.m_Colors.reserve(min_num);

		if( mdlDefs::GetVerboseMode() )
		{
			dbgLog::Write("total original position vertices: %d", orig_vertices.size());
			dbgLog::Write("total original normal vertices: %d", orig_normals.size());
			dbgLog::Write("total original texture vertices: %d", orig_uvs.size());
		}
		o_FragInfo.m_NumOrigVertices = orig_vertices.size();
		o_FragInfo.m_NumOrigNormals = orig_normals.size();

		// We could do this compression in the Maya exporter and then
		// avoid it here when the version number confirms that it has
		// been done in Maya already.
		//const bool c_DoConvertion = false;
		//if (c_DoConvertion && !i_FillInVertexRemap)
		// Do compression only on low resolution models
		if ((o_FragInfo.m_ResolutionLevel == 1) && !i_FillInVertexRemap)
		{
			// Look for duplicates in array in order to reduce the
			// number of expanded vertices
			mdlIndexUtil::ConvertIndices<maVector3d>(orig_vertices, gindices);
			mdlIndexUtil::ConvertIndices<maVector3d>(orig_normals, nindices);
			if( has_texture_indices )
				mdlIndexUtil::ConvertIndices<maPoint2d>(orig_uvs, tindices);
		}

		//	do this for each material
		int num_Materials = o_FragInfo.m_MaterialNames.size();
		int cur_material_index;
		for( cur_material_index = 0 ; cur_material_index < num_Materials ; cur_material_index++ )
		{
			DBG_ASSERT2(cur_material_index < gindices.size(), "Material index %d out of range of gindices %d", cur_material_index, gindices.size());
			IndexSet& vertex_indices = gindices[cur_material_index];
			DBG_ASSERT2(cur_material_index < nindices.size(), "Material index %d out of range of nindices %d", cur_material_index, nindices.size());
			IndexSet& normal_indices = nindices[cur_material_index];

			if( mdlDefs::GetVerboseMode() )
			{
				dbgLog::Write("Material %d --------", cur_material_index);
				dbgLog::Write("vertex indices: %d (%d tris)", vertex_indices.size(), vertex_indices.size() / 3);
				dbgLog::Write("normal indices: %d (%d tris)", normal_indices.size(), normal_indices.size() / 3);
			}

			if( cur_material_index != 0 )
				o_FragInfo.m_MaterialChanges.push_back(o_FragInfo.m_Indices.size() / 3);

			int num_indices = vertex_indices.size();
			int cur_index;

			if( has_texture_indices && (orig_uvs.size() > 0))
			{
				IndexSet& texture_indices = tindices[cur_material_index];
				DBG_ASSERT3((vertex_indices.size() == normal_indices.size()) && (normal_indices.size() == texture_indices.size()), "numbers of indices don't match, v:%d n:%d t:%d", vertex_indices.size(), normal_indices.size(), texture_indices.size());

				//	for each group of vertex, normal, uv index, see if we already made a new vertex
				for( cur_index = 0 ; cur_index < num_indices ; cur_index++ )
				{
					VertexInfo new_vertex(	vertex_indices[cur_index],
											normal_indices[cur_index],
											texture_indices[cur_index],
											o_FragInfo.m_Vertices.size());

					std::pair< std::set<VertexInfo>::iterator, bool> insert_result = vertex_set.insert(new_vertex);
					if( insert_result.second ) // actually inserted
					{
						if( i_FillInVertexRemap )
						{
							o_FragInfo.m_VertexRemap.insert(std::pair<int, int>(vertex_indices[cur_index], o_FragInfo.m_Vertices.size()));
							o_FragInfo.m_NormalRemap.insert(std::pair<int, int>(normal_indices[cur_index], o_FragInfo.m_Vertices.size()));
						}

						//	add new vertex
						o_FragInfo.m_Indices.push_back(new_vertex.m_NewIndex);
						o_FragInfo.m_Vertices.push_back(orig_vertices[new_vertex.m_VertexIndex]);
						o_FragInfo.m_Normals.push_back(orig_normals[new_vertex.m_NormalIndex]);
						o_FragInfo.m_UVs.push_back(orig_uvs[new_vertex.m_UVIndex]);

						if (did_read_VCOL)
						{
							DBG_ASSERT2(cur_material_index < cindices.size(), "Material index %d out of range of cindices %d", cur_material_index, cindices.size());
							IndexSet& color_indices = cindices[cur_material_index];

							int col_index = color_indices[cur_index];
							DBG_ASSERT2(col_index < orig_colors.size(), "Color index out of range: %d %d", col_index, orig_colors.size());
							maFloatRGBA color = orig_colors[col_index];
							o_FragInfo.m_Colors.push_back(orig_colors[col_index]);
						}
					}
					else
					{
						//	reuse old vertex
						o_FragInfo.m_Indices.push_back( insert_result.first->m_NewIndex );
					}
				}
			}
			else
			{
				DBG_ASSERT0(vertex_indices.size() == normal_indices.size(), "numbers of indices don't match");

				//	for each group of vertex, normal, uv index, see if we already made a new vertex
				for( cur_index = 0 ; cur_index < num_indices ; cur_index++ )
				{
					VertexInfo new_vertex(	vertex_indices[cur_index],
											normal_indices[cur_index],
											-1,
											o_FragInfo.m_Vertices.size());

					std::pair< std::set<VertexInfo>::iterator, bool> insert_result = vertex_set.insert(new_vertex);
					if( insert_result.second ) // actually inserted
					{
						if( i_FillInVertexRemap )
						{
							o_FragInfo.m_VertexRemap.insert(std::pair<int, int>(vertex_indices[cur_index], o_FragInfo.m_Vertices.size()));
							o_FragInfo.m_NormalRemap.insert(std::pair<int, int>(normal_indices[cur_index], o_FragInfo.m_Vertices.size()));
						}

						//	add new vertex
						o_FragInfo.m_Vertices.push_back(orig_vertices[new_vertex.m_VertexIndex]);
						o_FragInfo.m_Normals.push_back(orig_normals[new_vertex.m_NormalIndex]);
						o_FragInfo.m_Indices.push_back(new_vertex.m_NewIndex);

						if (did_read_VCOL)
						{
							DBG_ASSERT2(cur_material_index < cindices.size(), "Material index %d out of range of cindices %d", cur_material_index, cindices.size());
							IndexSet& color_indices = cindices[cur_material_index];

							o_FragInfo.m_Colors.push_back(orig_colors[color_indices[cur_index]]);
						}
					}
					else
					{
						//	reuse old vertex
						o_FragInfo.m_Indices.push_back( insert_result.first->m_NewIndex );
					}
				}
			}
		}

		if( mdlDefs::GetVerboseMode() )
		{
			dbgLog::Write("Total expanded vertices: %d", o_FragInfo.m_Vertices.size());
			dbgLog::Write("Total expanded indices: %d (%d tris)", o_FragInfo.m_Indices.size(), o_FragInfo.m_Indices.size() / 3);
			dbgLog::Write("avg vertices per tri: %0.4f", 3.0f * float(o_FragInfo.m_Vertices.size()) / float(o_FragInfo.m_Indices.size()));
		}

		if (did_read_SWLD)
		{
			// Map welded shadow vertices
			int num_weld = weld_geom.size();
			DBG_ASSERT0(weld_norm.size() == num_weld, "number of weld indices don't match");
			DBG_ASSERT0(weld_uv.size() == num_weld, "number of weld indices don't match");

			o_FragInfo.m_WeldIndices.resize(num_weld);
			for( int cur_index = 0 ; cur_index < num_weld ; cur_index++ )
			{
				VertexInfo new_vertex(	weld_geom[cur_index],
										weld_norm[cur_index],
										weld_uv[cur_index],
										0);
				std::set<VertexInfo>::iterator it = vertex_set.find(new_vertex);

				// TODO: do we want this assert here or silently fail?!?
				//
				//DBG_ASSERT0(it != vertex_set.end(), "Can't find weld vertex");
				if (it != vertex_set.end())
				{
					o_FragInfo.m_WeldIndices[cur_index] = it->m_NewIndex;
				}
				else
				{
					//std::string filename;
					//fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
					//DBG_WARNING2("mdlMeshParser:: can't find weld vertex %d in %s setting index to zero", cur_index, filename.c_str());

					o_FragInfo.m_WeldIndices[cur_index] = 0;
				}
			}
		}

		// Flags
		//o_FragInfo.m_Flags.m_bCastsShadow = did_read_SWLD;
		o_FragInfo.m_Flags.m_bCastsShadow = casts_shadows;
		o_FragInfo.m_Flags.m_bReceivesShadow = receives_shadows;
		o_FragInfo.m_Flags.m_bShadowHull = shadow_hull;
		o_FragInfo.m_Flags.m_bDoubleSided = double_sided;
		o_FragInfo.m_Flags.m_bTriangleSort = triangle_sort;
		o_FragInfo.m_Flags.m_bVertexAnimation = cloth;
		
		// If no low-res flag was read, make it high-resolution.
		// If the model consists completely of high-resolution models,
		// it will be converted to all "mixed" when the scCompoundObject 
		// is created.
		o_FragInfo.m_ResolutionLevel = (low_res<0) ? 2 : low_res;

		// Skip fragment only works when the resolution was set
		// from the FLAG structure.
		bool bSkipFragment = ( (l_bSkipLowRes && (low_res == 1)) 
			|| (l_bSkipHighRes && (low_res == 2)) );
		return (!bSkipFragment);
	}

}

