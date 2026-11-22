/*****************************************************************************
**	chBinReader.hpp
**
**		chBinReader defines the chBinReader class, which is a chReader that
**	operates on binary chunk files.
**
**	StudioGPU
**	Copyright(C) 2003-10 - All Rights Reserved
\****************************************************************************/
#ifdef CH_BINREADER_HPP
#error chBinReader.hpp multiply included
#endif
#define CH_BINREADER_HPP

#ifndef CH_READER_HPP
#include "Core/ch/chReader.hpp"
#endif

#include <stack>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class gfFileBin;


//============================================================================
//	
//============================================================================
class chBinReader : public chReader
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chBinReader(gfFileBin& io_File);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chBinReader();

		//--------------------------------------------------------------------
		//	ReadChunkHeader reads a chunk header in the file at the
		//	current position.  If there is a problem with the header,
		//	it will throw a chInvalidChunkX.  Returns true if more chunks to 
		//	read, false if no more.
		//	The size is the size of the chunk not including the header.
		//--------------------------------------------------------------------
		virtual bool ReadChunkHeader(	chDefs::Name& o_Name, 
										chDefs::Version& o_Version,
										chDefs::Size& o_Size);

		//--------------------------------------------------------------------
		//	FinishChunk will cause the reader to advance to the next position
		//	after the end of the current chunk.  Every ReadChunkHeader
		//	must be matched by a FinishChunk.
		//--------------------------------------------------------------------
		virtual void FinishChunk();

		//--------------------------------------------------------------------
		//	Read functions - These will be overridden to read the data 
		//	given in the appropriate format.
		//	It would have been nice to make these template member functions,
		//	but I think there would be problems getting them instantiated
		//	so that they are part of the abstract base interface.
		//--------------------------------------------------------------------
		virtual void Read(int8_t& o_Val);
		virtual void Read(uint8_t& o_Val);
		virtual void Read(int16_t& o_Val);
		virtual void Read(uint16_t& o_Val);
		virtual void Read(int32_t& o_Val);
		virtual void Read(uint32_t& o_Val);
		virtual void Read(int64_t& o_Val);
		virtual void Read(uint64_t& o_Val);
		virtual void Read(float& o_Val);
		virtual void Read(double& o_Val);

		//--------------------------------------------------------------------
		//	This Read reads a string called either "TRUE" or "FALSE" and 
		//  converts it into a bool value.
		//--------------------------------------------------------------------
		virtual void Read(bool& o_Val);

		//--------------------------------------------------------------------
		//	This Read reads a NULL-terminated single-byte character string.
		//--------------------------------------------------------------------
		virtual void Read(std::string& o_Val);

		//--------------------------------------------------------------------
		//	This Read Reads a single-byte character string with a length
		//	given by i_Length.
		//--------------------------------------------------------------------
		virtual void Read(std::string& o_Val, int i_Length);

		//--------------------------------------------------------------------
		//	ReadLegacyWideString reads a NULL-terminated string as UTF-8: stored as UTF-8
		//	in PWFB files, and as UTF-16, which it converts, in older ones and
		//	in files whose header wasn't read (gfFileBin::HasUTF8Strings).
		//--------------------------------------------------------------------
		virtual void ReadLegacyWideString(std::string& o_Val);

		using chReader::Read;
		using chReader::ReadLegacyWideString;

		//--------------------------------------------------------------------
		//	Read in an envAppVersion structure
		//--------------------------------------------------------------------
		virtual void Read(envAppVersion& o_Val);

		//--------------------------------------------------------------------
		//	Read chunk of binary data; this is only for
		//	the implementations, it is not a function of chReader base class
		//--------------------------------------------------------------------
		virtual	void Read(void* i_Data, int64_t i_NumBytes);

		//--------------------------------------------------------------------
		// This function asks if it is safe to read binary data in a 
		// single step with this reader. This applies to the implementation
		// in the function above Read(void*,int). Binary file formats
		// should return true and ascii file formats should return false.
		//--------------------------------------------------------------------
		virtual bool CanReadBinaryData() const { return true; }

		//------------------------------------------------------------------------
		//	GetLocator returns this file's associated locator
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

		//------------------------------------------------------------------------
		//	GetFilePos returns the current position of the file pointer.
		//------------------------------------------------------------------------
		fsFileStream::FilePosType GetFilePos() const;

	private:

		gfFileBin& m_File;
		int64_t m_FileLength;
		std::stack<fsFileStream::FilePosType, std::vector<fsFileStream::FilePosType> > m_ChunkEnds;

};

