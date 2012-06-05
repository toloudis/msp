/*****************************************************************************
**  gfFileBin.cpp
**
**      gfFileBin is a class which inherits from fsFileStream and contains
**		a data member tracking endian mode, along with accessor methods for it,
**		as well as a method for writing binary headers
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/gf/gfFileBin.hpp"

#include "Core/dbg/dbgLog.hpp"
//#include "Core/env/envPackageErrorIndices.hpp"
#include "Core/fs/fsFilePosSaver.hpp"
//#include "Core/gf/gfErrorCodes.hpp"
#include "Core/gf/gfFileX.hpp"
//#include "Core/gf/gfPaths.hpp"
//#include "Core/gf/gfPakFile.hpp"
//#include "Core/gf/gfFileTranslationMgr.hpp"

//========================================================================
//	Constructor
//========================================================================
gfFileBin::gfFileBin(const fsLocator& i_Locator, fsFileStream::AccessType i_DesiredAccess, EndianMode i_Mode)
:	m_Mode(i_Mode),
	m_FileStream(NULL)
{
	m_FileStream = new fsFileStream(i_Locator, i_DesiredAccess);
}

//========================================================================
//========================================================================
gfFileBin::~gfFileBin()
{
	delete m_FileStream;
}

//========================================================================
//	Read reads i_NumBytes into the buffer.  If the file is too short 
//	to read	i_NumBytes, it will read as many as it can.  The return
//	value is the number actually read into the o_Buffer.  This function
//	also advances the file pointer.
//========================================================================
int gfFileBin::Read(int i_NumBytes, void* o_Buffer)
{
	return m_FileStream->Read(i_NumBytes, o_Buffer);
}

//========================================================================
//	Write writes i_NumBytes from o_Buffer into the file.  The file 
//	pointer will also be advanced.
//========================================================================
void gfFileBin::Write(int i_NumBytes, const void* i_Buffer)
{
	DBG_ASSERT0(m_FileStream != NULL, "Must have a fsFileStream to write");
	m_FileStream->Write(i_NumBytes, i_Buffer);
}

//========================================================================
//	GetFilePos returns the current position of the file pointer.
//========================================================================
int gfFileBin::GetFilePos() const
{
	return m_FileStream->GetFilePos();
}

//========================================================================
//	GetFileSize returns the size of the file in bytes.
//========================================================================
int gfFileBin::GetFileSize() const
{

	// Old way was to seek to end of file and determine offset from there
	//fsFilePosSaver<fsFileStream> saver(*m_FileStream);
	//m_FileStream->SetFilePos(0, fsFileStream::e_End);
	//return m_FileStream->GetFilePos();

	// Trying to just use the windows API function now
	return m_FileStream->GetFileSize();
	
}


//========================================================================
//	SetFilePos sets the file pointer to the given value.  It is valid
//	to move the file pointer beyond the end of the file.
//========================================================================
void gfFileBin::SetFilePos( int i_Pos, fsFileStream::AccessPointType i_DesiredAccessPoint)
{
	m_FileStream->SetFilePos(i_Pos, i_DesiredAccessPoint);
}

//========================================================================
//	GetLocator returns this file's associated locator
//========================================================================
const fsLocator& gfFileBin::GetLocator() const
{	
	return m_FileStream->GetLocator();
}

//========================================================================
//	ReadHeader reads the file's header into the o_Header passed to it. If 
//	o_Header is NULL, the file pointer is simply moved past the file header
//	Throws: gfInvalidFileBinX
//========================================================================
void gfFileBin::ReadHeader(gfFileBin::Header *o_Header)
{
	//move file pointer past signature, 4 bytes is the size of the signature
	char Sig[4];
	std::string strGigaSig("GWFB");
	std::string strTeraSig("TWFB");
	this->Read(4, &Sig);
	std::string strReadSig(Sig, 4);
	if ( strGigaSig.compare(strReadSig) && strTeraSig.compare(strReadSig) )
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

//========================================================================
//	WriteHeader writes the given i_Header to the file. gfFileBin::m_Mode
//	is used instead of the value passed in i_Header::m_Mode since a 
//	gfFileBin's mode can't altered after construction. The second version of
//	this function is an inline convenience.
//========================================================================
void gfFileBin::WriteHeader(const gfFileBin::Header& i_Header)
{
	//write signature
	this->Write(4, "TWFB");

	//set header's mode to this filebin's mode
	gfFileBin::Header Header = i_Header;
	Header.m_Mode = m_Mode;

	//write header
	this->Write(sizeof(gfFileBin::Header), &Header);
}
