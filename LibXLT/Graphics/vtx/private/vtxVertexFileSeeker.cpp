/*****************************************************************************
**  vtxVertexFileSeeker.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/private/vtxVertexFileSeeker.hpp"

#include "Core/Ch/chBinReader.hpp"
#include "Core/Gf/gfFileBin.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxVertexFileSeeker::vtxVertexFileSeeker(const fsLocator &i_Locator)
:	m_Locator(i_Locator), m_AttachCount(0)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void vtxVertexFileSeeker::Attach()
{
	if (m_AttachCount == 0)
	{
		// Open the file handles
		DBG_ASSERT(!m_File, "File should not already be open");
		if (m_File)
			return;
		m_File.reset(new gfFileBin(m_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian));
		m_Reader.reset(new chBinReader(*m_File));
	}
	m_AttachCount++;

	//DBG_LOG("FileSeeker attach, count: " << m_AttachCount << " " << m_Locator);

}
void vtxVertexFileSeeker::Detach()
{
	m_AttachCount--;
	if (m_AttachCount == 0)
	{
		// Close the file handles
		m_Reader.reset();
		m_File.reset();
	}

	//DBG_LOG("FileSeeker detach, count: " << m_AttachCount << " " << m_Locator);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void vtxVertexFileSeeker::SetFilePos(fsFileStream::FilePosType i_FilePos)
{
	DBG_ASSERT(m_Reader.get(), "Must attach to the file seeker before setting the file position.");
	if (!m_Reader.get())
		return;
	m_File->SetFilePos(i_FilePos);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chBinReader& vtxVertexFileSeeker::GetReader()
{
	DBG_ASSERT(m_Reader.get(), "Must attach to the file seeker before using the Reader");
	return (*m_Reader);
}

//--------------------------------------------------------------------
// Use mutex when multiple threads are accessing this file seeker
//	at the same time.
//--------------------------------------------------------------------
envMutex& vtxVertexFileSeeker::GetMutex()
{
	return m_Mutex;
}
