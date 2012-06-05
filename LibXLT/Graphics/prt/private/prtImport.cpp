/****************************************************************************\
**  prtImport.cpp
**
**      prtImport.hpp handles importing baked particle animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtImport.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/prt/prtExceptionX.hpp"

//#undef FindResource


//============================================================================
//============================================================================
namespace prtImport
{
namespace
{

	// Read the whole array at once. Will fail on text file formats
	// or on files with different endian.
	const bool c_ReadArraysAtOnce = true;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_PTXA = chDefs::MakeName('P', 'T', 'X', 'A');
	const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');
	const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_FRAM = chDefs::MakeName('F', 'R', 'A', 'M');
	const chDefs::Name c_TIME = chDefs::MakeName('T', 'I', 'M', 'E');
	const chDefs::Name c_PATT = chDefs::MakeName('P', 'A', 'T', 'T');

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void read_vertex_frame( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							prtVertexFrame& o_VertexFrame,
							float &o_Time)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;
		bool did_read_PATT = false;
		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				if( name == c_TIME )
				{
					i_Reader.Read(o_Time);
	//				DBG_LOG("Reading vertex frame for time: " << o_Time);
				}
				else if( name == c_PATT )
				{
					// Because of binary packing, can't read old format files
					if (version != 1)
					{
						DBG_LOG("Unexpected particle info in prtImport::read_vertex_frame");
						throw prtInvalidlFileFormatX(i_Reader.GetLocator());
					}

					did_read_PATT = true;
					envType::Int32 nParticles;
					i_Reader.Read(nParticles);

					o_VertexFrame.m_Particles.resize(nParticles);
					if (c_ReadArraysAtOnce)
					{
						if (nParticles > 0)
						{
							// Read the whole array at once. 
							i_Reader.Read(&o_VertexFrame.m_Particles[0], nParticles * sizeof(prtParticleAttr));
						}
					}
					else
					{
						envType::Float32 val;
						for (int i = 0; i < nParticles; i++) 
						{
							prtParticleAttr &part = o_VertexFrame.m_Particles[i];
							i_Reader.Read(part.m_ID);
							//DBG_LOG("Part ID: " << part.m_ID);

							i_Reader.Read(val);
							part.m_Position.m_X = val;
							i_Reader.Read(val);
							part.m_Position.m_Y = val;
							i_Reader.Read(val);
							part.m_Position.m_Z = val;
							i_Reader.Read(val);
							part.m_Age = val;
							i_Reader.Read(val);
							part.m_Lifespan = val;
						}
					}
				}
				
				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in prtImport::read_vertex_frame");
			throw prtInvalidlFileFormatX(i_Reader.GetLocator());
		}

		DBG_ASSERT(did_read_PATT, "Did not read position data for vertex animation frame.");

	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void read_vertex_animation( chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								std::string& o_particleName,
								prtVertexAnimKeys& o_VertexAnim,
								std::vector<prtVertexFrame*>& o_VertexFrames)
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
					i_Reader.Read(o_particleName);
					DBG_LOG("Reading vertex animation for particle: " << o_particleName.c_str());
				}
				else 
				if( name == c_FRAM )
				{
					prtVertexFrame *pVertexFrame = new prtVertexFrame;
					float time = 0;
					read_vertex_frame(i_Reader, version, size, *pVertexFrame, time);
					// It is okay for the frame to be empty after reading it in.
					// The assert moved up into the read_vertex_frame function.
					//DBG_ASSERT(!pVertexFrame->m_Particles.empty(), "Did not read position data for vertex animation frame.");

					o_VertexAnim.Frames().AddKey(time, pVertexFrame);
					o_VertexFrames.push_back(pVertexFrame);
				}
				
				i_Reader.FinishChunk();
			}
			
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_LOG("Invalid chunk in prtImport::read_vertex_animation");
			throw prtInvalidlFileFormatX(i_Reader.GetLocator());
		}
	}

}	// end of local namespace


//------------------------------------------------------------------------
//	LoadVertexAnimation loads the baked vertex and normal 
//	animation from the given file.
//------------------------------------------------------------------------
void LoadVertexAnimation( const fsLocator& i_Locator,
						std::vector<prtVertexAnimKeys>& o_VertexKeys,
						std::vector<prtVertexFrame*>& o_VertexFrames,
						float &o_FramesPerSecond,
						float &o_BeginFrame)
{
	o_FramesPerSecond = g3dConstants::c_fDefaultFrameRate;

	fsResourceTracker::MarkBegin(i_Locator);

	gfFileBin file(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);

	//	If this isn't a real Terawatt/XLT binary file this will throw
	file.ReadHeader();

	chBinReader reader(file);

	chDefs::Name name;
	chDefs::Size size;
	chDefs::Version version;

	try
	{
		while( reader.ReadChunkHeader(name, version, size) )
		{
//			DBG_LOG("LoadVertexAnimation, got chunk: " << (char*)(&name));
			if ( name == c_PTXA )
			{
				prtVertexAnimKeys keys;
				std::string particle_name; // ignoring particle name in PTA files
				read_vertex_animation(reader, version, size, particle_name, keys, o_VertexFrames);
				o_VertexKeys.push_back(keys);
			}
			else if ( name == c_AFPS )
			{
				// Read fps to play this animation
				reader.Read(o_FramesPerSecond);
			}
			else if ( name == c_BGFR )
			{
				// Read first frame of animation when exported
				reader.Read(o_BeginFrame);
			}

			reader.FinishChunk();
		}
	}
	catch ( const chInvalidChunkX& i_Ex )
	{
		i_Ex;
		DBG_LOG("Invalid chunk in prtImport::LoadVertexAnimation");
		throw prtInvalidlFileFormatX(i_Locator);
	}
		
	fsResourceTracker::MarkEnd(i_Locator);
}

}	// end of namespace

