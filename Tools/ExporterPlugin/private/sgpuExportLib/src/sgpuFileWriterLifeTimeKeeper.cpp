/****************************************************************************\
**  sgpuFileWriterLifeTimeKeeper.cpp
**
**      sgpuFileWriterLifeTimeKeeper.hpp defines the sgpuFileWriterLifeTimeKeeper class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuFileWriterLifeTimeKeeper.hpp"
#include "sgpuBaseScene.hpp"
#include "sgpuStringImpl.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"

#undef CreateFile
#undef DeleteFile

#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chBinWriter.hpp"


#include <sstream>
#include <vector>

	sgpuFileWriterLifeTimeKeeper::sgpuFileWriterLifeTimeKeeper( sgpuBaseScene & i_Scene, const sgpuString &i_ExportFilename )
		:m_pScene( &i_Scene ),
		m_pFile(NULL),
		m_pWriter(NULL)
	{
		fsLocator locator;

		int nBytesForName = i_ExportFilename.GetNumBytes_WChart();
		CharacterBuffer< 256 > cbuffer ( nBytesForName );
		wchar_t *pWriteHere = reinterpret_cast< wchar_t * > ( cbuffer.GetBuffer() );
		int nBytesResulted= i_ExportFilename.GetData_WChart( pWriteHere, nBytesForName);
		DBG_ASSERT( nBytesResulted == nBytesForName, "error in getting data from sgpustring" );
		itString itFileName( reinterpret_cast< const itString::CharType * > ( pWriteHere ) );
		fsFileUtil::UnicodeStringToLocator( itFileName,  locator);
		try 
		{
			if( fsFileUtil::FileExists(locator) )
			{
				fsFileUtil::DeleteFile(locator);
			}
			fsFileUtil::CreateFile(locator);
			m_pFile = new gfFileBin (locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
			m_pFile->WriteHeader();		
			m_pWriter = new chBinWriter (*m_pFile);
		}
		catch( ... )
		{
			std::stringstream ss;
			ss << "cant write to file " << i_ExportFilename.m_pImpl->m_Data.c_str();
			throw sgpuException( sgpuString( ss.str().c_str() ) );
		}

	}


	sgpuFileWriterLifeTimeKeeper::~sgpuFileWriterLifeTimeKeeper()
	{
		Cleanup();
	}

	void sgpuFileWriterLifeTimeKeeper::Cleanup()
	{
		delete m_pWriter;
		delete m_pFile;
		m_pFile = NULL;
		m_pWriter = NULL;
		//m_pExportDoc->UnregsiterWriter();
	}