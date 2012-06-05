/*****************************************************************************
**	vtxGeometryCacheWriter.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxGeometryCacheWriter.hpp"

#include "Core/Ch/chWriter.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Graphics/vtx/vtxVertexAnimWriter.hpp"
#include "Graphics/vtx/vtxVertexFrame.hpp"


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_GCHE = chDefs::MakeName('G', 'C', 'H', 'E');
}


//--------------------------------------------------------------------
// Constructor takes file to which to write data 
//--------------------------------------------------------------------
vtxGeometryCacheWriter::vtxGeometryCacheWriter(chWriter &io_Writer, gfFileBin &i_File)
:	m_Writer(io_Writer),
	m_File(i_File),
	m_bIsOpen(false)
{
}

//--------------------------------------------------------------------
// Get locator to which this geoemtry cache is writing,
// used for error messages
//--------------------------------------------------------------------
const fsLocator& vtxGeometryCacheWriter::GetLocator() const
{
	return m_File.GetLocator();
}

//--------------------------------------------------------------------
// Begin and End the geometry cache chunk. While the cache is open,
// only this class should be writing to the chunk writer.
//--------------------------------------------------------------------
void vtxGeometryCacheWriter::OpenCache()
{
	m_Writer.WriteChunkHeader(c_GCHE, 0, true);
	m_bIsOpen = true;
	m_CacheStart = m_File.GetFilePos();
}
void vtxGeometryCacheWriter::CloseCache()
{
	m_Writer.FinishChunk();
	m_bIsOpen = false;
}

//--------------------------------------------------------------------
// Get file position as offset from the beginning of the cache
//--------------------------------------------------------------------
fsFileStream::FilePosType vtxGeometryCacheWriter::GetCacheOffset() const
{
	return (m_File.GetFilePos() - m_CacheStart);
}

//--------------------------------------------------------------------
// Write the full vertex animation frame data for this frame
//--------------------------------------------------------------------
void vtxGeometryCacheWriter::WriteReferenceFrame(const vtxVertexFrame &i_Frame)
{
	DBG_ASSERT(m_bIsOpen, "Need to open the geometry cache before writing to it.");
	if (!m_bIsOpen)
		return;
	//bool c_bTwoComponentNormals = true; // compress normals
	vtxVertexAnimWriter::WriteVertexFrameData(m_Writer, i_Frame); // c_bTwoComponentNormals);
}

//--------------------------------------------------------------------
// Write encoded deltas as a large buffer of memory
//--------------------------------------------------------------------
void vtxGeometryCacheWriter::WriteDeltas(const void* i_Data, int i_NumBytes)
{
	DBG_ASSERT(m_bIsOpen, "Need to open the geometry cache before writing to it.");
	if (!m_bIsOpen)
		return;
	vtxVertexAnimWriter::WriteDeltas(m_Writer, i_Data, i_NumBytes);
}
