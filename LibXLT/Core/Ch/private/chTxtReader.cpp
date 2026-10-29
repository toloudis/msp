/*****************************************************************************
**  chTxtReader.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/ch/chTxtReader.hpp"
#include "Core/env/envNumberText.hpp"

#include "Core/ch/chExceptionX.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/it/itStringUtil.hpp"


//============================================================================
//	
//============================================================================
namespace
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class T>
bool read_number(gfFileTxt &i_File, T &o_Val)
{
	std::string tag;
	if (!i_File.ReadToken(tag))
	{
		o_Val = 0;
		return false;
	}

	if (!envNumberText::FromText(tag, o_Val))
		o_Val = 0;
	return true;
}

//----------------------------------------------------------------------------
//	Int8 and UInt8 were once written as raw characters (by chTxtWriter until 2026),
//	so a one-character token that isn't a number is taken as that byte.
//	Bytes that are whitespace, digits, '"' or '#' can't be recovered.
//----------------------------------------------------------------------------
template<class T>
bool read_byte(gfFileTxt &i_File, T &o_Val)
{
	std::string tag;
	if (!i_File.ReadToken(tag))
	{
		o_Val = 0;
		return false;
	}

	if (!envNumberText::FromText(tag, o_Val))
		o_Val = (tag.length() == 1) ? T(tag[0]) : 0;
	return true;
}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chTxtReader::chTxtReader(gfFileTxt& io_File)
:	m_File(io_File)
{
	m_pMap = NULL;
	m_NumContainers = 0;
	m_bCurrentContainer = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chTxtReader::~chTxtReader()
{
}


//--------------------------------------------------------------------
//	Ascii Writer and Reader need map from string to chDefs::Name
//  in order to read and write useful chunk names.
//--------------------------------------------------------------------
void	
chTxtReader::SetMap(std::map<std::string, chDefs::Name > *i_pMap)
{
	m_pMap = i_pMap;
}

std::map<std::string, chDefs::Name>*	
chTxtReader::GetMap() const
{
	return m_pMap;
}

//--------------------------------------------------------------------
//	ReadChunkHeader reads a chunk header in the file at the
//	current position.  If there is a problem with the header,
//	it will throw a chInvalidChunkX.  Returns true if more chunks to 
//	read, false if no more.
//	The size is the size of the chunk not including the header.
//--------------------------------------------------------------------
bool chTxtReader::ReadChunkHeader(	chDefs::Name& o_Name, 
									chDefs::Version& o_Version,
									chDefs::Size& o_Size)
{
	std::string Token;

	fsFileStream::FilePosType cur_pos = m_File.GetFilePos();
	if (!m_File.ReadToken(Token))
	{
		// hit end of file
		return false;
	}

//	DBG_LOG("ReadChunkHeader: read " << Token.c_str());

	if (Token == "}")
	{
		// Back up to before "}" was read, let FinishHeader 
		// handle it
		m_File.SetFilePos(cur_pos, fsFileStream::e_Beginning);
		return false;
	}

	// Convert from Token to header
	int len = Token.length();
	o_Name = chDefs::MakeName((len > 0) ? Token[0] : '\0', (len > 1) ? Token[1] : '\0',
		(len > 2) ? Token[2] : '\0', (len > 3) ? Token[3] : '\0');
	o_Version = 0;
	o_Size = 0;

	if (m_pMap)
	{
		// use std::map from string to i_Name
		// to map useful name to chDefs::Name.
		// This is a case-sensitive compare
		//
		std::map<std::string, chDefs::Name>::iterator it = 
					m_pMap->find(Token);
		if (it != m_pMap->end())
		{
			o_Name = (*it).second;
		}
	}

	// See if this chunk is a container
	cur_pos = m_File.GetFilePos();
	bool bContainer = true;
	if (!m_File.ReadToken(Token) || (Token != "{"))
	{
		// Not a container, back up to where we were
		m_File.SetFilePos(cur_pos, fsFileStream::e_Beginning);
		bContainer = false;
	}
//	DBG_LOG("ReadChunkHeader: read(2) " << Token.c_str());

	m_NumContainers++;
	m_bCurrentContainer = bContainer;
	return true;
}

//--------------------------------------------------------------------
//	FinishChunk will cause the reader to advance to the next position
//	after the end of the current chunk.
//--------------------------------------------------------------------
void chTxtReader::FinishChunk()
{
	DBG_ASSERT(m_NumContainers > 0, "Finished a chunk that wasn't started");
	
	if (m_bCurrentContainer)
	{
		// Skip along nested chunks until we reach
		// our matching "}"
		//
		std::string Token;
		int depth = 1;
		while (m_File.ReadToken(Token))
		{
//			DBG_LOG("FinishChunk: read " << Token.c_str());

			if (Token == "}")
			{
				depth--;
				if (depth <= 0) break;
			}
			else if (Token == "{")
			{
				depth++;
			}
		}
	}

	m_NumContainers--;
	m_bCurrentContainer = true;
}

//--------------------------------------------------------------------
//	Read functions - These will be overridden to read the data 
//	given in the appropriate format.
//	It would have been nice to make these template member functions,
//	but I think there would be problems getting them instantiated
//	so that they are part of the abstract base interface.
//--------------------------------------------------------------------
void chTxtReader::Read(int8_t& o_Val)
{
	read_byte<int8_t>(m_File, o_Val);
}

void chTxtReader::Read(uint8_t& o_Val)
{
	read_byte<uint8_t>(m_File, o_Val);
}

void chTxtReader::Read(int16_t& o_Val)
{
	read_number<int16_t>(m_File, o_Val);
}

void chTxtReader::Read(uint16_t& o_Val)
{
	read_number<uint16_t>(m_File, o_Val);
}

void chTxtReader::Read(int32_t& o_Val)
{
	read_number<int32_t>(m_File, o_Val);
}

void chTxtReader::Read(uint32_t& o_Val)
{
	read_number<uint32_t>(m_File, o_Val);
}

void chTxtReader::Read(int64_t& o_Val)
{
	read_number<int64_t>(m_File, o_Val);
}

void chTxtReader::Read(uint64_t& o_Val)
{
	read_number<uint64_t>(m_File, o_Val);
}

void chTxtReader::Read(float& o_Val)
{
	read_number<float>(m_File, o_Val);
}

void chTxtReader::Read(double& o_Val)
{
	read_number<double>(m_File, o_Val);
}

//--------------------------------------------------------------------
//	This Read reads a string called either "TRUE" or "FALSE" and 
//  converts it into a bool value.
//--------------------------------------------------------------------
void chTxtReader::Read(bool& o_Val)
{
	// Read the bool value from the file as a string.
	std::string BoolString;
	Read(BoolString);

	// Convert the string into a bool and return.
	itStringUtil::ToLower( BoolString );
	o_Val = ( BoolString == "true" );
}

//--------------------------------------------------------------------
//	This Read reads a NULL-terminated single-byte character string.
//--------------------------------------------------------------------
void chTxtReader::Read(std::string& o_String)
{
	m_File.ReadToken( o_String );
}

//--------------------------------------------------------------------
//	This Read reads a single-byte character string with a length
//	given by i_Length.
//--------------------------------------------------------------------
void chTxtReader::Read(std::string& o_String, int i_Length)
{
	m_File.ReadToken( o_String, i_Length );
}

//--------------------------------------------------------------------
//	ReadItString reads a string as UTF-8, which is how text and XML
//	files store every string: the same as Read.
//--------------------------------------------------------------------
void chTxtReader::ReadItString(std::string& o_Val)
{
	Read(o_Val);
}

//--------------------------------------------------------------------
//	Read in an envAppVersion structure
//--------------------------------------------------------------------
void chTxtReader::Read(envAppVersion& o_Val)
{
	DBG_ASSERT( false, "application version read functionality not implemented yet");
}

//------------------------------------------------------------------------
//	GetLocator returns this file's associated locator
//------------------------------------------------------------------------
const fsLocator& chTxtReader::GetLocator() const
{
	return m_File.GetLocator();
}

//------------------------------------------------------------------------
//	GetFilePos returns the current position of the file pointer.
//------------------------------------------------------------------------
fsFileStream::FilePosType chTxtReader::GetFilePos() const
{
	return m_File.GetFilePos();
}
