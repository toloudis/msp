/****************************************************************************\
**	vtxVertexAnimImport.cpp
**
**		vtxVertexAnimImport.hpp supplies functions used to import 
**	animation files.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexAnimImport.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/vtx/private/vtxZLibCompress.hpp"
#include "Graphics/vtx/vtxDeltaLosslessEncoder.hpp"
#include "Graphics/vtx/vtxDeltaToleranceEncoder.hpp"
#include "Graphics/vtx/vtxExceptionX.hpp"
#include "Graphics/vtx/vtxNormal16BitEncoder.hpp"
#include "Graphics/vtx/vtxNormalLosslessEncoder.hpp"
#include "Graphics/vtx/vtxVertexAnimKeysCompressed.hpp"
#include "Graphics/vtx/vtxVertexAnimKeysDynamic.hpp"
#include "Graphics/vtx/vtxVertexAnimKeysStatic.hpp"


//============================================================================
//============================================================================
namespace vtxVertexAnimImport
{
	namespace
	{
		//------------------------------------------------------------------------
		// Chunk names
		//------------------------------------------------------------------------
		const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
		const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
		//const chDefs::Name c_NCMP = chDefs::MakeName('N', 'C', 'M', 'P');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');

		const chDefs::Name c_VTXA = chDefs::MakeName('V', 'T', 'X', 'A');
		const chDefs::Name c_FRAM = chDefs::MakeName('F', 'R', 'A', 'M');
		const chDefs::Name c_TIME = chDefs::MakeName('T', 'I', 'M', 'E');
		const chDefs::Name c_BBOX = chDefs::MakeName('B', 'B', 'O', 'X');

		const chDefs::Name c_VDLT = chDefs::MakeName('V', 'D', 'L', 'T');
		const chDefs::Name c_VCFG = chDefs::MakeName('V', 'C', 'F', 'G');
		const chDefs::Name c_CFRS = chDefs::MakeName('C', 'F', 'R', 'S');
		const chDefs::Name c_CFRM = chDefs::MakeName('C', 'F', 'R', 'M');
		const chDefs::Name c_FRDT = chDefs::MakeName('F', 'R', 'D', 'T');


		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void read_compressed_frames(chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								fsFileStream::FilePosType i_GeometryCacheOffset,
								vtxVertexAnimKeysCompressed& io_VertexAnimKeys, 
								shared_ptr<vtxDynamicVertexSet>& io_VertexSet, 
								shared_ptr<vtxCompressedDeltasSet>& io_DeltasSet)
		{
			// Reference frames and delta blocks are referenced by file position
			// and can be used more than once. So, track a map of which frames
			// and delta blocks have already been read in.
			std::map<fsFileStream::FilePosType, int> ref_frame_map;
			std::map<fsFileStream::FilePosType, int> deltas_map;
			std::map<fsFileStream::FilePosType, int>::iterator map_it;

			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;
			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if ( name == c_CFRM )
					{
						envType::Float32 time = 0;
						i_Reader.Read(time);

						vtxVertexAnimKeysCompressed::sFrameData frame_data;

						envType::Int64 ref_frame_pos = 0;
						i_Reader.Read(ref_frame_pos);

						envType::Int64 deltas_pos = -1;
						i_Reader.Read(deltas_pos);

						envType::Int64 normals_pos = -1;
						i_Reader.Read(normals_pos);

						i_Reader.Read(frame_data.m_DeltaOffset);

						maVector3d min_pt, max_pt;
						chChunkParserUtil::Read(i_Reader, min_pt);
						chChunkParserUtil::Read(i_Reader, max_pt);
						frame_data.m_BBox.Set(min_pt, max_pt);

						chChunkParserUtil::Read(i_Reader, frame_data.m_bStatic);

						//DBG_LOG("Read compressed frame at time " << time << " ref frame " << ref_frame_pos << " deltas " << deltas_pos << " delta offset " << frame_data.m_DeltaOffset);

						// Map refrence frame from file cache position into frame index
						map_it = ref_frame_map.find(ref_frame_pos);
						if (map_it == ref_frame_map.end())
						{
							// New reference frame location, offset to position within geometry cache
							frame_data.m_ReferenceFrame = io_VertexSet->AddFilePos(ref_frame_pos + i_GeometryCacheOffset);
							ref_frame_map[ref_frame_pos] = frame_data.m_ReferenceFrame;
						}
						else
						{
							// Use existing index
							frame_data.m_ReferenceFrame = map_it->second;
						}

						// Map delta block from file cache position into frame index
						if (deltas_pos < 0)
						{
							frame_data.m_DeltaBlock = -1; // reference frames are marked with negative delta block index
						}
						else
						{
							map_it = deltas_map.find(deltas_pos);
							if (map_it == deltas_map.end())
							{
								// New deltas block location, offset to position within geometry cache,
								// also give it the index of the reference frame in order to expand out the deltas
								envType::Int64 normals_offset = (normals_pos<0) ? -1 : (normals_pos + i_GeometryCacheOffset);
								frame_data.m_DeltaBlock = io_DeltasSet->AddFilePos(deltas_pos + i_GeometryCacheOffset, normals_offset, frame_data.m_ReferenceFrame);
								deltas_map[deltas_pos] = frame_data.m_DeltaBlock;
							}
							else
							{
								// Use existing index
								frame_data.m_DeltaBlock = map_it->second;
							}
						}
						
						io_VertexAnimKeys.Frames().AddKey(time, frame_data);
					}
					
					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in vtxVertexAnimImport::ReadVertexAnimation");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}

		//------------------------------------------------------------------------
		// Read a single frame of vertex animation 
		//	(state of mesh at certain time).
		// Can pass in NULL for o_pVertexFrame in order to skip
		// actual load of animation data.
		//------------------------------------------------------------------------
		void ReadVertexFrame( chReader& i_Reader,
							  float &o_Time,
							  vtxVertexFrame* o_pVertexFrame = NULL,
							  maAxisBox* o_pBBox = NULL)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;
			try
			{
				while ( i_Reader.ReadChunkHeader(name, version, size) )
				{
					if ( name == c_TIME )
					{
						i_Reader.Read(o_Time);
						//DBG_LOG("Reading vertex frame for time: " << o_Time);
					}
					else if (o_pVertexFrame) // Only read frame data if pointer is non-null
					{
						if ( name == c_GVER )
						{
							envType::Int32 nVerts;
							i_Reader.Read(nVerts);
							chChunkParserUtil::ReadArray(i_Reader, o_pVertexFrame->m_Positions, nVerts);
						}
						else if ( name == c_NVER )
						{
							envType::Int32 nNorms;
							i_Reader.Read(nNorms);
							chChunkParserUtil::ReadArray(i_Reader, o_pVertexFrame->m_Normals, nNorms);
						}
						//else if ( name == c_NCMP )
						//{
						//bga - took this out because it lost the sign of the z component, 
						// but could be handled in vertex shader...
						//	// NCMP has only two components per normal
						//	envType::Int32 nNorms;
						//	i_Reader.Read(nNorms);
						//	o_pVertexFrame->m_Normals.resize(nNorms);
						//	for (int i=0; i<nNorms; ++i)
						//	{
						//		maVector3d &normal = o_pVertexFrame->m_Normals[i];
						//		i_Reader.Read(normal.m_X);
						//		i_Reader.Read(normal.m_Y);
						//		// compute z value so that normal is unit-length
						//		normal.m_Z = ::sqrtf(1.0f - (normal.m_X*normal.m_X + normal.m_Y*normal.m_Y));
						//	}
						//}
					}
					if (o_pBBox) // Only read bounding box data if pointer is non-null
					{
						if (name == c_BBOX )
						{
							maVector3d min_pt, max_pt;
							chChunkParserUtil::Read(i_Reader, min_pt);
							chChunkParserUtil::Read(i_Reader, max_pt);
							o_pBBox->Set(min_pt, max_pt);
						}
					}
					
					i_Reader.FinishChunk();
				}
			}
			catch ( const chInvalidChunkX& i_Ex )
			{
				i_Ex;
				DBG_LOG("Invalid chunk in vtxVertexAnimImport::read_vertex_frame");
				throw mdlInvalidModelFileX(i_Reader.GetLocator());
			}
		}
	}	// end of local namespace


	//------------------------------------------------------------------------
	// Read compressed deltas into the given output buffer,
	// Returns amount of data read in o_SizeRead.
	//------------------------------------------------------------------------
	void ReadDeltas( chReader& i_Reader,
					 void* o_OutputData, 
					 int i_OutputBufferSize,
					 int &o_SizeRead)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		o_SizeRead = 0;

		try
		{
			// Deltas are a single chunk? 
			// Or should we have positional and normal delta blocks?
			i_Reader.ReadChunkHeader(name, version, size);
			
			if ( name == c_VDLT )
			{
				o_SizeRead = vtxZLibCompress::ReadCompressed(i_Reader, size, o_OutputData, i_OutputBufferSize);
			}		

			i_Reader.FinishChunk();
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in vtxVertexAnimImport::ReadDeltas");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}
	}

	//------------------------------------------------------------------------
	// Read a single frame of vertex animation data
	//	(state of mesh at certain time). Does not read time or bounding box.
	// This is intended for dynamic paging of the animation data and the 
	// file pointer should be at the beginning of the FRAM or FRDT chunk.
	//------------------------------------------------------------------------
	void ReadSingleVertexFrameData( chReader& i_Reader,
									vtxVertexFrame& o_VertexFrame)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		try
		{
			// Deltas are a single chunk? 
			// Or should we have positional and normal delta blocks?
			i_Reader.ReadChunkHeader(name, version, size);
			
			if ( ( name == c_FRAM ) || ( name == c_FRDT ))
			{		
				float time = 0; // ignoring time
				ReadVertexFrame(i_Reader, time, &o_VertexFrame);
			}		
			i_Reader.FinishChunk();
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in vtxVertexAnimImport::ReadSingleVertexFrameData");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

	}

	//------------------------------------------------------------------------
	// Read in vertex animation for a single surface within an already
	//	opened file. This represents the VTXA chunk.
	// Read the data into a static set of vertex anim keys (loads all data).
	//------------------------------------------------------------------------
	void ReadStaticVertexAnimation( chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								std::string& o_SurfaceName,
								shared_ptr<vtxVertexAnimKeys>& o_VertexAnim,
								shared_ptr<vtxVertexFrames>& o_VertexFrames)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// Make sure we have the correct storage type for the frames
		vtxVertexFramesStatic *pVertexFrames = NULL;
		if (!o_VertexFrames)
		{
			pVertexFrames = new vtxVertexFramesStatic();
			o_VertexFrames.reset(pVertexFrames);
		}
		else
			pVertexFrames = dynamic_cast<vtxVertexFramesStatic*>(o_VertexFrames.get());
		if (!pVertexFrames)
		{
			DBG_ERROR("Incorrect mix of vertex animation types within one file.");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}


		vtxVertexAnimKeysStatic *pVertexAnimKeys = new vtxVertexAnimKeysStatic();
		o_VertexAnim.reset(pVertexAnimKeys);

		try
		{
			while ( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if ( name == c_NNAM )
				{
					i_Reader.Read(o_SurfaceName);
					//DBG_LOG("Reading vertex animation for mesh: " << o_SurfaceName.c_str());
				}
				else if ( name == c_FRAM )
				{
					vtxVertexFrame *pVertexFrame = new vtxVertexFrame;
					float time = 0;
					ReadVertexFrame(i_Reader, time, pVertexFrame);
					DBG_ASSERT(!pVertexFrame->m_Positions.empty(), "Did not read position data for vertex animation frame.");

					pVertexFrames->AddVertexFrame(pVertexFrame);
					pVertexAnimKeys->Frames().AddKey(time, pVertexFrame);
				}
				
				i_Reader.FinishChunk();
			}
			
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in vtxVertexAnimImport::ReadVertexAnimation");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

	}

	//------------------------------------------------------------------------
	// Read in vertex animation for a single surface within an already
	//	opened file. This represents the VTXA chunk.
	// Read the data into a dynamic set of vertex anim keys (can page data).
	//------------------------------------------------------------------------
	void ReadDynamicVertexAnimation( chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								std::string& o_SurfaceName,
								shared_ptr<vtxVertexAnimKeys>& o_VertexAnim,
								shared_ptr<vtxVertexFrames>& o_VertexFrames)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// Make sure we have the correct storage type for the frames
		vtxVertexFramesDynamic *pVertexFrames = NULL;
		if (!o_VertexFrames)
		{
			pVertexFrames = new vtxVertexFramesDynamic();
			o_VertexFrames.reset(pVertexFrames);
		}
		else
			pVertexFrames = dynamic_cast<vtxVertexFramesDynamic*>(o_VertexFrames.get());
		if (!pVertexFrames)
		{
			DBG_ERROR("Incorrect mix of vertex animation types within one file.");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

		// Begin a vertex set here
		shared_ptr<vtxDynamicVertexSet> vertex_set = pVertexFrames->CreateVertexSet(i_Reader.GetLocator(), i_Size);
		//DBG_WARNING("New Vertex Set, size: " << i_Size << " fully loaded: " << vertex_set->IsFullyLoaded());
		vtxVertexAnimKeysDynamic *pVertexAnimKeys = new vtxVertexAnimKeysDynamic(vertex_set);
		o_VertexAnim.reset(pVertexAnimKeys);

		try
		{
			// Get file position at start of chunk, not inside of chunk
			fsFileStream::FilePosType file_pos = i_Reader.GetFilePos();

			while ( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if ( name == c_NNAM )
				{
					i_Reader.Read(o_SurfaceName);
					//DBG_LOG("Reading vertex animation for mesh: " << o_SurfaceName.c_str());
				}
				else if ( name == c_FRAM )
				{
					// moved file position pointer to beginning of FRAM chunk above
					//fsFileStream::FilePosType file_pos = i_Reader.GetFilePos();
					int frame_index = vertex_set->AddFilePos(file_pos);
					//if (frame_index == 0)
					//	DBG_WARNING("Vertex frame, size: " << size);

					float time = 0;
					maAxisBox bbox;
					if (vertex_set->IsFullyLoaded())
					{
						// Read full frame data and give it to vertex set
						std::unique_ptr<vtxVertexFrame> pVertexFrame(new vtxVertexFrame);
						ReadVertexFrame(i_Reader, time, pVertexFrame.get(), &bbox);
						vertex_set->AddVertexFrame(pVertexFrame.release());	
					}
					else if (frame_index == 0)
					{
						// Always read the first frame, even when paging the
						// vertex animation. This gives the information about
						// number of vertices, etc. to the vertex set.
						// Read vertex info directly into frame buffer 
						// of vertex set.
						ReadVertexFrame(i_Reader, time, vertex_set->FirstFrame(), &bbox);
						vertex_set->AllocateSecondFrame();
					}
					else
					{
						// Read just the time value from the chunk
						ReadVertexFrame(i_Reader, time, NULL, &bbox);
					}
					pVertexAnimKeys->Frames().AddKey(time, frame_index);

					// If we read a meaningful bounding box, then use that
					// in the bbox animation. This allows us to judge the size of the
					// surface before it is animated. This could be used for
					// attachment or for culling.
					if (!bbox.IsEmpty())
						pVertexAnimKeys->BBoxAnim().AddKey(time, bbox);
				}
				
				i_Reader.FinishChunk();
				 file_pos = i_Reader.GetFilePos(); // set file pointer to beginning of next chunk
			}
			
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in vtxVertexAnimImport::ReadVertexAnimation");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

	}

	//------------------------------------------------------------------------
	// Read in compressed vertex animation for a single surface within an 
	//	already	opened file. This represents the VTXC chunk.
	//------------------------------------------------------------------------
	void ReadCompressedVertexAnimation( chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										fsFileStream::FilePosType i_GeometryCacheOffset,
										std::string& o_SurfaceName,
										shared_ptr<vtxVertexAnimKeys>& o_VertexAnim,
										shared_ptr<vtxVertexFrames>& o_VertexFrames)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		// Make sure we have the correct storage type for the frames
		vtxVertexFramesCompressed *pVertexFrames = NULL;
		if (!o_VertexFrames)
		{
			pVertexFrames = new vtxVertexFramesCompressed();
			o_VertexFrames.reset(pVertexFrames);
		}
		else
			pVertexFrames = dynamic_cast<vtxVertexFramesCompressed*>(o_VertexFrames.get());
		if (!pVertexFrames)
		{
			DBG_ERROR("Incorrect mix of vertex animation types within one file.");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

		bool bHaveConfigurationInfo = false;
		shared_ptr<vtxDeltaEncoder> encoder;
		shared_ptr<vtxDeltaEncoder> normal_encoder;
		envType::Int32 max_num_frames_per_block = 0;
		envType::Int32 num_vertices = 0;
		envType::Int32 num_normals = 0;

		try
		{
			while ( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if ( name == c_NNAM )
				{
					i_Reader.Read(o_SurfaceName);
					//DBG_LOG("Reading compressed vertex animation for mesh: " << o_SurfaceName.c_str());
				}
				else if ( name == c_VCFG )
				{
					// Configuration information about the compressed frames coming
					bHaveConfigurationInfo = true;
					i_Reader.Read(max_num_frames_per_block);
					i_Reader.Read(num_vertices);
					i_Reader.Read(num_normals);

					// Create correct encoder type, throw exception if unrecognized.
					std::string encoder_string;
					chChunkParserUtil::Read(i_Reader, encoder_string);
					if (encoder_string == "EXT_DLT_ZL")
					{
						encoder.reset(new vtxDeltaLosslessEncoder());
						normal_encoder.reset(new vtxNormalLosslessEncoder());
					}
					else if (encoder_string == "EXT_TOL16_ZL")
					{
						encoder.reset(new vtxDeltaToleranceEncoder(0.0f)); // tolerance doesn't matter when importing
						normal_encoder.reset(new vtxNormal16BitEncoder());
					}
					else
					{
						// Future proof encoder styles, throw exception when unsupported.
						DBG_ERROR("Unrecognized compression codex: " << encoder_string);
						throw vtxDecompressErrorX(i_Reader.GetLocator());
					}

					// Allocate memory for the encoder to work...
					encoder->Allocate(max_num_frames_per_block, num_vertices);
					normal_encoder->Allocate(max_num_frames_per_block, num_normals);

				}
				else if ( name == c_CFRS )
				{
					// All compressed frames are in a sub-chunk so that we know that
					// we first have our configuration information.
					if (!bHaveConfigurationInfo)
					{
						// Need to read configuration info for compressed animation
						// before frames.
						DBG_ERROR("Need to read configuration info for compressed animation before frames.");
						throw mdlInvalidModelFileX(i_Reader.GetLocator());
					}

					// Begin a vertex set here,
					// Use size value of zero to signify we don't know the size yet,
					// it will be computed later.
					const chDefs::Size c_EmptySize = 0;
					shared_ptr<vtxDynamicVertexSet> vertex_set = pVertexFrames->CreateVertexSet(i_Reader.GetLocator(), c_EmptySize);
					//DBG_WARNING("New Vertex Set, size: " << i_Size << " fully loaded: " << vertex_set->IsFullyLoaded());

					shared_ptr<vtxCompressedDeltasSet> deltas_set = pVertexFrames->CreateDeltasSet(i_Reader.GetLocator(), 
													   vertex_set,
													   encoder,
													   normal_encoder,
													   max_num_frames_per_block,
													   num_vertices, 
													   num_normals);
					//DBG_WARNING("New Deltas Set, size: " << i_Size << " fully loaded: " << deltas_set->IsFullyLoaded());

					vtxVertexAnimKeysCompressed *pVertexAnimKeys = new vtxVertexAnimKeysCompressed(vertex_set, deltas_set);
					o_VertexAnim.reset(pVertexAnimKeys);

					read_compressed_frames(i_Reader, version, size, i_GeometryCacheOffset,
											*pVertexAnimKeys, vertex_set, deltas_set);

					// After we have read how many frames of data we have, but have not yet
					// read the frame data itself, we can submit the size info to the
					// vertex anim budgeting and see whether to load the data now.
					//
					// Since the vertex and deltas sets use a file seeker with its own file handle
					// and chBinReader, does that mean it won't mess up the current reading that
					// we are doing right now?
					vertex_set->SubmitForBudgeting(num_vertices, num_normals);
					// deltas set computes its own size and submits for budgeting now
					deltas_set->SubmitForBudgeting(); 
				}
				
				i_Reader.FinishChunk();
			}
			
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in vtxVertexAnimImport::ReadVertexAnimation");
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}

	}
}	// end of namespace

