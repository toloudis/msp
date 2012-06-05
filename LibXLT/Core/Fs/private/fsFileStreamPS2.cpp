/**********************************************************
**  fsFileStreamPS2.cpp
**
**      This is the PS2 version of the fsFileStream
**	implementation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include "fsFileStream.hpp"

#include <eekernel.h>
#include <libcdvd.h>
#include <sifdev.h>

#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "envErrorDisplayForwarder.hpp"
#include "envPackageErrorIndices.hpp"
#include "envPool.hpp"
#include "fsErrorCodes.hpp"
#include "fsFileUtilPAC.hpp"
#include "fsFileX.hpp"
#include "maFunctions.hpp"

#include "appApplicationPAC.hpp"

#include <malloc.h>

#include "MultiStream.h"

namespace
{
	const itString lc_CDRomString("cdrom0:");
	const int lc_CacheSize = 256;

	void handle_file_error(int i_ErrorNumber, const fsLocator& i_Locator)
	{
		switch( i_ErrorNumber )
		{	
			case -2:	//	not sure what -2 is supposed to mean, but that's what comes out when the file is not there
				throw fsFileDoesntExistX(i_Locator);
			break;
			case -SCE_ENXIO:		// No such device or address
				DBG_LOG0("SCE_ENXIO");
			break;
			case -SCE_EBADF:		// Bad file number 
				DBG_LOG0("SCE_EBADF");
			break;
			case -SCE_ENODEV:		// No such device 
				DBG_LOG0("SCE_ENODEV");
				throw fsInvalidLocatorX(i_Locator);
			break;
			case -SCE_EINVAL:		// Invalid argument 
				DBG_LOG0("SCE_EINVAL");
			break;
			case -SCE_EMFILE:		// Too many open files 
				DBG_LOG0("SCE_EMFILE");
			break;
			default:
				throw fsUnknownX(i_Locator);
			break;
		}
		
		throw fsUnknownX(i_Locator);
	}
}

//========================================================================
//	The base file stream implemenation, providing a common interface to
//	CD and host files.
//========================================================================
class fsFileStreamImp
{
	public:

		//========================================================================
		//========================================================================
		fsFileStreamImp(const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~fsFileStreamImp() = 0;

		//========================================================================
		//========================================================================
		virtual int Read(int i_NumBytes, void* o_Buffer) = 0;

		//========================================================================
		//========================================================================
		virtual void Write(int i_NumBytes, const void* i_Buffer) = 0;

		//========================================================================
		//========================================================================
		virtual void SetFilePos(int i_Pos, fsFileStream::AccessPointType i_DesiredAccessPoint) = 0;

		//========================================================================
		//========================================================================
		int GetFilePos() const { return m_VirtualFilePointer; }

		//========================================================================
		//========================================================================
		const fsLocator& GetLocator() const { return m_Locator; }

	protected:

		void SetVirtualFilePointer(int i_Pos) { m_VirtualFilePointer = i_Pos; }
		void IncVirtualFilePointer(int i_Num) { m_VirtualFilePointer += i_Num; } 
	
	private:

		int m_VirtualFilePointer;
		fsLocator m_Locator;
};

//========================================================================
//========================================================================
fsFileStreamImp::fsFileStreamImp(const fsLocator& i_Locator)
:	m_Locator(i_Locator),
	m_VirtualFilePointer(0)
{
}

//========================================================================
//========================================================================
fsFileStreamImp::~fsFileStreamImp()
{
}

//========================================================================
//	This is the file stream implementation that handles networked host
//	file access (on TOOL only).
//========================================================================
class fsHostFileStream : public fsFileStreamImp
{
	public:

		//========================================================================
		//========================================================================
		fsHostFileStream(fsFileStream::AccessType i_DesiredAccess, const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~fsHostFileStream();

		//========================================================================
		//========================================================================
		virtual int Read(int i_NumBytes, void* o_Buffer);

		//========================================================================
		//========================================================================
		virtual void Write(int i_NumBytes, const void* i_Buffer);

		//========================================================================
		//========================================================================
		virtual void SetFilePos(int i_Pos, fsFileStream::AccessPointType i_DesiredAccessPoint);

	private:

		int m_FileHandle;
		fsFileStream::AccessType m_Access;
		int m_RealFilePointer;
		int m_CacheLoc;
		int m_CacheCount;
		envType::Int8 m_ReadCache[lc_CacheSize];
};

fsHostFileStream::fsHostFileStream(fsFileStream::AccessType i_DesiredAccess, const fsLocator& i_Locator)
:	m_RealFilePointer(0),
	m_CacheLoc(0),
	m_CacheCount(0),
	m_Access(i_DesiredAccess),
	fsFileStreamImp(i_Locator)
{
	int flags;
	unsigned short access_mode;

	switch ( i_DesiredAccess )
	{
		case fsFileStream::e_ReadOnly:
			flags = SCE_RDONLY;
			access_mode = 0;
		break;

		case fsFileStream::e_WriteOnly:
			flags = SCE_WRONLY;
			access_mode = 0;
		break;

		case fsFileStream::e_ReadWrite:
			flags = SCE_RDWR;
			access_mode = 0;
		break;

		default:
			DBG_ASSERT0(false, "Invalid Access type");
		break;
	}

	std::string filename;
	fsFileUtilPAC::LocatorToANSIFilename(i_Locator, filename);
	
	//	use sony functions for non-critical host file access
	m_FileHandle = sceOpen(filename.c_str(), flags);

	if ( m_FileHandle < 0 )
		handle_file_error(m_FileHandle, i_Locator); // this will end up throwing, and abort construction
}

//========================================================================
//========================================================================
fsHostFileStream::~fsHostFileStream()
{
	::sceClose(m_FileHandle);
}

//========================================================================
//========================================================================
int fsHostFileStream::Read(int i_NumBytes, void* o_Buffer)
{
	DBG_ASSERT0(m_Access != fsFileStream::e_WriteOnly, "Wrong access type for Read");

	//	if the area to be read is in the cache, we can just memcpy it
	int cache_offset = this->GetFilePos() - m_CacheLoc;
	int total_read = 0;

	if( (cache_offset < m_CacheCount) && (cache_offset >= 0) )
	{
		int num_in_cache = maFunctions::Lowest(m_CacheCount - cache_offset, i_NumBytes);
		memcpy(o_Buffer, m_ReadCache + cache_offset, num_in_cache);
		total_read += num_in_cache;
		this->IncVirtualFilePointer(num_in_cache);
		
		if( i_NumBytes == num_in_cache )
		{
			//	done with read operation
			return total_read;
		}
		else
		{
			//	partial cache hit
			i_NumBytes -= num_in_cache;
			o_Buffer = ((envType::UInt8*)o_Buffer) + num_in_cache;
		}
	}

	//	at this point we have to read data that's not in the cache
	//	if the amount requested is less than the cache size, recache at new location
	if( i_NumBytes <= lc_CacheSize )
	{
		//	recache
		int num_read = 0;
		num_read = sceRead(m_FileHandle, m_ReadCache, lc_CacheSize);

		if ( num_read < 0 )
		{
			// handle failure
			handle_file_error(num_read, this->GetLocator());
		}
		else
		{
			int num_copied = maFunctions::Lowest(i_NumBytes, num_read);
			memcpy(o_Buffer, m_ReadCache, num_copied);
			m_CacheLoc = m_RealFilePointer;
			m_CacheCount = num_read;
			m_RealFilePointer += num_read;
			this->IncVirtualFilePointer(num_copied);
			total_read += num_copied;
		}
	}
	else
	{
		total_read = sceRead(m_FileHandle, o_Buffer, i_NumBytes);

		if ( total_read < 0 )
		{
			// handle failure
			handle_file_error(total_read, this->GetLocator());
		}
		else
		{
			m_RealFilePointer += total_read;
			this->IncVirtualFilePointer(total_read);
		}
	}
	
	return total_read;
}

//========================================================================
//========================================================================
void fsHostFileStream::Write(int i_NumBytes, const void* i_Buffer)
{
	DBG_ASSERT0(m_Access != fsFileStream::e_ReadOnly, "Wrong access type for Write");

	int num_written = 0;
	num_written = sceWrite(m_FileHandle, i_Buffer, i_NumBytes);

	if ( num_written < 0 )
	{
		// handle failure
		handle_file_error(num_written, this->GetLocator());
	}
	else
	{
		m_RealFilePointer += num_written;
		this->IncVirtualFilePointer(num_written);
	}
}

//========================================================================
//========================================================================
void fsHostFileStream::SetFilePos(int i_Pos, fsFileStream::AccessPointType i_DesiredAccessPoint)
{
	int ret_val = 0;
	int whence;
	int seek_to;

	if( i_DesiredAccessPoint == fsFileStream::e_Beginning )
	{
		DBG_ASSERT0( i_Pos >= 0,  "Negative seek attempted" );
		seek_to = i_Pos;
	}
	else if( i_DesiredAccessPoint == fsFileStream::e_Current )
	{
		DBG_ASSERT0( this->GetFilePos() - i_Pos >= 0, "Negative seek attempted" );
		seek_to = i_Pos + this->GetFilePos();
	}
	else
	{
		//	assume not in cache; invalidate cache and move to position
		m_CacheCount = 0;
		seek_to = i_Pos;
		ret_val = sceLseek(m_FileHandle, seek_to, SCE_SEEK_END);
	
		if ( ret_val < 0 )
		{
			// handle failure
			handle_file_error(ret_val, this->GetLocator());
		}
		else
		{
			m_RealFilePointer = ret_val;
			this->SetVirtualFilePointer(ret_val);
		}	
		
		return;
	}

	if( (seek_to >= m_CacheLoc) && (seek_to < (m_CacheLoc + m_CacheCount)) )
	{
		//	in cache; just move pointer
		this->SetVirtualFilePointer(seek_to);
	}
	else
	{
		//	not in cache; invalidate cache and move to position
		m_CacheCount = 0;
		ret_val = sceLseek(m_FileHandle, seek_to, SCE_SEEK_SET);
	
		if ( ret_val < 0 )
		{
			// handle failure
			handle_file_error(ret_val, this->GetLocator());
		}
		else
		{
			m_RealFilePointer = ret_val;
			this->SetVirtualFilePointer(ret_val);
		}	
	}		
}

//========================================================================
//	This is the file stream implementation that handles CD file 
//	access, which will be used for all shipping game file stuff.
//========================================================================
class fsCDFileStream : public fsFileStreamImp
{
	public:

		enum {	e_SectorShift = 11,
				e_SectorSize = 2048 };

		//========================================================================
		//========================================================================
		fsCDFileStream(fsFileStream::AccessType i_DesiredAccess, const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~fsCDFileStream();

		//========================================================================
		//========================================================================
		virtual int Read(int i_NumBytes, void* o_Buffer);

		//========================================================================
		//========================================================================
		virtual void Write(int i_NumBytes, const void* i_Buffer);

		//========================================================================
		//========================================================================
		virtual void SetFilePos(int i_Pos, fsFileStream::AccessPointType i_DesiredAccessPoint);

	private:

		//========================================================================
		//========================================================================
		void cd_sync();

		//========================================================================
		//========================================================================
		void sync_multistream();

		int m_CachedSector;
		int m_StartSector;
		int m_FileSize;
		envType::Int8 *m_Cache;
		sceCdRMode m_CDMode;
};

//========================================================================
//========================================================================
inline void fsCDFileStream::cd_sync()
{
	while(sceCdSync(1));
	int error = sceCdGetError();
	if( error != SCECdErNO )
	{
		DBG_WARNING1("sceCdGetError: %d", error);
		throw fsUnknownX(this->GetLocator());
	}
}

//========================================================================
//========================================================================
void fsCDFileStream::sync_multistream()
{
/*
	SOUND_STREAM_INFO stream_info;
	SOUND_GetStreamInfo(0, &stream_info);
	while( stream_info.CDStatus == 1 )
	{
//		DBG_WARNING0("Waiting on MultiStream....");
		while(SOUND_CheckRPCStatus()==1);
		SOUND_FlushIOPCommand(0);
		SOUND_GetStreamInfo(0, &stream_info);
	}	
	while(sceCdSync(1));
//	DBG_WARNING0("fsCDFileStream ready to read");
*/
}


