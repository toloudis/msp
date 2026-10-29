/*****************************************************************************
**	chReader.hpp
**
**		chReader defines the chReader class, which serves as a base for
**	chunk file readers.
**
**	StudioGPU
**	Copyright(C) 2003-10 - All Rights Reserved
\****************************************************************************/
#ifdef CH_READER_HPP
#error chReader.hpp multiply included
#endif
#define CH_READER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef ENV_APPVERSION_HPP
#include "Core/env/envAppVersion.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/Fs/fsFileStream.hpp"
#endif 

#include <string>


//============================================================================
//	forward references
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
class chReader
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chReader();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chReader() = 0;

		//--------------------------------------------------------------------
		//	ReadChunkHeader reads a chunk header in the file at the
		//	current position.  If there is a problem with the header,
		//	it will throw a chInvalidChunkX.  Returns true if more chunks to 
		//	read, false if no more.
		//	The size is the size of the chunk not including the header.
		//--------------------------------------------------------------------
		virtual bool ReadChunkHeader(	chDefs::Name& o_Name, 
										chDefs::Version& o_Version,
										chDefs::Size& o_Size) = 0;

		//--------------------------------------------------------------------
		//	FinishChunk will cause the reader to advance to the next position
		//	after the end of the current chunk.  Every ReadChunkHeader
		//	must be matched by a FinishChunk.
		//--------------------------------------------------------------------
		virtual void FinishChunk() = 0;

		//--------------------------------------------------------------------
		//	Read functions - These will be overridden to read the data 
		//	given in the appropriate format.
		//	It would have been nice to make these template member functions,
		//	but I think there would be problems getting them instantiated
		//	so that they are part of the abstract base interface.
		//--------------------------------------------------------------------
		virtual void Read(int8_t& o_Val) = 0;
		virtual void Read(uint8_t& o_Val) = 0;
		virtual void Read(int16_t& o_Val) = 0;
		virtual void Read(uint16_t& o_Val) = 0;
		virtual void Read(int32_t& o_Val) = 0;
		virtual void Read(uint32_t& o_Val) = 0;
		virtual void Read(int64_t& o_Val) = 0;
		virtual void Read(uint64_t& o_Val) = 0;
		virtual void Read(float& o_Val) = 0;
		virtual void Read(double& o_Val) = 0;

		//--------------------------------------------------------------------
		//	This Read reads a string called either "TRUE" or "FALSE" and 
		//  converts it into a bool value.
		//--------------------------------------------------------------------
		virtual void Read(bool& o_Val) = 0;
		
		//--------------------------------------------------------------------
		//	This Read reads a NULL-terminated single-byte character string:
		//	the bytes as written, UTF-8 in files written now. It is for strings
		//	that were always written as single bytes, and every new field; a
		//	string that binary files written before PWFB stored as UTF-16 is
		//	read with ReadLegacyWideString.
		//--------------------------------------------------------------------
		virtual void Read(std::string& o_Val) = 0;

		//--------------------------------------------------------------------
		//	This Read Reads a single-byte character string with a length
		//	given by i_Length.
		//--------------------------------------------------------------------
		virtual void Read(std::string& o_Val, int i_Length) = 0;

		//--------------------------------------------------------------------
		//	ReadLegacyWideString reads a NULL-terminated string, as UTF-8,
		//	from a field that held an itString in binary files written before
		//	the PWFB signature (itString wasn't portable UTF-8 but 16-bit
		//	wide characters, so those files store the field as UTF-16, which
		//	this converts). In PWFB files, and in text and XML files, it is
		//	the same as Read (gfFileBin::HasUTF8Strings).
		//
		//	Only for those fields. Nothing in an old file says which strings
		//	are UTF-16, so each such field's parser says so by calling this.
		//	New fields always use Read: no file written before PWFB has them.
		//	Don't change an existing call to Read either: old files would
		//	misread that field and everything after it.
		//--------------------------------------------------------------------
		virtual void ReadLegacyWideString(std::string& o_Val) = 0;

		//--------------------------------------------------------------------
		//	Read in an envAppVersion structure
		//--------------------------------------------------------------------
		virtual void Read(envAppVersion& o_Val) = 0;

		//--------------------------------------------------------------------
		//	Read chunk of binary data; this is only for
		//	the implementations, it is not a function of chReader base class
		//--------------------------------------------------------------------
		virtual	void Read(void* i_Data, int64_t i_NumBytes) = 0;

		//--------------------------------------------------------------------
		// This function asks if it is safe to read binary data in a 
		// single step with this reader. This applies to the implementation
		// in the function above Read(void*,int). Binary file formats
		// should return true and ascii file formats should return false.
		//--------------------------------------------------------------------
		virtual bool CanReadBinaryData() const = 0;

		//------------------------------------------------------------------------
		//	GetLocator returns this file's associated locator
		//------------------------------------------------------------------------
		virtual const fsLocator& GetLocator() const = 0;

		//------------------------------------------------------------------------
		//	GetFilePos returns the current position of the file pointer.
		//------------------------------------------------------------------------
		virtual fsFileStream::FilePosType GetFilePos() const = 0;

		//------------------------------------------------------------------------
		//	A flag that code can set if the file is newer
		//------------------------------------------------------------------------
		virtual const bool IsNewerVersion() const;
		virtual void SetNewerVersion(const bool i_bIsNewerFlag);

	private:
		bool m_bNewerVersion;
};

