/****************************************************************************\
**  g2dOpenEXROutStream.hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dOpenEXROutStream.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"

//--------------------------------------------------------------------
// g2dOpenEXROutStream()
//--------------------------------------------------------------------
g2dOpenEXROutStream::g2dOpenEXROutStream(const char fileName[],const fsLocator& i_Locator)
:
	Imf::OStream(fileName)
{
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( i_Locator, itFileName );
	m_OutStream = new std::ofstream( itFileName.GetString() , std::ios::binary );
}

//--------------------------------------------------------------------
// ~g2dOpenEXROutStream()
//--------------------------------------------------------------------
g2dOpenEXROutStream::~g2dOpenEXROutStream()
{
	m_OutStream->close();
	delete m_OutStream;
	m_OutStream = NULL;
}

//--------------------------------------------------------------------
// write()
//--------------------------------------------------------------------
void g2dOpenEXROutStream::write(const char c[/*n*/], int n)
{
	m_OutStream->write(c,n);
}

//--------------------------------------------------------------------
// tellp()
//--------------------------------------------------------------------
uint64_t g2dOpenEXROutStream::tellp()
{
	return m_OutStream->tellp();
}

//--------------------------------------------------------------------
// seekp()
//--------------------------------------------------------------------
void g2dOpenEXROutStream::seekp(uint64_t pos)
{
	m_OutStream->seekp(pos);
}