//========================================================================
//========================================================================
fsCDFileStream::fsCDFileStream(fsFileStream::AccessType i_DesiredAccess, const fsLocator& i_Locator)
:	fsFileStreamImp(i_Locator),
	m_CachedSector(-1)
{
	DBG_ASSERT0(i_DesiredAccess == fsFileStream::e_ReadOnly, "Only read only access supported for CD files");

	m_CDMode.trycount = 0;
	m_CDMode.spindlctrl = SCECdSpinNom;
	m_CDMode.datapattern = SCECdSecS2048;

	if( fsFileUtilPAC::CDSearchFile(m_StartSector, m_FileSize, i_Locator) == false )
		throw fsFileDoesntExistX(i_Locator);
		
	//	alloc cache with nice alignment
	m_Cache = (envType::Int8*)memalign(64, e_SectorSize);

	//	cache first sector
//	this->sync_multistream();
	int ret_val = sceCdRead(m_StartSector, 1, m_Cache, &m_CDMode);
	DBG_ASSERT0(ret_val != 0, "sceCdRead returned 0");
	m_CachedSector = 0;
	this->cd_sync();

	DBG_ASSERT0((this->GetFilePos() >> e_SectorShift) == m_CachedSector, "non-current cache");
}

//========================================================================
//========================================================================
fsCDFileStream::~fsCDFileStream()
{
	free(m_Cache);
}

