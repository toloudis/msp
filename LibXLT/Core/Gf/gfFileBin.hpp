/*****************************************************************************
**	gfFileBin.hpp
**
**		gfFileBin is a class which inherits from fsFileStream and contains
**		a data member tracking endian mode, along with accessor methods for it,
**		as well as a method for writing binary headers
**
**		A binary file starts with a four-byte signature and the Header.
**		The signature says how the file stores strings that were itStrings
**		when they were written (chWriter::Write, chReader::ReadItString):
**			PWFB (petawatt)	UTF-8, null-terminated; every file written now
**			TWFB (terawatt)	UTF-16LE, null-terminated 16-bit units
**			GWFB (gigawatt)	UTF-16LE, as TWFB
**		Narrow strings are single bytes in all three.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GF_FILEBIN_HPP
#error gfFileBin.hpp multiply included
#endif
#define GF_FILEBIN_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/Env/envExceptionX.hpp"
#endif
#ifndef ENV_PLATFORM_HPP
#include "Core/Env/envPlatform.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/Fs/fsFileStream.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif
#ifndef GF_FILE_HPP
#include "gfFile.hpp"
#endif

#include <memory>


//============================================================================
//============================================================================
class gfFileBin : public gfFile
{
	public:
		//------------------------------------------------------------------------
		//	structs
		//------------------------------------------------------------------------

		//this header is preceded in the file by the four-byte signature:
		//"PWFB", or "TWFB" or "GWFB" in older files (see the top of this file)
		struct Header
		{
			char m_Mode;	// deprecated endian mode; WriteHeader always writes 2 (little endian)
			char m_Major;
			char m_Minor;
			char m_Rev;
		};

		//------------------------------------------------------------------------
		//	Constructor
		//------------------------------------------------------------------------
		gfFileBin(const fsLocator& i_Locator, fsFileStream::AccessType i_DesiredAccess);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		~gfFileBin();

		//------------------------------------------------------------------------
		//	Read reads i_NumBytes into the buffer.  If the file is too short 
		//	to read	i_NumBytes, it will read as many as it can.  The return
		//	value is the number actually read into the o_Buffer.  This function
		//	also advances the file pointer.
		//------------------------------------------------------------------------
		int Read(int64_t i_NumBytes, void* o_Buffer);

		//------------------------------------------------------------------------
		//	Write writes i_NumBytes from o_Buffer into the file.  The file 
		//	pointer will also be advanced.
		//------------------------------------------------------------------------
		void Write(int64_t i_NumBytes, const void* i_Buffer);

		//------------------------------------------------------------------------
		//	ReadHeader reads the file's header into the o_Header passed to it. If 
		//	o_Header is NULL, the file pointer is simply moved past the file header
		//	Throws: gfInvalidFileBinX
		//	ReadHeader accepts the signatures PWFB, TWFB and GWFB, and
		//	remembers which one the file has (HasUTF8Strings).
		//------------------------------------------------------------------------
		void ReadHeader(Header* o_Header = NULL);

		//------------------------------------------------------------------------
		//	WriteHeader writes the given i_Header to the file. i_Header::m_Mode
		//	is ignored; the deprecated endian mode byte is always written as
		//	little endian. The second version of this function is an inline
		//	convenience.
		//	The signature written is always PWFB: the file's strings are
		//	UTF-8.
		//------------------------------------------------------------------------
		void WriteHeader(const Header& i_Header);
		void WriteHeader(int i_Major = 0, int i_Minor = 0, int i_Rev = 0);

		//------------------------------------------------------------------------
		//	HasUTF8Strings is true if the file stores text strings as UTF-8
		//	(its signature is PWFB), false if as UTF-16 (TWFB or GWFB), and
		//	false until ReadHeader or WriteHeader has been called: a file
		//	without a header (chunks only, from the legacy exporters) is an
		//	old one.
		//------------------------------------------------------------------------
		bool HasUTF8Strings() const;

		static int GetHeaderSize();

	private:
		bool m_bUTF8Strings;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline void gfFileBin::WriteHeader(int i_Major, int i_Minor, int i_Rev)
{
	Header h;
	h.m_Major = i_Major;
	h.m_Minor = i_Minor;
	h.m_Rev = i_Rev;
	WriteHeader(h);
}

