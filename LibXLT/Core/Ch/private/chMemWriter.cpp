/*****************************************************************************
**  chMemWriter.cpp
**
**      see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Core/ch/chMemWriter.hpp"
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include "Core/ch/chExceptionX.hpp"

#include <climits>
#include <cstring>


//============================================================================
//============================================================================
namespace
{
	// chBinWriter's limit: the size is written as an unsigned 32-bit integer.
	const int64_t c_MaxChunkSize = UINT_MAX;

	const size_t c_HeaderSize = sizeof(chDefs::Name) + sizeof(chDefs::Size) + sizeof(chDefs::Version);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chMemWriter::chMemWriter(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chMemWriter::~chMemWriter()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chMemWriter::WriteChunkHeader(	chDefs::Name i_Name,
									chDefs::Version i_Version,
									bool /*i_Container*/)
{
	m_HeaderLocs.push_back(m_Bytes.size());
	WriteValue(i_Name);
	WriteValue(chDefs::Size(-1));	// patched by FinishChunk
	WriteValue(i_Version);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chMemWriter::FinishChunk()
{
	DBG_ASSERT( m_HeaderLocs.size() > 0, "FinishChunk w/o corresponding WriteChunkHeader");

	const size_t header_pos = m_HeaderLocs.back();
	const int64_t chunk_size = int64_t(m_Bytes.size() - (header_pos + c_HeaderSize));
	if (chunk_size > c_MaxChunkSize)
	{
		DBG_ERROR("Invalid chunk -- chunk size exceeding maximum size.");
		throw chInvalidChunkX();
	}

	const chDefs::Size size = chDefs::Size(chunk_size);
	::memcpy(&m_Bytes[header_pos + sizeof(chDefs::Name)], &size, sizeof(size));
	m_HeaderLocs.pop_back();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chMemWriter::Write(int8_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(uint8_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(int16_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(uint16_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(int32_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(uint32_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(int64_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(uint64_t i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(float i_Val)	{ WriteValue(i_Val); }
void chMemWriter::Write(double i_Val)	{ WriteValue(i_Val); }

void chMemWriter::Write(bool i_Val)
{
	WriteValue(static_cast<int8_t>( i_Val ));
}

void chMemWriter::Write(const char* i_Val)
{
	Write(static_cast<const void*>(i_Val), int64_t(::strlen(i_Val) + 1));	// with the NULL
}

void chMemWriter::Write(const char* i_Val, int i_Length)
{
	Write(static_cast<const void*>(i_Val), int64_t(i_Length));
}

void chMemWriter::Write(const envAppVersion& i_Val)
{
	WriteValue(i_Val.GetMajor());
	WriteValue(i_Val.GetMinor());
	WriteValue(i_Val.GetRevision());
	WriteValue(i_Val.GetBuild());
}

void chMemWriter::Write(const void* i_Data, int64_t i_NumBytes)
{
	const uint8_t* data = static_cast<const uint8_t*>(i_Data);
	m_Bytes.insert(m_Bytes.end(), data, data + i_NumBytes);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const fsLocator& chMemWriter::GetLocator() const
{
	return m_Locator;
}
