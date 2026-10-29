/*****************************************************************************
**	gfFileBin.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/Gf/gfFileBin.hpp"

#include "Core/Fs/fsFilePosSaver.hpp"
#include "Core/Gf/gfFileX.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/Gf/gfFileTranslationMgr.hpp"


//------------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------------
gfFileBin::gfFileBin(const fsLocator& i_Locator, fsFileStream::AccessType i_DesiredAccess)
:	gfFile(i_Locator, i_DesiredAccess),
	m_bUTF8Strings(false)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
gfFileBin::~gfFileBin()
{
}

//------------------------------------------------------------------------
//	Read reads i_NumBytes into the buffer.  If the file is too short 
//	to read	i_NumBytes, it will read as many as it can.  The return
//	value is the number actually read into the o_Buffer.  This function
//	also advances the file pointer.
//------------------------------------------------------------------------
int gfFileBin::Read(int64_t i_NumBytes, void* o_Buffer)
{
	return m_FileStream->Read(i_NumBytes, o_Buffer);
}

//------------------------------------------------------------------------
//	Write writes i_NumBytes from o_Buffer into the file.  The file 
//	pointer will also be advanced.
//------------------------------------------------------------------------
void gfFileBin::Write(int64_t i_NumBytes, const void* i_Buffer)
{
	DBG_ASSERT(m_FileStream != NULL, "Must have a fsFileStream to write");
	m_FileStream->Write(i_NumBytes, i_Buffer);
}

//------------------------------------------------------------------------
//	ReadHeader reads the file's header into the o_Header passed to it. If
//	o_Header is NULL, the file pointer is simply moved past the file header
//	The signature is PWFB (strings as UTF-8), or TWFB or GWFB (strings as
//	UTF-16) in older files.
//	Throws: gfInvalidFileBinX
//------------------------------------------------------------------------
void gfFileBin::ReadHeader(gfFileBin::Header *o_Header)
{
	//move file pointer past signature, 4 bytes is the size of the signature
	char Sig[4];
	this->Read(4, &Sig);
	const std::string strReadSig(Sig, 4);
	if ( strReadSig == "PWFB" )
	{
		m_bUTF8Strings = true;
	}
	else if ( strReadSig == "TWFB" || strReadSig == "GWFB" )
	{
		m_bUTF8Strings = false;
	}
	else
	{
		//not a gfFileBin, throw gfInvalidFileBinX
		throw gfInvalidFileBinX(GetLocator());
	}


	//now actually read the header
	if (NULL != o_Header)
	{
		this->Read(sizeof(gfFileBin::Header), o_Header);
	}
	else
	{
		//NULL was passed in, so just move the filepointer
		this->SetFilePos(sizeof(gfFileBin::Header), fsFileStream::e_Current);
	}
}

//------------------------------------------------------------------------
//	WriteHeader writes the given i_Header to the file. i_Header::m_Mode
//	is ignored; the deprecated endian mode byte is always written as
//	little endian. The second version of this function is an inline
//	convenience.
//	The signature is always PWFB: chWriter writes every string as UTF-8.
//	(TWFB and GWFB files stored itStrings as UTF-16.)
//------------------------------------------------------------------------
void gfFileBin::WriteHeader(const gfFileBin::Header& i_Header)
{
	//write signature
	this->Write(4, "PWFB");
	m_bUTF8Strings = true;

	//	m_Mode is a deprecated endian mode (0 native, 1 big, 2 little endian).
	//	Files are always little endian and nothing reads this byte any more;
	//	2 is written so files keep the same bytes as before.
	gfFileBin::Header Header = i_Header;
	Header.m_Mode = 2;

	//write header
	this->Write(sizeof(gfFileBin::Header), &Header);
	DBG_ASSERT( GetFilePos() == GetHeaderSize(), "Inconsistency in writing header" );
}
//------------------------------------------------------------------------
//	HasUTF8Strings is true if the file stores text strings as UTF-8
//	(see the .hpp)
//------------------------------------------------------------------------
bool gfFileBin::HasUTF8Strings() const
{
	return m_bUTF8Strings;
}

int  gfFileBin::GetHeaderSize()
{
	return ( 4 + sizeof(gfFileBin::Header) ) ;
}