//========================================================================
//========================================================================
int fsCDFileStream::Read(int i_NumBytes, void* o_Buffer)
{
	//	first see how much we can copy from cache
	int total_read = 0;
	int num_in_cache = 0;
	int cur_sector = this->GetFilePos() >> e_SectorShift;
	int cur_offset = this->GetFilePos() - (cur_sector << e_SectorShift);
	DBG_ASSERT0((this->GetFilePos() >> e_SectorShift) == m_CachedSector, "non-current cache");

	num_in_cache = maFunctions::Lowest(e_SectorSize - cur_offset, i_NumBytes);
	memcpy(o_Buffer, m_Cache + cur_offset, num_in_cache);
	total_read += num_in_cache;
	this->IncVirtualFilePointer(num_in_cache);
	
	if( i_NumBytes == num_in_cache )
	{
		//	done with read operation
		//	need to cache next sector?
		cur_sector = this->GetFilePos() >> e_SectorShift;
		if( cur_sector != m_CachedSector )
		{
//			this->sync_multistream();
			int ret_val = sceCdRead(cur_sector + m_StartSector, 1, m_Cache, &m_CDMode);
			DBG_ASSERT0(ret_val != 0, "sceCdRead returned 0");
			m_CachedSector = cur_sector;
			this->cd_sync();
		}

		DBG_ASSERT0((this->GetFilePos() >> e_SectorShift) == m_CachedSector, "non-current cache");
		return total_read;
	}
	else
	{
		//	partial cache hit
		i_NumBytes -= num_in_cache;
		o_Buffer = ((envType::UInt8*)o_Buffer) + num_in_cache;
	}

	//	at this point we have to read data that's not in the cache
	//	might be able to read multiple sectors, directly into buffer
	//	except the last sector we will generally need less than
	//	2048 bytes, so we must cache that one and copy only the
	//	amount requested
	int full_sectors_requested = i_NumBytes >> e_SectorShift;
	if( full_sectors_requested > 0 )
	{
//		DBG_WARNING1("Num sectors requested: %d", full_sectors_requested);
//		this->sync_multistream();
		int ret_val = sceCdRead(m_CachedSector + m_StartSector + 1, full_sectors_requested, o_Buffer, &m_CDMode);
		DBG_ASSERT0(ret_val != 0, "sceCdRead returned 0");
		int num_read = full_sectors_requested << e_SectorShift;
		this->IncVirtualFilePointer(num_read);
		i_NumBytes -= num_read;
		total_read += num_read;
		o_Buffer = ((envType::UInt8*)o_Buffer) + num_read;
		this->cd_sync();
	}

	//	cache next sector
	m_CachedSector += full_sectors_requested + 1;
//	this->sync_multistream();
	int ret_val = sceCdRead(m_CachedSector + m_StartSector, 1, m_Cache, &m_CDMode);
	DBG_ASSERT0(ret_val != 0, "sceCdRead returned 0");
	this->cd_sync();

	if( i_NumBytes > 0 )
	{
		memcpy(o_Buffer, m_Cache, i_NumBytes);
		total_read += i_NumBytes;
		this->IncVirtualFilePointer(i_NumBytes);
	}
	
	DBG_ASSERT0((this->GetFilePos() >> e_SectorShift) == m_CachedSector, "non-current cache");
	return total_read;
}

