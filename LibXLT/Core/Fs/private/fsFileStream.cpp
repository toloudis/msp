/**********************************************************
**  fsFileStream.cpp
**
**      see .hpp
**
**	The file is a std::filebuf opened with a std::filesystem::path, so
**	this is the same on every platform. Reads and writes go through the
**	stream's own buffer as before; its behavior is pinned down by
**	UnitTests/CoreTests/Fs/fsFileStreamModeTest.cpp and fsFileTest.cpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/
#include "Core/fs/fsFileStream.hpp"

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_PATHUTIL_HPP
#include "Core/fs/fsPathUtil.hpp"
#endif
#ifndef FS_RESOURCETRACKER_HPP
#include "Core/fs/fsResourceTracker.hpp"
#endif
#ifndef FS_FILEERRORUTIL_HPP
#include "Core/fs/private/fsFileErrorUtil.hpp"
#endif
#include "Core/Ma/maFunctions.hpp"

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <system_error>


//============================================================================
//============================================================================
const int c_ReadBufferSize = 512;
const int c_WriteBufferSize = 512; //4096;
typedef uint8_t fsBufferChar;


//============================================================================
//============================================================================
enum BufferFormat
{
	e_BufferRead,
	e_BufferWrite
	//bga - potential for a e_NoBuffering flag here also
};


//============================================================================
//============================================================================
class fsFileStreamImp
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	fsFileStreamImp()
	:	m_Access(fsFileStream::e_ReadOnly),
		m_FilePointer(0),
		m_pBuffer(NULL),
		m_BufferLoc(0),
		m_BufferValid(0),
		m_BufferFormat(e_BufferRead)
	{
	}

public:
	std::filebuf m_File;
	std::filesystem::path m_Path;
	fsFileStream::AccessType m_Access;
	fsFileStream::FilePosType m_FilePointer;
	fsLocator m_Locator;
	// Buffer for reading in chunks of data
	fsBufferChar* m_pBuffer;
	int	m_BufferLoc;
	int	m_BufferValid;
	BufferFormat m_BufferFormat;
};


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//	A read or write failed: the C runtime under std::filebuf reports why
	//	in errno (the standard doesn't promise it, so with no errno the
	//	error is unknown)
	//------------------------------------------------------------------------
	[[noreturn]] void throw_io_error(const fsLocator& i_Locator)
	{
		if (errno == 0)
			throw fsUnknownX(i_Locator);
		fsFileErrorUtil::Throw(std::error_code(errno, std::generic_category()), i_Locator);
	}

	//------------------------------------------------------------------------
	// Attempt to read a chunk of data into the buffer
	//------------------------------------------------------------------------
	void read_into_buffer(fsFileStreamImp* io_pImp)
	{
		DBG_ASSERT(io_pImp->m_BufferFormat==e_BufferRead, "Needs read buffer format.");
		io_pImp->m_BufferLoc = 0;
		io_pImp->m_BufferValid = int(io_pImp->m_File.sgetn(reinterpret_cast<char*>(io_pImp->m_pBuffer), c_ReadBufferSize));
	}

	//------------------------------------------------------------------------
	// Attempt to write the contents of the delayed write buffer
	//to the file.
	//------------------------------------------------------------------------
	void write_out_buffer(fsFileStreamImp* io_pImp)
	{
		DBG_ASSERT(io_pImp->m_BufferFormat==e_BufferWrite, "Needs write buffer format.");
		const std::streamsize count = io_pImp->m_BufferValid;
		errno = 0;
		if (io_pImp->m_File.sputn(reinterpret_cast<const char*>(io_pImp->m_pBuffer), count) != count)
			throw_io_error(io_pImp->m_Locator);
		io_pImp->m_BufferLoc = 0;
		io_pImp->m_BufferValid = 0;
	}

	//------------------------------------------------------------------------
	// If we are writing through a delayed buffer, write the contents
	//	of the buffer to the file.
	//------------------------------------------------------------------------
	void flush_buffer(fsFileStreamImp* io_pImp)
	{
		if ((io_pImp->m_BufferFormat==e_BufferWrite) &&
			(io_pImp->m_BufferValid > 0))
		{
			write_out_buffer(io_pImp);
		}
	}

	//------------------------------------------------------------------------
	//	The stream couldn't be opened: forget the file and throw
	//------------------------------------------------------------------------
	[[noreturn]] void open_failed(const fsLocator& i_Locator, const std::error_code& i_Error)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Locator, filename);
		DBG_WARNING( "Can't open file " << filename.c_str() );

		fsResourceTracker::Remove(i_Locator);		// shouldn't be here?  put where throw is CAUGHT

		if (!i_Error)
			throw fsUnknownX(i_Locator);
		fsFileErrorUtil::Throw(i_Error, i_Locator);
	}
}

//------------------------------------------------------------------------
//	Constructor.  This could throw an exception if the file doesn't
//	exist, or something.  So be ready!
//
//	Every access type opens an existing file; none creates one
//	(fsFileUtil::CreateFile does). e_WriteOnly writes over the file from
//	the start, keeping what is past the end of what is written;
//	e_WriteCreate empties it first.
//------------------------------------------------------------------------
fsFileStream::fsFileStream(const fsLocator& i_Locator, AccessType i_DesiredAccess)
{
	std::ios::openmode mode = std::ios::binary;
	BufferFormat format = e_BufferRead;

	switch ( i_DesiredAccess )
	{
		case e_ReadOnly:
			mode |= std::ios::in;
			format = e_BufferRead;
		break;

		case e_WriteOnly:
			// in | out opens an existing file without emptying it
			mode |= std::ios::in | std::ios::out;
			format = e_BufferWrite;
		break;

		case e_WriteCreate:
			mode |= std::ios::out | std::ios::trunc;
			format = e_BufferWrite;
		break;

		default:
			DBG_ASSERT(false, "Invalid Access type");
		break;
	}

	const std::filesystem::path path = fsPathUtil::LocatorToPath(i_Locator);
	if ( path.empty() )
	{
		fsResourceTracker::Remove(i_Locator);		// shouldn't be here?  put where throw is CAUGHT
		throw fsInvalidLocatorX(i_Locator);
	}

	// The file must exist (out | trunc would create it), and be a file
	std::error_code status_error;
	const std::filesystem::file_status status = std::filesystem::status(path, status_error);
	if ( !std::filesystem::exists(status) )
		open_failed(i_Locator, std::make_error_code(std::errc::no_such_file_or_directory));
	if ( std::filesystem::is_directory(status) )
		open_failed(i_Locator, std::make_error_code(std::errc::permission_denied));

	// save our work
	m_pImp = new fsFileStreamImp;

	errno = 0;
	if ( !m_pImp->m_File.open(path, mode) )
	{
		const int error = errno;
		delete m_pImp;
		m_pImp = NULL;
		open_failed(i_Locator, error != 0 ? std::error_code(error, std::generic_category()) : std::error_code());
	}

	m_pImp->m_Path = path;
	m_pImp->m_Access = i_DesiredAccess;
	m_pImp->m_FilePointer = 0;
	m_pImp->m_Locator = i_Locator;
	m_pImp->m_pBuffer = NULL;
	m_pImp->m_BufferLoc = 0;
	m_pImp->m_BufferFormat = format;
}

//------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------
fsFileStream::~fsFileStream()
{
	//m_pImp->m_FilePointer
	flush_buffer(m_pImp);	// make sure write buffer is flushed to disk before closing
	m_pImp->m_File.close();
	if (m_pImp->m_pBuffer != NULL)
		delete [] m_pImp->m_pBuffer;
	delete m_pImp;
}

//------------------------------------------------------------------------
//	Read reads i_NumBytes into the buffer.  If the file is too short
//	to read	i_NumBytes, it will read as many as it can.  The return
//	value is the number actually read into the o_Buffer.  This function
//	also advances the file pointer.
//------------------------------------------------------------------------
int fsFileStream::Read(int64_t i_NumBytes, void* o_Buffer)
{
	DBG_ASSERT(m_pImp->m_Access != e_WriteOnly, "Wrong access type for Read");
	DBG_ASSERT(m_pImp->m_BufferFormat==e_BufferRead, "Needs read buffer format.");

	int64_t num_read = 0;

	// Initialize the buffered reading
	if ((!m_pImp->m_pBuffer) && (c_ReadBufferSize > 0))
	{
		m_pImp->m_pBuffer = new fsBufferChar[c_ReadBufferSize];
		m_pImp->m_BufferValid = 0;
		m_pImp->m_BufferLoc = 0;
		read_into_buffer(m_pImp);
	}

	// How much data is left in the buffer
	int64_t num_left_in_buffer = m_pImp->m_BufferValid - m_pImp->m_BufferLoc;
	if (i_NumBytes <= num_left_in_buffer)
	{
		// We can deliver this request purely with data already in
		// the buffer.
		::memcpy(o_Buffer, m_pImp->m_pBuffer+m_pImp->m_BufferLoc, size_t(i_NumBytes));
		m_pImp->m_BufferLoc += int(i_NumBytes);
		num_read = i_NumBytes;
	}
	else
	{
		int64_t left_to_read = i_NumBytes;
		fsBufferChar* output_buffer = static_cast<fsBufferChar*>(o_Buffer);
		if (num_left_in_buffer > 0)
		{
			// Put the rest of our buffer into the requesting array
			::memcpy(o_Buffer, m_pImp->m_pBuffer+m_pImp->m_BufferLoc, size_t(num_left_in_buffer));
			m_pImp->m_BufferLoc += int(num_left_in_buffer);

			left_to_read -= num_left_in_buffer;
			output_buffer += num_left_in_buffer;
			num_read += num_left_in_buffer;
		}

		// If the requester wants more data than can fit in a single buffer
		// read, then just read that directly into the output buffer
		if (left_to_read > c_ReadBufferSize)
		{
			num_read += m_pImp->m_File.sgetn(reinterpret_cast<char*>(output_buffer), left_to_read);
		}
		else if (left_to_read > 0)
		{
			// If there is still more to read, then fill the buffer again
			read_into_buffer(m_pImp);
			num_left_in_buffer = m_pImp->m_BufferValid;

			if (num_left_in_buffer > 0)
			{
				// Copy more from our buffer into the requesting array
				int amount_to_copy = int(maFunctions::Lowest(num_left_in_buffer, left_to_read));
				::memcpy(output_buffer, m_pImp->m_pBuffer, amount_to_copy);
				m_pImp->m_BufferLoc += amount_to_copy;
				num_read += amount_to_copy;
			}
		}
	}

	// Advance the internal file pointer
	m_pImp->m_FilePointer += num_read;

	return int(num_read);
}

//------------------------------------------------------------------------
//	Write writes i_NumBytes from o_Buffer into the file.  The file
//	pointer will also be advanced.
//------------------------------------------------------------------------
void fsFileStream::Write(int64_t i_NumBytes, const void* i_Buffer)
{
	DBG_ASSERT(m_pImp->m_Access != e_ReadOnly, "Wrong access type for Write");
	DBG_ASSERT(m_pImp->m_BufferFormat==e_BufferWrite, "Needs write buffer format.");

	// If the requester wants to write more data than can fit in a single buffer,
	// then just write that directly into the file
	if (i_NumBytes > c_WriteBufferSize)
	{
		// Flush anything we had in the buffer
		flush_buffer(m_pImp);

		// Write the full request directly to file
		errno = 0;
		if (m_pImp->m_File.sputn(static_cast<const char*>(i_Buffer), i_NumBytes) != i_NumBytes)
			throw_io_error(m_pImp->m_Locator);
	}
	else
	{
		// Initialize the buffered writing
		if ((!m_pImp->m_pBuffer) && (c_WriteBufferSize > 0))
		{
			m_pImp->m_pBuffer = new fsBufferChar[c_WriteBufferSize];
			m_pImp->m_BufferValid = 0;
			m_pImp->m_BufferLoc = 0;
		}

		// How much room is left in the buffer
		int64_t num_left_in_buffer = c_WriteBufferSize - m_pImp->m_BufferLoc;
		if (i_NumBytes > num_left_in_buffer)
		{
			// Flush anything we had in the buffer
			flush_buffer(m_pImp);
		}

		// Since there was an earlier test for whether the request was
		// larger than the buffer size, we know we can handle this request
		// purely within the memory buffer
		::memcpy(m_pImp->m_pBuffer+m_pImp->m_BufferLoc, i_Buffer, size_t(i_NumBytes));
		m_pImp->m_BufferLoc += int(i_NumBytes);
		// Note that we might have backed up within the memory buffer,
		// so we only want to increase the valid counter when the
		// current buffer loc has written pass the old valid counter.
		if (m_pImp->m_BufferLoc > m_pImp->m_BufferValid)
			m_pImp->m_BufferValid = m_pImp->m_BufferLoc;
	}

	// Advance the internal file pointer
	m_pImp->m_FilePointer += i_NumBytes;
}

//------------------------------------------------------------------------
//	GetFilePos returns the current position of the file pointer.
//------------------------------------------------------------------------
fsFileStream::FilePosType fsFileStream::GetFilePos() const
{
	return m_pImp->m_FilePointer;
}

//------------------------------------------------------------------------
//	GetFileSize returns the size of the file: what has reached it, not
//	what is still in this stream's write buffer.
//------------------------------------------------------------------------
int64_t fsFileStream::GetFileSize() const
{
	// What std::filebuf holds back has been handed to the file already
	if (m_pImp->m_BufferFormat == e_BufferWrite)
		m_pImp->m_File.pubsync();

	std::error_code error;
	const std::uintmax_t size = std::filesystem::file_size(m_pImp->m_Path, error);
	return error ? 0 : int64_t(size);
}

//------------------------------------------------------------------------
//	GetLocator returns this filestream's associated locator
//------------------------------------------------------------------------
const fsLocator& fsFileStream::GetLocator() const
{
	return m_pImp->m_Locator;
}

//------------------------------------------------------------------------
//	SetFilePos sets the file pointer to the given value.  It is valid
//	to move the file pointer beyond the end of the file.
//------------------------------------------------------------------------
void fsFileStream::SetFilePos(  FilePosType i_Pos, AccessPointType i_DesiredAccessPoint )
{
	if( i_DesiredAccessPoint == e_Beginning )
	{
		DBG_ASSERT( i_Pos >= 0,  "Negative seek attempted" );
	}
	else if( i_DesiredAccessPoint == e_Current )
	{
		DBG_ASSERT( m_pImp->m_FilePointer + i_Pos >= 0, "Negative seek attempted" );
	}

	// See if we can resolve this move simply by moving within
	// the buffer we already have in memory.
	bool bNeedFileSeek = true;
	if ((m_pImp->m_pBuffer != NULL) && (i_DesiredAccessPoint != e_End))
	{
		// Convert both seek methods into a delta from the current position
		int64_t delta = (i_DesiredAccessPoint == e_Beginning) ? (i_Pos - m_pImp->m_FilePointer) : i_Pos;
		if (delta == 0)
		{
			// Already at the correct position
			bNeedFileSeek = false;
		}
		else if (m_pImp->m_BufferValid > 0)
		{
			int64_t new_pos = m_pImp->m_BufferLoc + delta;
			if (new_pos >= 0 && new_pos < m_pImp->m_BufferValid)
			{
				bNeedFileSeek = false;
				m_pImp->m_BufferLoc = int(new_pos);
				m_pImp->m_FilePointer += delta;
			}
		}
	}

	if (bNeedFileSeek)
	{
		// If we are writing a delayed buffer and we have to seek outside
		// of the buffer range, then flush what we have written
		flush_buffer(m_pImp);

		// Mark the buffer as invalid
		m_pImp->m_BufferLoc = m_pImp->m_BufferValid = 0;

		// The file's position is past what has been read from the buffer
		// (it read ahead to fill it), so a move from the current position
		// is made from m_FilePointer instead
		std::streampos new_pos;
		if (i_DesiredAccessPoint == e_End)
			new_pos = m_pImp->m_File.pubseekoff(i_Pos, std::ios::end);
		else if (i_DesiredAccessPoint == e_Current)
			new_pos = m_pImp->m_File.pubseekpos(m_pImp->m_FilePointer + i_Pos);
		else
			new_pos = m_pImp->m_File.pubseekpos(i_Pos);

		if (new_pos == std::streampos(std::streamoff(-1)))
			throw fsUnknownX(m_pImp->m_Locator);

		m_pImp->m_FilePointer = std::streamoff(new_pos);
	}
}
