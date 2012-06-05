/****************************************************************************\
**	mdlSubdivParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlSubdivParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"

#include <algorithm>
#include <set>


//============================================================================
//	Any of these mdlSubdivParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlSubdivParser
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	namespace
	{
		const chDefs::Name c_SUBD = chDefs::MakeName('S', 'U', 'B', 'D');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
		const chDefs::Name c_TVER = chDefs::MakeName('T', 'V', 'E', 'R');
		const chDefs::Name c_MTLS = chDefs::MakeName('M', 'T', 'L', 'S');
		const chDefs::Name c_MATR = chDefs::MakeName('M', 'A', 'T', 'R');
		const chDefs::Name c_MTID = chDefs::MakeName('M', 'T', 'I', 'D');
		const chDefs::Name c_TIND = chDefs::MakeName('T', 'I', 'N', 'D');
		const chDefs::Name c_FLAG = chDefs::MakeName('F', 'L', 'A', 'G');
		const chDefs::Name c_VRMP = chDefs::MakeName('V', 'R', 'M', 'P');
		const chDefs::Name c_GFAC = chDefs::MakeName('G', 'F', 'A', 'C');
		const chDefs::Name c_CRSV = chDefs::MakeName('C', 'R', 'S', 'V');
		const chDefs::Name c_CRSE = chDefs::MakeName('C', 'R', 'S', 'E');

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		struct VertexInfo
		{
			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
			VertexInfo() :	m_NormalIndex(-1),
							m_UVIndex(-1),
							m_VertexIndex(-1){}

			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
			VertexInfo(	int i_VertexIndex,
						int i_NormalIndex,
						int i_UVIndex,
						int i_NewIndex) :	m_VertexIndex(i_VertexIndex),
											m_NormalIndex(i_NormalIndex),
											m_UVIndex(i_UVIndex),
											m_NewIndex(i_NewIndex) {}

			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
			bool operator == (const VertexInfo& i_Vertex) const {	return	(m_NormalIndex == i_Vertex.m_NormalIndex) &&
																			(m_UVIndex == i_Vertex.m_UVIndex) &&
																			(m_VertexIndex == i_Vertex.m_VertexIndex); }

			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
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

			//----------------------------------------------------------------------------
			//----------------------------------------------------------------------------
			int m_NormalIndex;
			int m_UVIndex;
			int m_VertexIndex;
			int m_NewIndex;
		};

	}


	//----------------------------------------------------------------------------
	//	read subdivision surface information
	//----------------------------------------------------------------------------
	void ReadSUBD(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlSubdivInfo& o_SubdivInfo,
					bool i_FillInVertexRemap,
					const mdlMatInfoTable *i_MaterialTable)
	{
		if( mdlDefs::GetVerboseMode() )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_TEXT("");
			DBG_TEXT("mdlSubdivParser::ReadSUBD loading from %s" << filename.c_str());
		}

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		std::vector<maPoint3d> orig_vertices;
		std::vector<maPoint2d> orig_uvs;
		std::vector<envType::UInt32> orig_geom_inds;
		std::vector<envType::UInt32> orig_uv_inds;

		bool did_read_GVER = false, did_read_GFAC = false, did_read_VRMP = false;
		bool has_texture_indices = false;

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if( name == c_NNAM )
				{
					i_Reader.Read(o_SubdivInfo.m_Name);
					//DBG_LOG(" Read subdiv named: " << o_SubdivInfo.m_Name.c_str());
				}
				else if( name == c_GVER )
				{
					did_read_GVER = true;
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 2));
					chChunkParserUtil::ReadArray(i_Reader, orig_vertices, num);
				}
				else if( name == c_TVER )
				{
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 2));
					chChunkParserUtil::ReadArray(i_Reader, orig_uvs, num);

					// Only flip UVs if we are reading in an older version
					if (version < 2)
					{
						for (envType::UInt32 cur = 0 ; cur < num ; cur++ )
						{
							orig_uvs[cur].m_Y = 1 - orig_uvs[cur].m_Y;
						}
					}
//					DBG_LOG("Read " << num << " tex coords" );
				}
				else if( name == c_MTLS)
				{
					envType::UInt16 num_mats;
					i_Reader.Read(num_mats);
					for (int i=0; i<num_mats; ++i)
					{
						std::string material_name;
						i_Reader.Read(material_name);

						//DBG_LOG("MTID material name " << material_name.c_str() );

						// Find material by name from the material table
						if (i_MaterialTable)
						{
							mdlMatInfoTable::const_iterator it = i_MaterialTable->find(material_name);
							if (it != i_MaterialTable->end())
								o_SubdivInfo.m_Materials.push_back(it->second);
							else
							{
								std::string filename;
								fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
								DBG_LOG("mdlSubdivParser::ReadSUBD(): Did not find material id '" << material_name.c_str() << "' in table in " << filename.c_str());
								throw mdlInvalidModelFileX(i_Reader.GetLocator());
							}
						}
						else
						{
							std::string filename;
							fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
							DBG_LOG("mdlSubdivParser::ReadSUBD(): Using material ids (" << material_name.c_str() << "), but no material table given in " << filename.c_str());
							throw mdlInvalidModelFileX(i_Reader.GetLocator());
						}

						// Read material changes (index when next material should be used).			
						if (i<num_mats-1)
						{
							envType::Int32 ind;
							i_Reader.Read(ind);
							o_SubdivInfo.m_MaterialChanges.push_back(ind);
						}
					}
				}
				else if( name == c_MATR )
				{
					// MATR chunk is only used in very old file formats with single materials

					// Only read one material at a time so that
					// we don't confuse the effect and shader code.
					//envScopedLock material_lock(l_Mutex);

					shared_ptr<mdlMatInfo> new_mat_info(new mdlMatInfo());
					mdlMaterialParser::ReadMATR(i_Reader, version, size, new_mat_info->m_Info);
					//o_SubdivInfo.m_Material = new_mat_info;
					o_SubdivInfo.m_Materials.push_back( new_mat_info );
				}
				else if( name == c_MTID)
				{
					// MTID with version==1 means that a MTLS chunk was also written
					// and the MTID chunk should be ignored. MTID is only added to the
					// file in order to support backwards compatibility and old parsers.
					if (version < 1)
					{
						std::string material_name;
						i_Reader.Read(material_name);

						mdlMatInfoTable::const_iterator it = i_MaterialTable->find(material_name);
						if (it != i_MaterialTable->end())
						{
							//o_SubdivInfo.m_Material = it->second;
							o_SubdivInfo.m_Materials.push_back( it->second );
						}
						else
						{
							std::string filename;
							fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
							DBG_LOG("mdlImport::read_SUBD(): Did not find material id '" << material_name.c_str() << "' in table in " << filename.c_str());
							throw mdlInvalidModelFileX(i_Reader.GetLocator());
						}
					}
				}
				else if( name == c_GFAC )
				{
					did_read_GFAC = true;
					// Version 2 uses 32-bit for number of faces and indices
					envType::UInt32 num_faces = mdlIndexUtil::ReadNumber(i_Reader, (version >= 2));
					envType::UInt32 num_indices = mdlIndexUtil::ReadNumber(i_Reader, (version >= 2));
					o_SubdivInfo.m_NumFaces = num_faces;
					orig_geom_inds.resize(num_indices);
					//DBG_LOG2("Reading %d subdiv indices, including %d faces", num_indices, num_faces);
					
					if (version >= 1)
					{
						// Version 1 is just a 32-bit array in order to
						// not have to have inner loops per polygon.
						chChunkParserUtil::ReadArray(i_Reader, orig_geom_inds, num_indices);
					}
					else
					{
						envType::UInt8 num_poly_verts;
						envType::UInt16 gind;
						int ind = 0;
						for (int f=0; f<num_faces; f++)
						{
							i_Reader.Read(num_poly_verts);
							//DBG_LOG("num_poly_verts: " << num_poly_verts);
							orig_geom_inds[ind++] = num_poly_verts;

							for (int fv=0; fv<num_poly_verts; fv++)
							{
								i_Reader.Read(gind);
								//DBG_LOG("gind: " << gind);
								orig_geom_inds[ind++] = gind;
							}
						}
					}
				}
				else if( name == c_TIND )
				{
					has_texture_indices = true;
//					DBG_LOG("Reading texture indices");

					envType::UInt16 num_faces, num_indices;
					i_Reader.Read(num_faces);
					i_Reader.Read(num_indices);
					orig_uv_inds.resize(num_indices);
					
					envType::UInt8 num_poly_verts;
					envType::UInt16 gind;
					int ind = 0;
					for (int f=0; f<num_faces; f++)
					{
						i_Reader.Read(num_poly_verts);
						orig_uv_inds[ind++] = num_poly_verts;

						for (int fv=0; fv<num_poly_verts; fv++)
						{
							i_Reader.Read(gind);
							orig_uv_inds[ind++] = gind;
						}
					}
				}
				else if( name == c_VRMP )
				{
					did_read_VRMP = true;
					envType::UInt32 num_orig;
					i_Reader.Read(num_orig);
					o_SubdivInfo.m_NumOrigVertices = num_orig;
					mdlIndexUtil::ReadRemap(i_Reader, o_SubdivInfo.m_VertexRemap);
				}
				else if( name == c_CRSV )
				{
					envType::UInt16 num, maxlevel;
					i_Reader.Read(maxlevel);
					o_SubdivInfo.m_MaxVertexCreaseLevel = maxlevel;
					i_Reader.Read(num);
					o_SubdivInfo.m_VertexCreases.resize(num);
					envType::UInt32 base, path, corner;
					envType::UInt16 first, level;
					for (int c = 0 ; c < num ; c++ )
					{
						i_Reader.Read(base);
						o_SubdivInfo.m_VertexCreases[c].m_Base = base;
						i_Reader.Read(first);
						o_SubdivInfo.m_VertexCreases[c].m_First = first;
						i_Reader.Read(level);
						o_SubdivInfo.m_VertexCreases[c].m_Level = level;
						i_Reader.Read(path);
						o_SubdivInfo.m_VertexCreases[c].m_Path = path;
						i_Reader.Read(corner);
						o_SubdivInfo.m_VertexCreases[c].m_Corner = corner;
					}
					//DBG_LOG("Read " << num << " vertex creases" );
				}
				else if( name == c_CRSE )
				{
					envType::UInt16 num, maxlevel;
					i_Reader.Read(maxlevel);
					o_SubdivInfo.m_MaxEdgeCreaseLevel = maxlevel;
					i_Reader.Read(num);
					o_SubdivInfo.m_EdgeCreases.resize(num);
					envType::UInt32 base, path, corner;
					envType::UInt16 first, level;
					for (int c = 0 ; c < num ; c++ )
					{
						i_Reader.Read(base);
						o_SubdivInfo.m_EdgeCreases[c].m_Base = base;
						i_Reader.Read(first);
						o_SubdivInfo.m_EdgeCreases[c].m_First = first;
						i_Reader.Read(level);
						o_SubdivInfo.m_EdgeCreases[c].m_Level = level;
						i_Reader.Read(path);
						o_SubdivInfo.m_EdgeCreases[c].m_Path = path;
						i_Reader.Read(corner);
						o_SubdivInfo.m_EdgeCreases[c].m_Corner = corner;

						//DBG_LOG3("Read edge crease base %d level %d corner %d", base, level, corner);
					}
					//DBG_LOG("Read " << num << " edge creases" );
				}
				else if( name == c_FLAG )
				{
					// fragment flags
					//did_read_FLAG = true;
					envType::Int8 doubleSided;
					envType::Int8 triangleSort;

					// Version 0 has 2 flags, doubleSided and triangleSort:
					i_Reader.Read(doubleSided);
					i_Reader.Read(triangleSort);
					o_SubdivInfo.m_Flags.m_bDoubleSided = (doubleSided != 0);
					o_SubdivInfo.m_Flags.m_bTriangleSort = (triangleSort != 0);

					// skipped version 1 in order to sync with polygon flags
					if (version >= 2)
					{
						envType::Int8 castsShadow;
						envType::Int8 receivesShadow;
						envType::Int8 shadowHull;

						i_Reader.Read(castsShadow);
						o_SubdivInfo.m_Flags.m_bCastsShadow = (castsShadow != 0);
						i_Reader.Read(receivesShadow);
						o_SubdivInfo.m_Flags.m_bReceivesShadow = (receivesShadow != 0);
						i_Reader.Read(shadowHull);
						o_SubdivInfo.m_Flags.m_bShadowHull = (shadowHull != 0);

						// version 3 adds control of when to auto generate low resolution
						if (version >= 3)
						{
							envType::Int8 autoGenLowRes;
							i_Reader.Read(autoGenLowRes);
							o_SubdivInfo.m_Flags.m_bAutoGenLowRes = (autoGenLowRes != 0);
						}
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
			DBG_LOG("mdlImport::read_SUBD(): Invalid chunk in " << filename.c_str());
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

		DBG_ASSERT(did_read_GVER, "Didn't find required GVER chunk in mdlSubdivParser::read_SUBD");
		DBG_ASSERT(did_read_GFAC, "Didn't find required GFAC chunk in mdlSubdivParser::read_SUBD");

		// Newer versions of the file format are already sorted
		if (i_Version >= 2)
		{
			// Use "swap" to move the arrays quickly without copying
			o_SubdivInfo.m_Indices.swap( orig_geom_inds );
			o_SubdivInfo.m_Vertices.swap( orig_vertices );
			o_SubdivInfo.m_UVs.swap( orig_uvs );
		}
		else
		{
			// Resort the vertices. No sharing of texture coordinate info in Terawatt format.
			// Note: Only need to resort if there are texture coordinates.
			//
			o_SubdivInfo.m_NumOrigVertices = orig_vertices.size();
			if( has_texture_indices && (orig_uvs.size() > 0))
			{
				DBG_ASSERT((orig_geom_inds.size() == orig_uv_inds.size()), "numbers of indices don't match");

				// TODO: - reserve some number of vertices in std::vectors in SubdivInfo,
				// hard to compute how many is a good guess.
				//o_SubdivInfo.m_Vertices.reserve(min_num);
				//o_SubdivInfo.m_UVs.reserve(min_num);
				o_SubdivInfo.m_Indices.reserve(orig_geom_inds.size()); // this will match

				// Each unique combination of geometry vertex index and texture vertex index
				// will turn into one combined vertex. 
				// Note: Normal do not exist, so normal indices will not be used.
				std::set<VertexInfo> vertex_set;

				int cur_index = 0, num_poly_verts = 0;
				for (int f=0; f<o_SubdivInfo.m_NumFaces; f++)
				{
					num_poly_verts = orig_geom_inds[cur_index++];
					o_SubdivInfo.m_Indices.push_back( num_poly_verts );

					for (int fv=0; fv<num_poly_verts; fv++, cur_index++)
					{
						VertexInfo new_vertex(	orig_geom_inds[cur_index],
							-1,	// no normals in this format
							orig_uv_inds[cur_index],
							o_SubdivInfo.m_Vertices.size());

						std::pair< std::set<VertexInfo>::iterator, bool> insert_result = 
							vertex_set.insert(new_vertex);\
						if ( insert_result.second ) // actually inserted
						{
							if( i_FillInVertexRemap )
								o_SubdivInfo.m_VertexRemap.insert(std::pair<int, int>(orig_geom_inds[cur_index], new_vertex.m_NewIndex));

							//	add new vertex
							o_SubdivInfo.m_Indices.push_back(new_vertex.m_NewIndex);
							o_SubdivInfo.m_Vertices.push_back(orig_vertices[new_vertex.m_VertexIndex]);
							o_SubdivInfo.m_UVs.push_back(orig_uvs[new_vertex.m_UVIndex]);
						}
						else
						{
							//	reuse old vertex
							o_SubdivInfo.m_Indices.push_back( insert_result.first->m_NewIndex );
						}
					}
				}
			}
			else
			{
				o_SubdivInfo.m_Indices.swap( orig_geom_inds );
				o_SubdivInfo.m_Vertices.swap( orig_vertices );

				if( i_FillInVertexRemap )
				{
					// Simple remapping
					for (int v=0; v<o_SubdivInfo.m_Vertices.size(); v++)
						o_SubdivInfo.m_VertexRemap.insert(std::pair<int, int>(v, v));
				}
			}
		}
	}

	//----------------------------------------------------------------------------
	//	Write subdivision surface information to file.
	//----------------------------------------------------------------------------
	void WriteSUBD(	chWriter& o_Writer,
					const mdlSubdivInfo& i_SubdivInfo)
	{
		DBG_ASSERT(!i_SubdivInfo.m_Vertices.empty(), "Need to have some vertices to write subdiv info.");
		DBG_ASSERT(!i_SubdivInfo.m_Indices.empty(), "Need to have some indices to write subdiv info.");

		// 16-bit requirements removed now
		//DBG_ASSERT(i_SubdivInfo.m_Vertices.size()<0xFFFF, "Too many vertices in subdivision base mesh: %d", i_SubdivInfo.m_Vertices.size());
		//DBG_ASSERT(i_SubdivInfo.m_Indices.size()<0xFFFF, "Too many indices in subdivision base mesh: %d", i_SubdivInfo.m_Vertices.size());
		
		// Subdivision surface chunk
		// Raised to version 2 when changing to pre-sorted vertices 
		// and a single set of indices
		o_Writer.WriteChunkHeader(c_SUBD, 2, true);

		// Subdiv name
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		chChunkParserUtil::Write(o_Writer, i_SubdivInfo.m_Name);
		o_Writer.FinishChunk();

		// Write 32 bit number when writing number of verts for subdivs
		envType::UInt32 num, cur;

		// Vertices - version 2 is 32-bit
		o_Writer.WriteChunkHeader(c_GVER, 2, false);
		num = i_SubdivInfo.m_Vertices.size();
		o_Writer.Write(num);
		for( cur = 0 ; cur < num ; cur++ )
			chChunkParserUtil::Write(o_Writer, i_SubdivInfo.m_Vertices[cur]);
		o_Writer.FinishChunk();

		// Texture Coords - version 2 is 32-bit
		num = i_SubdivInfo.m_UVs.size();
		if (num > 0)
		{
			DBG_ASSERT(num == i_SubdivInfo.m_Vertices.size(), "Texture coords array must match vertex array length.");
			o_Writer.WriteChunkHeader(c_TVER, 2, false);
			o_Writer.Write(num);
			for( cur = 0 ; cur < num ; cur++ )
				chChunkParserUtil::Write(o_Writer, i_SubdivInfo.m_UVs[cur]);
			o_Writer.FinishChunk();
		}

		// Vertex remapping from Maya separate index arrays
		if (!i_SubdivInfo.m_VertexRemap.empty())
		{
			o_Writer.WriteChunkHeader(c_VRMP, 0, false);
			o_Writer.Write(envType::Int32(i_SubdivInfo.m_NumOrigVertices));
			mdlIndexUtil::WriteRemap(o_Writer, i_SubdivInfo.m_VertexRemap);
			o_Writer.FinishChunk();
		}

		// Face Indices - polygons, not triangles
		//	first number of vertices in face, then vertex indices.
		//	In version 1, the index array is already sorted like that, 
		// we just write it out as 32-bit indices.
		// Version 2 uses 32-bit for num faces and num indices.
		o_Writer.WriteChunkHeader(c_GFAC, 2, false);
		o_Writer.Write(envType::UInt32(i_SubdivInfo.m_NumFaces));
		num = i_SubdivInfo.m_Indices.size();
		o_Writer.Write(num);
		for( cur = 0 ; cur < num ; cur++ )
			chChunkParserUtil::Write(o_Writer, i_SubdivInfo.m_Indices[cur]);
		o_Writer.FinishChunk();

		// Materials - can be more than one. Write only the name
		// of the material, assuming that we will read a material table
		// from some other part of the file.
		// MaterialChanges array needs to be one fewer than number of materials.
		envType::Int16 num_mats = i_SubdivInfo.m_Materials.size();
		DBG_ASSERT(num_mats-1 == i_SubdivInfo.m_MaterialChanges.size(), "Incorrect number of material changes.");
		o_Writer.WriteChunkHeader(c_MTLS, 0, false);
		o_Writer.Write(num_mats);	
		for (int i=0; i<num_mats; i++)
		{	
			chChunkParserUtil::Write(o_Writer, i_SubdivInfo.m_Materials[i]->m_Info.GetMaterialName());
			if (i<num_mats-1)
			{
				o_Writer.Write(envType::Int32(i_SubdivInfo.m_MaterialChanges[i]));
			}
		}
		o_Writer.FinishChunk();

		// Old single-material format, used for backwards compatibility...
		// Material - just first material id written by name.
		if (!i_SubdivInfo.m_Materials.empty())
		{
			// version raised to "1" when adding MTLS chunk above that
			// replaces this chunk. Up-to-date parsers will read 
			// only the MTLS chunk.
			o_Writer.WriteChunkHeader(c_MTID, 1, false);
			//chChunkParserUtil::Write(o_Writer, i_SubdivInfo.m_Material->m_Info.GetMaterialName());
			chChunkParserUtil::Write(o_Writer, i_SubdivInfo.m_Materials[0]->m_Info.GetMaterialName());
			o_Writer.FinishChunk();
		}

		if (!i_SubdivInfo.m_VertexCreases.empty())
		{
			// Write vertex creasing information
			o_Writer.WriteChunkHeader(c_CRSV, 0, false);
			o_Writer.Write(envType::Int16(i_SubdivInfo.m_MaxVertexCreaseLevel));
			const int nvi = i_SubdivInfo.m_VertexCreases.size();
			o_Writer.Write(envType::Int16(nvi));
			for (int i=0; i<nvi; i++)
			{
				const mayCreaseInfo &crease = i_SubdivInfo.m_VertexCreases[i];
				// Some of these items need many bits, some just a few
				o_Writer.Write(envType::Int32(crease.m_Base));
				o_Writer.Write(envType::Int16(crease.m_First));
				o_Writer.Write(envType::Int16(crease.m_Level));
				o_Writer.Write(envType::Int32(crease.m_Path));
				o_Writer.Write(envType::Int32(crease.m_Corner));
			}
			o_Writer.FinishChunk();
		}

		if (!i_SubdivInfo.m_EdgeCreases.empty())
		{
			// Write vertex creasing information
			o_Writer.WriteChunkHeader(c_CRSE, 0, false);
			o_Writer.Write(envType::Int16(i_SubdivInfo.m_MaxEdgeCreaseLevel));
			const int nvi = i_SubdivInfo.m_EdgeCreases.size();
			o_Writer.Write(envType::Int16(nvi));
			for (int i=0; i<nvi; i++)
			{
				const mayCreaseInfo &crease = i_SubdivInfo.m_EdgeCreases[i];
				// Some of these items need many bits, some just a few
				o_Writer.Write(envType::Int32(crease.m_Base));
				o_Writer.Write(envType::Int16(crease.m_First));
				o_Writer.Write(envType::Int16(crease.m_Level));
				o_Writer.Write(envType::Int32(crease.m_Path));
				o_Writer.Write(envType::Int32(crease.m_Corner));
			}
			o_Writer.FinishChunk();
		}


		// Surface flags
		// Note: need to increment version when adding flags
		// Version 2 is last version used in old Maya exporter
		// Version 3 adds m_bAutoGenLowRes
		o_Writer.WriteChunkHeader(c_FLAG, 3, false);
		o_Writer.Write(envType::Int8(i_SubdivInfo.m_Flags.m_bDoubleSided));
		o_Writer.Write(envType::Int8(i_SubdivInfo.m_Flags.m_bTriangleSort));
		o_Writer.Write(envType::Int8(i_SubdivInfo.m_Flags.m_bCastsShadow));
		o_Writer.Write(envType::Int8(i_SubdivInfo.m_Flags.m_bReceivesShadow));
		o_Writer.Write(envType::Int8(i_SubdivInfo.m_Flags.m_bShadowHull));
		o_Writer.Write(envType::Int8(i_SubdivInfo.m_Flags.m_bAutoGenLowRes));
		o_Writer.FinishChunk(); // c_FLAG

		o_Writer.FinishChunk(); // c_GFRG
	}
}