//========================================================================
//========================================================================
void fsCDFileStream::Write(int i_NumBytes, const void* i_Buffer)
{
	DBG_ASSERT0(false, "Can't write to CD!");
	throw fsReadOnlyX(this->GetLocator());
}

//========================================================================
//========================================================================
void fsCDFileStream::SetFilePos(int i_Pos, fsFileStream::AccessPointType i_DesiredAccessPoint)
{
	DBG_ASSERT0((this->GetFilePos() >> e_SectorShift) == m_CachedSector, "non-current cache");
	int ret_val = 0;
	int whence;
	int seek_to;

	if( i_DesiredAccessPoint == fsFileStream::e_Beginning )
	{
		DBG_ASSERT0( i_Pos >= 0,  "Negative seek attempted" );
		seek_to = i_Pos;
	}
	else if( i_DesiredAccessPoint == fsFileStream::e_Current )
	{
		DBG_ASSERT0( this->GetFilePos() - i_Pos >= 0, "Negative seek attempted" );
		seek_to = i_Pos + this->GetFilePos();
	}
	else
	{
		seek_to = m_FileSize + i_Pos;
	}

	int sector_to_cache = seek_to >> e_SectorShift;

	if( m_CachedSector != sector_to_cache )
	{
//		this->sync_multistream();
		int ret_val = sceCdRead(sector_to_cache + m_StartSector, 1, m_Cache, &m_CDMode);
		DBG_ASSERT0(ret_val != 0, "sceCdRead returned 0");
		m_CachedSector = sector_to_cache;
		this->cd_sync();
	}

	this->SetVirtualFilePointer(seek_to);
	DBG_ASSERT0((this->GetFilePos() >> e_SectorShift) == m_CachedSector, "non-current cache");
}

