/*****************************************************************************
**	vtxGeometryCacheWriter.hpp
**
**		vtxGeometryCacheWriter manages a section of an animation file which
**	contains raw and compressed data in any order. It returns file offsets
**	into the cache for each section of animation data in order to
**	allow random access to the data later.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_GEOMETRYCACHEWRITER_HPP
#error vtxGeometryCacheWriter.hpp multiply included
#endif
#define VTX_GEOMETRYCACHEWRITER_HPP

#ifndef FS_FILESTREAM_HPP
#include "Core/Fs/fsFileStream.hpp"
#endif 


//============================================================================
//============================================================================
class chWriter;
class gfFileBin;
struct vtxVertexFrame;


//============================================================================
//============================================================================
class vtxGeometryCacheWriter
{
public:
	//--------------------------------------------------------------------
	// Constructor takes file to which to write data 
	//--------------------------------------------------------------------
	vtxGeometryCacheWriter(chWriter &io_Writer, gfFileBin &i_File);

	//--------------------------------------------------------------------
	// Get locator to which this geoemtry cache is writing,
	// used for error messages
	//--------------------------------------------------------------------
	const fsLocator& GetLocator() const;

	//--------------------------------------------------------------------
	// Begin and End the geometry cache chunk. While the cache is open,
	// only this class should be writing to the chunk writer.
	//--------------------------------------------------------------------
	void OpenCache();
	void CloseCache();

	//--------------------------------------------------------------------
	// Return whether the goemetry cache is open.
	//--------------------------------------------------------------------
	bool IsOpen() const { return m_bIsOpen; }

	//--------------------------------------------------------------------
	// Get file position as offset from the beginning of the cache
	//--------------------------------------------------------------------
	fsFileStream::FilePosType GetCacheOffset() const;

	//--------------------------------------------------------------------
	// Write the full vertex animation frame data for this frame
	//--------------------------------------------------------------------
	void WriteReferenceFrame(const vtxVertexFrame &i_Frame);
	
	//--------------------------------------------------------------------
	// Write encoded deltas as a large buffer of memory
	//--------------------------------------------------------------------
	void WriteDeltas(const void* i_Data, int i_NumBytes);

private:
	chWriter &m_Writer;
	gfFileBin &m_File;
	bool m_bIsOpen;
	fsFileStream::FilePosType m_CacheStart;
};
