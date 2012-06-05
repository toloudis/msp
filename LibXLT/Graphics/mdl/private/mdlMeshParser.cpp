/****************************************************************************\
**	mdlMeshParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlMeshParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
#include "Graphics/mdl/private/mdlMaterialParser.hpp"
#include "Graphics/mdl/private/mdlMeshMayaParser.hpp"

#include <algorithm>
#include <iterator>
#include <set>
#include <string>


//============================================================================
//	Any of these mdlMeshParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlMeshParser
{
	namespace
	{
		const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
		const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
		const chDefs::Name c_TVER = chDefs::MakeName('T', 'V', 'E', 'R');
		const chDefs::Name c_BVEC = chDefs::MakeName('B', 'V', 'E', 'C');
		const chDefs::Name c_VCOL = chDefs::MakeName('V', 'C', 'O', 'L');
		const chDefs::Name c_VRMP = chDefs::MakeName('V', 'R', 'M', 'P');
		const chDefs::Name c_NRMP = chDefs::MakeName('N', 'R', 'M', 'P');
		const chDefs::Name c_GIND = chDefs::MakeName('G', 'I', 'N', 'D');
		const chDefs::Name c_MTLS = chDefs::MakeName('M', 'T', 'L', 'S');
		const chDefs::Name c_FLAG = chDefs::MakeName('F', 'L', 'A', 'G');
		const chDefs::Name c_GNDP = chDefs::MakeName('G', 'N', 'D', 'P');
		const chDefs::Name c_FPRT = chDefs::MakeName('F', 'P', 'R', 'T');
		bool l_bSkipLowRes = false;
		bool l_bSkipHighRes = false;
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
					bool i_FillInVertexRemap,
					mdlMatInfoTable *i_MaterialTable)
	{
		// Versions 1 and below use an older format exported directly from
		// the Maya plugin. That code is handled in a separate namespace
		if (i_Version <= 1)
		{
			return mdlMeshMayaParser::ReadGFRG(i_Reader, i_Version, i_Size, o_FragInfo, 
				i_FillInVertexRemap, l_bSkipLowRes, l_bSkipHighRes, i_MaterialTable);
		}


		if ( mdlDefs::GetVerboseMode() )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_TEXT("");
			DBG_TEXT("mdlMeshParser::ReadGFRG loading from %s" << filename.c_str());
		}

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// With version 1 of GFRG, the size of the indices in bytes
		// is written to each index chunk. And the "number of"
		// values are now 32-bit integers.
		bool bNeedIndexSize = (i_Version >= 1);

		bool did_read_GVER = false, did_read_NVER = false, did_read_GIND = false;
		bool has_texture_indices = false;
		bool did_read_VRMP = false, did_read_NRMP = false;

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if ( name == c_NNAM )
				{
					i_Reader.Read(o_FragInfo.m_Name);
					//DBG_LOG(" Read mesh named: " << o_FragInfo.m_Name.c_str());
				}
				else if ( name == c_GVER )
				{
					did_read_GVER = true;
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					chChunkParserUtil::ReadArray(i_Reader, o_FragInfo.m_Vertices, num);
				}
				else if ( name == c_NVER )
				{
					did_read_NVER = true;
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					chChunkParserUtil::ReadArray(i_Reader, o_FragInfo.m_Normals, num);
				}
				else if ( name == c_TVER )
				{
					envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					chChunkParserUtil::ReadArray(i_Reader, o_FragInfo.m_UVs, num);
				}
				else if ( name == c_BVEC )
				{
					// basis vectors
					envType::UInt32 num;
					i_Reader.Read(num);
					chChunkParserUtil::ReadArray(i_Reader, o_FragInfo.m_Ss, num);
					chChunkParserUtil::ReadArray(i_Reader, o_FragInfo.m_Ts, num);
				}
				//else if ( name == c_VCOL )
				//{
				//	// vertex colors, RGB (0-1)
				//	envType::UInt32 num = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
				//	o_FragInfo.m_Colors.resize(num);
				//	envType::UInt32 cur;
				//	maVector3d colorRGB;
				//	for ( cur = 0 ; cur < num ; cur++ )
				//	{
				//		chChunkParserUtil::Read(i_Reader, colorRGB);
				//		//DBG_LOG3("Read color %f %f %f", colorRGB.m_X, colorRGB.m_Y, colorRGB.m_Z);
				//		o_FragInfo.m_Colors[cur].Set(colorRGB.m_X, colorRGB.m_Y, colorRGB.m_Z, 1.0f);
				//	}
				//}
				else if ( name == c_VRMP )
				{
					did_read_VRMP = true;
					envType::UInt32 num_orig;
					i_Reader.Read(num_orig);
					o_FragInfo.m_NumOrigVertices = num_orig;
					mdlIndexUtil::ReadRemap(i_Reader, o_FragInfo.m_VertexRemap);
				}
				else if ( name == c_NRMP )
				{
					did_read_NRMP = true;
					envType::UInt32 num_orig;
					i_Reader.Read(num_orig);
					o_FragInfo.m_NumOrigNormals = num_orig;
					mdlIndexUtil::ReadRemap(i_Reader, o_FragInfo.m_NormalRemap);
				}
				else if ( name == c_MTLS)
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
								o_FragInfo.m_Materials.push_back(it->second);
							else
							{
								std::string filename;
								fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
								DBG_LOG("mdlImport::read_GFRG(): Did not find material id '" << material_name.c_str() << "' in table in " << filename.c_str());
								throw mdlInvalidModelFileX(i_Reader.GetLocator());
							}
						}
						else
						{
							std::string filename;
							fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
							DBG_LOG("mdlImport::read_GFRG(): Using material ids (" << material_name.c_str() << "), but no material table given in " << filename.c_str());
							throw mdlInvalidModelFileX(i_Reader.GetLocator());
						}

						// Read material changes (index when next material should be used).			
						if (i<num_mats-1)
						{
							envType::Int32 ind;
							i_Reader.Read(ind);
							o_FragInfo.m_MaterialChanges.push_back(ind);
						}
					}
				}
				else if ( name == c_GIND )
				{
					did_read_GIND = true;
					envType::UInt32 num_indices = mdlIndexUtil::ReadNumber(i_Reader, (version >= 1));
					const bool b32Bit = true;
					o_FragInfo.m_Indices.resize(num_indices);
					mdlIndexUtil::ReadIndexSet(i_Reader, o_FragInfo.m_Indices, num_indices, b32Bit);
				} else if ( name == c_FPRT )
				{
					envType::UInt32 num_parts;
					i_Reader.Read(num_parts);
					std::string part_name;
					for ( int i=0; i < num_parts; ++i )
					{
						i_Reader.Read( part_name );
					}
					std::vector< envType::UInt32 > beginIndices;					
					const bool b32Bit = true;
					beginIndices.resize(num_parts);
					mdlIndexUtil::ReadIndexSet(i_Reader, beginIndices, num_parts, b32Bit);
				} else if ( name == c_FLAG )
				{
					// fragment flags
					envType::Int8 flag_val;

					i_Reader.Read(flag_val);
					o_FragInfo.m_Flags.m_bDoubleSided = (flag_val != 0);
					i_Reader.Read(flag_val);
					o_FragInfo.m_Flags.m_bTriangleSort = (flag_val != 0);
					i_Reader.Read(flag_val);
					o_FragInfo.m_Flags.m_bCastsShadow = (flag_val != 0);
					i_Reader.Read(flag_val);
					o_FragInfo.m_Flags.m_bReceivesShadow = (flag_val != 0);
					i_Reader.Read(flag_val);
					o_FragInfo.m_Flags.m_bShadowHull = (flag_val != 0);

					// Version 5 makes resolution a 3-way state
					//  0 - appear in all resolutions
					//	1 - appear only in low resolutions
					//	2 - appear only in high resolutions
					i_Reader.Read(flag_val);
					o_FragInfo.m_ResolutionLevel = flag_val;

					i_Reader.Read(flag_val);
					o_FragInfo.m_Flags.m_bVertexAnimation = (flag_val != 0);
				}

				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;

			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_LOG("mdlImport::read_GFRG(): Invalid chunk in " << filename.c_str());
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

		DBG_ASSERT(did_read_GVER, "Didn't find required GVER chunk in mdlImport::read_GFRG");
		DBG_ASSERT(did_read_NVER, "Didn't find required NVER chunk in mdlImport::read_GFRG");
		DBG_ASSERT(did_read_GIND, "Didn't find required GIND chunk in mdlImport::read_GFRG");

		//if ( mdlDefs::GetVerboseMode() )
		//{
		//	dbgLog::Write("total original position vertices: %d", o_FragInfo.m_Vertices.size());
		//	dbgLog::Write("total original normal vertices: %d", o_FragInfo.m_Normals.size());
		//	dbgLog::Write("total original texture vertices: %d", o_FragInfo.m_UVs.size());
		//}
		

		// If we did not read in a vertex remapping, then set the number
		// of original vertices to the current number in the fragment structure.
		if (!did_read_VRMP)
			o_FragInfo.m_NumOrigVertices = o_FragInfo.m_Vertices.size();
		if (!did_read_NRMP)
			o_FragInfo.m_NumOrigNormals = o_FragInfo.m_Normals.size();

		// We could do this compression in the Maya exporter and then
		// avoid it here when the version number confirms that it has
		// been done in Maya already.
		//const bool c_DoCompression = false;
		//if (c_DoCompression && !i_FillInVertexRemap)
		// Do compression only on low resolution models
		//if ((o_FragInfo.m_ResolutionLevel == 1) && !i_FillInVertexRemap)
		//{
		//	// Look for duplicates in array in order to reduce the
		//	// number of expanded vertices
		//	mdlIndexUtil::CompressIndices<maVector3d>(o_FragInfo.m_Vertices, gindices);
		//	mdlIndexUtil::CompressIndices<maVector3d>(o_FragInfo.m_Normals, nindices);
		//	if ( has_texture_indices )
		//		mdlIndexUtil::CompressIndices<maPoint2d>(o_FragInfo.m_UVs, tindices);
		//}

		// Skip fragment only works when the resolution was set
		// from the FLAG structure.
		bool bSkipFragment = ( (l_bSkipLowRes && (o_FragInfo.m_ResolutionLevel == 1)) 
			|| (l_bSkipHighRes && (o_FragInfo.m_ResolutionLevel == 2)) );
		return (!bSkipFragment);
	}

	//----------------------------------------------------------------------------
	//	Read Node Proxy
	//----------------------------------------------------------------------------
	bool ReadGNDP(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlNodeInfoProxy& o_NodeInfoProxy )
	{
		if ( mdlDefs::GetVerboseMode() )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_TEXT("");
			DBG_TEXT("mdlMeshParser::ReadGNDP loading from %s" << filename.c_str());
		}
		envType::UInt32 num;
		i_Reader.Read( num );
		mdlPathReference::TPath & path = o_NodeInfoProxy.m_Path;
		std::back_insert_iterator< mdlPathReference::TPath > bit( path );
		for ( int i=0; i < num; ++i)
		{
			std::string pathComponent;
			i_Reader.Read( pathComponent );
			*bit++ = pathComponent;
		}
		DBG_ASSERT( num > 0, "Found an empty path while reading mdlImport::read_GNDP");
		return true;
	}

	//----------------------------------------------------------------------------
	//	Write geometry fragment to file.
	//----------------------------------------------------------------------------
	void WriteGFRG(	chWriter& o_Writer,
					const mdlFragInfo& i_FragInfo)
	{
		DBG_ASSERT(!i_FragInfo.m_Vertices.empty(), "Need to have some vertices to write mesh info.");
		DBG_ASSERT(!i_FragInfo.m_Indices.empty(), "Need to have some indices to write mesh info.");

		// Geometry fragment chunk
		// Raised to version 2 when changing to pre-sorted vertices 
		// and a single set of indices
		o_Writer.WriteChunkHeader(c_GFRG, 2, true);

		// Mesh name
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Name);
		o_Writer.FinishChunk();

		// Write 32 bit number when writing number of verts
		envType::UInt32 num, cur;

		// Vertices - version 1 signifies 32-bit num verts
		o_Writer.WriteChunkHeader(c_GVER, 1, false);
		num = i_FragInfo.m_Vertices.size();
		o_Writer.Write(num);
		for ( cur = 0 ; cur < num ; cur++ )
			chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Vertices[cur]);
		o_Writer.FinishChunk();

		// Normals - version 1 signifies 32-bit num verts
		num = i_FragInfo.m_Normals.size();
		if (num > 0)
		{
			DBG_ASSERT(num == i_FragInfo.m_Vertices.size(), "Normals array must match vertex array length.");
			o_Writer.WriteChunkHeader(c_NVER, 1, false);
			o_Writer.Write(num);
			for ( cur = 0 ; cur < num ; cur++ )
				chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Normals[cur]);
			o_Writer.FinishChunk();
		}

		// Texture Coords - version 1 signifies 32-bit num verts
		num = i_FragInfo.m_UVs.size();
		if (num > 0)
		{
			DBG_ASSERT(num == i_FragInfo.m_Vertices.size(), "Texture coords array must match vertex array length.");
			o_Writer.WriteChunkHeader(c_TVER, 1, false);
			o_Writer.Write(num);
			for ( cur = 0 ; cur < num ; cur++ )
				chChunkParserUtil::Write(o_Writer, i_FragInfo.m_UVs[cur]);
			o_Writer.FinishChunk();
		}

		// Vertex colors - version 1 signifies 32-bit num verts
		//num = i_FragInfo.m_Colors.size();
		//if (num > 0)
		//{
		//	DBG_ASSERT(num == i_FragInfo.m_Vertices.size(), "Vertex color array must match vertex array length.");
		//	o_Writer.WriteChunkHeader(c_VCOL, 1, false);
		//	o_Writer.Write(num);
		//	for ( cur = 0 ; cur < num ; cur++ )
		//		chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Colors[cur]);
		//	o_Writer.FinishChunk();
		//}

		// Basis Vectors - version 1 signifies 32-bit num verts
		num = i_FragInfo.m_Ss.size();
		DBG_ASSERT(num == i_FragInfo.m_Ts.size(), "Basis vector array sizes must match.");
		if (num > 0)
		{
			DBG_ASSERT(num == i_FragInfo.m_Vertices.size(), "Basis vector array must match vertex array length.");
			o_Writer.WriteChunkHeader(c_BVEC, 1, false);
			o_Writer.Write(num);
			for ( cur = 0 ; cur < num ; cur++ )
				chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Ss[cur]);
			for ( cur = 0 ; cur < num ; cur++ )
				chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Ts[cur]);
			o_Writer.FinishChunk();
		}

		// Vertex remapping from Maya separate index arrays
		if (!i_FragInfo.m_VertexRemap.empty())
		{
			o_Writer.WriteChunkHeader(c_VRMP, 0, false);
			o_Writer.Write(envType::Int32(i_FragInfo.m_NumOrigVertices));
			mdlIndexUtil::WriteRemap(o_Writer, i_FragInfo.m_VertexRemap);
			o_Writer.FinishChunk();
		}

		// Normal remapping from Maya separate index arrays
		if (!i_FragInfo.m_NormalRemap.empty())
		{
			o_Writer.WriteChunkHeader(c_NRMP, 0, false);
			o_Writer.Write(envType::Int32(i_FragInfo.m_NumOrigNormals));
			mdlIndexUtil::WriteRemap(o_Writer, i_FragInfo.m_NormalRemap);
			o_Writer.FinishChunk();
		}

		// Indices - version 1 signifies 32-bit num verts
		o_Writer.WriteChunkHeader(c_GIND, 1, false);
		num = i_FragInfo.m_Indices.size();
		o_Writer.Write(num);
		for ( cur = 0 ; cur < num ; cur++ )
			chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Indices[cur]);
		o_Writer.FinishChunk();

		// Materials - can be more than one. Write only the name
		// of the material, assuming that we will read a material table
		// from some other part of the file.
		// MaterialChanges array needs to be one fewer than number of materials.
		envType::Int16 num_mats = i_FragInfo.m_Materials.size();
		DBG_ASSERT(num_mats-1 == i_FragInfo.m_MaterialChanges.size(), "Incorrect number of material changes.");
		o_Writer.WriteChunkHeader(c_MTLS, 0, false);
		o_Writer.Write(num_mats);	
		for (int i=0; i<num_mats; i++)
		{	
			chChunkParserUtil::Write(o_Writer, i_FragInfo.m_Materials[i]->m_Info.GetMaterialName());
			if (i<num_mats-1)
			{
				o_Writer.Write(envType::Int32(i_FragInfo.m_MaterialChanges[i]));
			}
		}
		o_Writer.FinishChunk();
		//If this fragment was a merged fragment that contains parts from
		//various other fragments, then please write the parts info also.
		if ( i_FragInfo.m_Parts.size() > 0 )
		{	
			num = i_FragInfo.m_Parts.size();
			o_Writer.WriteChunkHeader(c_FPRT, 0, false);
			o_Writer.Write(num);
			std::vector< mdlFragInfo::PartComponent >::const_iterator pit;
			for ( pit = i_FragInfo.m_Parts.begin(); pit != i_FragInfo.m_Parts.end(); ++pit )
			{
				const mdlFragInfo::PartComponent &part = *pit;
				chChunkParserUtil::Write(o_Writer, part.m_Name );
			}
			for ( pit = i_FragInfo.m_Parts.begin(); pit != i_FragInfo.m_Parts.end(); ++pit )
			{
				const mdlFragInfo::PartComponent &part = *pit;
				chChunkParserUtil::Write(o_Writer, part.m_PartBeginIndex );
			}
			o_Writer.FinishChunk();
		}
		// Fragment flags
		// Note: need to increment version when adding flags
		// Version 5 is last version used in old Maya exporter
		o_Writer.WriteChunkHeader(c_FLAG, 5, false);
		o_Writer.Write(envType::Int8(i_FragInfo.m_Flags.m_bDoubleSided));
		o_Writer.Write(envType::Int8(i_FragInfo.m_Flags.m_bTriangleSort));
		o_Writer.Write(envType::Int8(i_FragInfo.m_Flags.m_bCastsShadow));
		o_Writer.Write(envType::Int8(i_FragInfo.m_Flags.m_bReceivesShadow));
		o_Writer.Write(envType::Int8(i_FragInfo.m_Flags.m_bShadowHull));
		// "low-resolution" flag is 3-way state
		//  0 - appear in all resolutions
		//	1 - appear only in low resolutions
		//	2 - appear only in high resolutions
		o_Writer.Write(envType::Int8(i_FragInfo.m_ResolutionLevel));
		o_Writer.Write(envType::Int8(i_FragInfo.m_Flags.m_bVertexAnimation));
		o_Writer.FinishChunk(); // c_FLAG
	
		o_Writer.FinishChunk(); // c_GFRG
	}

	//----------------------------------------------------------------------------
	//	Write node proxy to file.
	//----------------------------------------------------------------------------
	void WriteGNDP(	chWriter& o_Writer,
					const mdlNodeInfoProxy& i_NodeInfoProxy )
	{
		DBG_ASSERT( ( !i_NodeInfoProxy.m_Path.empty() ), "Need to have a non-empty path");

		// Geometry fragment chunk
		// Raised to version 2 when changing to pre-sorted vertices 
		// and a single set of indices
		o_Writer.WriteChunkHeader(c_GNDP, 0, true);	
		typedef mdlPathReference::TPath TPath;
		const TPath &path = i_NodeInfoProxy.m_Path;
		TPath::const_iterator pit;
		envType::UInt32 num = static_cast< envType::UInt32> ( path.size() );
		o_Writer.Write( num );
		for ( pit = path.begin();  pit != path.end(); ++pit )
		{
			chChunkParserUtil::Write( o_Writer, *pit );
		}
		o_Writer.FinishChunk(); // c_GNDP
	}
}