//========================================================================
//	fsFileStream
//========================================================================

//========================================================================
//	Constructor.  This could throw an exception if the file doesn't
//	exist, or something.  So be ready!
//========================================================================
fsFileStream::fsFileStream(const fsLocator& i_Locator, AccessType i_DesiredAccess)
{
	if( i_Locator.GetName(0) == lc_CDRomString )
		m_pImp = new fsCDFileStream(i_DesiredAccess, i_Locator);
	else
		m_pImp = new fsHostFileStream(i_DesiredAccess, i_Locator);
}

//========================================================================
//	Destructor
//========================================================================
fsFileStream::~fsFileStream()
{
	delete m_pImp;
}

//========================================================================
//	Read reads i_NumBytes into the buffer.  If the file is too short 
//	to read	i_NumBytes, it will read as many as it can.  The return
//	value is the number actually read into the o_Buffer.  This function
//	also advances the file pointer.
//========================================================================
int fsFileStream::Read(int i_NumBytes, void* o_Buffer)
{
	return m_pImp->Read(i_NumBytes, o_Buffer);
}

//========================================================================
//	Write writes i_NumBytes from o_Buffer into the file.  The file 
//	pointer will also be advanced.
//========================================================================
void fsFileStream::Write(int i_NumBytes, const void* i_Buffer)
{
	m_pImp->Write(i_NumBytes, i_Buffer);
}

//========================================================================
//	GetFilePos returns the current position of the file pointer.
//========================================================================
int fsFileStream::GetFilePos() const
{
	return m_pImp->GetFilePos();
}

//========================================================================
//	GetLocator returns this filestream's associated locator
//========================================================================
const fsLocator& fsFileStream::GetLocator() const
{
	return m_pImp->GetLocator();
}


//========================================================================
//	SetFilePos sets the file pointer to the given value.  It is valid
//	to move the file pointer beyond the end of the file.
//========================================================================
void fsFileStream::SetFilePos(int i_Pos, AccessPointType i_DesiredAccessPoint )
{
	m_pImp->SetFilePos(i_Pos, i_DesiredAccessPoint);
}
