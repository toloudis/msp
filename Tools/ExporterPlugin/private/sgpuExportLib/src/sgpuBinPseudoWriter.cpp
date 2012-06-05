/*****************************************************************************
**  sgpuBinPseudoWriter.cpp
**
**      sgpuBinPseudoWriter is a chWriter which writes binary chunk files.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "sgpuBinPseudoWriter.hpp"
#include "sgpuException.hpp"

#include <limits>

#include "Core/fs/fsFilePosSaver.hpp"
#include "Core/gf/gfFileUtil.hpp"

#undef max

namespace
{
		fsLocator	dummyLocator;
}

//====================================================================
//====================================================================
sgpuBinPseudoWriter::sgpuBinPseudoWriter( int i_InitialOffset ):
m_CurFilePos( i_InitialOffset )
{
}

//====================================================================
//====================================================================
sgpuBinPseudoWriter::~sgpuBinPseudoWriter()
{
}

//====================================================================
//	WriteChunkHeader writes a chunk header in the file at the
//	current position.
//====================================================================
void sgpuBinPseudoWriter::WriteChunkHeader(	chDefs::Name i_Name, 
									chDefs::Version i_Version, 
									bool i_Container)
{
	int header_pos = m_CurFilePos;
	m_HeaderLocs.push(header_pos);
	//gfFileUtil::Write(m_File, i_Name);
	m_CurFilePos += sizeof ( chDefs::Name );
	//gfFileUtil::Write(m_File, chDefs::Size(-1));	// don't know the size yet, write a obviously wrong value
	m_CurFilePos += sizeof( chDefs::Size );
	//gfFileUtil::Write(m_File, i_Version);
	m_CurFilePos += sizeof( chDefs::Version );
}

//====================================================================
//	FinishChunk will cause the writer to compute the size of the 
//	chunk and finish writing the header.
//====================================================================
void sgpuBinPseudoWriter::FinishChunk()
{
	DBG_ASSERT( m_HeaderLocs.size() > 0, "FinishChunk w/o corresponding WriteChunkHeader");	
	int header_pos = m_HeaderLocs.top();
	int chunk_size = m_CurFilePos - (header_pos + sizeof(chDefs::Name) + sizeof(chDefs::Size) + sizeof(chDefs::Version));
	m_HeaderLocs.pop();
}

//====================================================================
//	Write functions - These write the data to the chunk file.
//====================================================================
void sgpuBinPseudoWriter::Write(envType::Int8 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);	
	m_CurFilePos += sizeof( envType::Int8 );
}

void sgpuBinPseudoWriter::Write(envType::UInt8 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::UInt8 );
}

void sgpuBinPseudoWriter::Write(envType::Int16 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::Int16 );
}

void sgpuBinPseudoWriter::Write(envType::UInt16 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::UInt16 );
}

void sgpuBinPseudoWriter::Write(envType::Int32 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::Int32 );
}

void sgpuBinPseudoWriter::Write(envType::UInt32 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::UInt32 );
}

void sgpuBinPseudoWriter::Write(envType::Int64 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::Int64 );
}

void sgpuBinPseudoWriter::Write(envType::UInt64 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::UInt64 );
}

void sgpuBinPseudoWriter::Write(envType::Float32 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::Float32 );
}

void sgpuBinPseudoWriter::Write(envType::Float64 i_Val)
{
	//gfFileUtil::Write(m_File, i_Val);
	m_CurFilePos += sizeof( envType::Float64 );
}

//====================================================================
//	This Write writes a string called either "TRUE" or "FALSE" based 
//  on a bool value.
//====================================================================
void sgpuBinPseudoWriter::Write(bool i_Val)
{
	//gfFileUtil::Write(m_File, static_cast<envType::Int8>( i_Val ));
	m_CurFilePos += sizeof( envType::Int8 );
}

//====================================================================
//	This Write writes a NULL-terminated single-byte character string
//====================================================================
void sgpuBinPseudoWriter::Write(const char* i_Val)
{
	int num_to_write = ::strlen(i_Val);
	//m_File.Write(num_to_write + 1, i_Val);	// write NULL also
	m_CurFilePos += ( num_to_write + 1 );
}

//====================================================================
//	This Write writes a single-byte character string with a length
//	given by i_Length.
//====================================================================
void sgpuBinPseudoWriter::Write(const char* i_Val, int i_Length)
{
	//m_File.Write(i_Length, i_Val);
	m_CurFilePos += i_Length;
}

//====================================================================
//	This Write writes a NULL-terminated Unicode character string
//====================================================================
void sgpuBinPseudoWriter::Write(const envType::UInt16* i_Val)
{
	int num_to_write = 0;
	
	while( i_Val[num_to_write] )
		num_to_write++;

	//m_File.Write(2 * (num_to_write + 1), i_Val);	// write NULL also
	m_CurFilePos += ( num_to_write + 1 );
}

//====================================================================
//	This Write writes a Unicode character string with a length
//	given by i_Length.
//====================================================================
void sgpuBinPseudoWriter::Write(const envType::UInt16* i_Val, int i_Length)
{
	//m_File.Write(i_Length * 2, i_Val);
	m_CurFilePos += i_Length * 2;
}


//====================================================================
//	Write chunk of binary data; this is only for
//	sgpuBinPseudoWriter, it is not a function of chWriter base class
//====================================================================
void sgpuBinPseudoWriter::Write(const void* i_Data, int i_NumBytes)
{
	//m_File.Write(i_NumBytes, i_Data);
	m_CurFilePos += i_NumBytes;
}

//====================================================================
//	Write chunk of binary data; this is only for
//	sgpuBinPseudoWriter, it is not a function of chWriter base class
//====================================================================
void sgpuBinPseudoWriter::Write(const void* i_Data, envType::Int64 i_NumBytes )
{
	if( i_NumBytes >= std::numeric_limits< int >::max() )
	{
		throw sgpuException( sgpuString("number of bytes to be written is high") );
	}
	m_CurFilePos += static_cast< int > ( i_NumBytes );
}

//========================================================================
//	GetLocator returns the dummy locator
//========================================================================
const fsLocator& sgpuBinPseudoWriter::GetLocator() const
{
	return dummyLocator;
}

