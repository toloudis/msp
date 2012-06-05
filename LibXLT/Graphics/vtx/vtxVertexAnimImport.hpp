/****************************************************************************\
**	vtxVertexAnimImport.hpp
**
**		vtxVertexAnimImport.hpp supplies functions used to import 
**	animation files.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXANIMIMPORT_HPP
#error vtxVertexAnimImport.hpp multiply included
#endif
#define VTX_VERTEXANIMIMPORT_HPP

#ifndef CH_DEFS_HPP
#include "Core/Ch/chDefs.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef FS_FILESTREAM_HPP
#include "Core/Fs/fsFileStream.hpp"
#endif 

#include <string>
#include <vector>


//============================================================================
//============================================================================
class chReader;
class chWriter;
class maAxisBox;
class vtxVertexAnimKeys;
class vtxVertexFrames;
struct vtxVertexFrame;


//============================================================================
//	Any of these vtxVertexAnimImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace vtxVertexAnimImport
{
	//------------------------------------------------------------------------
	// Read compressed deltas into the given output buffer,
	// Returns amount of data read in o_SizeRead.
	//------------------------------------------------------------------------
	void ReadDeltas( chReader& i_Reader,
					 void* o_OutputData, 
					 int i_OutputBufferSize,
					 int &o_SizeRead );

	//------------------------------------------------------------------------
	// Read a single frame of vertex animation data
	//	(state of mesh at certain time). Does not read time or bounding box.
	// This is intended for dynamic paging of the animation data and the 
	// file pointer should be at the beginning of the FRAM or FRDT chunk.
	//------------------------------------------------------------------------
	void ReadSingleVertexFrameData( chReader& i_Reader,
									vtxVertexFrame& o_VertexFrame);

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
								shared_ptr<vtxVertexFrames>& o_VertexFrames);

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
								shared_ptr<vtxVertexFrames>& o_VertexFrames);

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
								shared_ptr<vtxVertexFrames>& o_VertexFrames);
}

