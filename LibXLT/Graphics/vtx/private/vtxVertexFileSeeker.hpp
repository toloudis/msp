/*****************************************************************************
**	vtxVertexFileSeeker.hpp
**
**		vtxVertexFileSeeker - multithread safe wrapper for seeking through
**	a file for animation data
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXFILESEEKER_HPP
#error vtxVertexFileSeeker.hpp multiply included
#endif
#define VTX_VERTEXFILESEEKER_HPP

#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/Fs/fsFileStream.hpp"
#endif 
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 


//============================================================================
//============================================================================
class chBinReader;
class fsLocator;
class gfFileBin;


//============================================================================
// Wrapper for seeking through a file with vertex animation data.
//============================================================================
class vtxVertexFileSeeker
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	vtxVertexFileSeeker(const fsLocator &i_Locator);

	//--------------------------------------------------------------------
	// Attach and detach from the file seeker. When the count of 
	// attachers reaches zero the file is closed, when more attach
	// again the file is reopened.
	//--------------------------------------------------------------------
	void Attach();
	void Detach();

	//--------------------------------------------------------------------
	// Move the file to the given position
	//--------------------------------------------------------------------
	void SetFilePos(fsFileStream::FilePosType i_FilePos);

	//--------------------------------------------------------------------
	// Access to the binary reader
	//--------------------------------------------------------------------
	chBinReader& GetReader();

	//--------------------------------------------------------------------
	// Use mutex when multiple threads are accessing this file seeker
	//	at the same time.
	//--------------------------------------------------------------------
	envMutex& GetMutex();

private:
	fsLocator m_Locator;
	int m_AttachCount;
	shared_ptr<gfFileBin> m_File;
	shared_ptr<chBinReader> m_Reader;
	envMutex m_Mutex;
};

