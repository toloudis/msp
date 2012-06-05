/****************************************************************************\
**  fsFileUtilPACPS2.cpp
**
**      fsFileUtilPACPS2.cpp defines the file util PAC for PS2.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "fsFileUtilPACPS2.hpp"

//#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "envInitX.hpp"
#include "fsFileX.hpp"

#include <libcdvd.h>
#include <sifdev.h>
#include <sifrpc.h>

namespace fsFileUtilPAC
{

namespace
{
	bool l_RebootIOP = false;
	bool l_CDFiles = false;

	const char lc_HostDirSep = 47; // '/'
	const char lc_CDRomDirSep = 92; // '\'

	const int lc_MaxPath = 256;

	const itString lc_CDRomString("cdrom0:");
	const itString lc_HostString("host0:");

	int l_RootSecNum = -1;
	int l_RootSectors = -1;

	unsigned char l_CdBuffer[2048] __attribute__ ((aligned (64)));	// CD sector buffer
	sceCdRMode l_CDMode;											// Mode settings for CD reading

	struct DirRecord
	{
		char m_Name[16];
		int m_Size;
		int m_Sector;
		bool m_IsDir;
		std::vector<DirRecord> m_SubDirs;
	};

	inline int size_to_sectors(int i_Size)
	{
		return (i_Size + 2047) >> 11;
	}
	
	inline int get_int(int start)
	{
		return( l_CdBuffer[start]+(l_CdBuffer[start+1]<<8)+(l_CdBuffer[start+2]<<16)+(l_CdBuffer[start+3]<<24) );
	}

	DirRecord l_Root;

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

	void recurse_add_dirs(DirRecord& o_Root, int i_StartSec, int i_NumSectors)
	{
		int cur_sector;

		// loop through all the sectors, checking for directory or filename
		for( cur_sector = 0 ; cur_sector < i_NumSectors ; cur_sector++ )
		{ 
			int dir_record_loc;

			sceCdRead(i_StartSec + cur_sector, 1, l_CdBuffer, &l_CDMode);
			sceCdSync(0);

			for( dir_record_loc = 0 ; l_CdBuffer[dir_record_loc] != 0 ; dir_record_loc += l_CdBuffer[dir_record_loc] )
			{
				const char* dir_name = (const char*)(l_CdBuffer + dir_record_loc + 33);
				int name_len = l_CdBuffer[dir_record_loc+32];
				if( (!isalnum(*dir_name)) || (name_len == 0) )
					continue;		//	skip "current dir" and so on

				int cur_rec_num = o_Root.m_SubDirs.size();
				o_Root.m_SubDirs.push_back(DirRecord());
				DirRecord& cur_rec = o_Root.m_SubDirs[cur_rec_num];
		
				// Copy name from directory record
				memcpy(cur_rec.m_Name, dir_name, name_len);
				cur_rec.m_Name[name_len] = 0;

				//	and file location/size
				cur_rec.m_Sector = get_int(dir_record_loc+2);
				cur_rec.m_Size = get_int(dir_record_loc+10);

				cur_rec.m_IsDir = (l_CdBuffer[dir_record_loc + 25] & 0x2) != 0;
			}
		}

		//	now get subdirectories for each directory
		int cur_subdir_num;
		int num_subdirs = o_Root.m_SubDirs.size();
		for( cur_subdir_num = 0 ; cur_subdir_num < num_subdirs ; ++cur_subdir_num )
		{
			DirRecord& cur_record = o_Root.m_SubDirs[cur_subdir_num];
			if( cur_record.m_IsDir )
				recurse_add_dirs(cur_record, cur_record.m_Sector, size_to_sectors(cur_record.m_Size));
		}
	}

	void get_root_dir_details(int& o_SecNum, int& o_NumSectors)
	{
		// read PVD details from sector 16
		sceCdRead(0x10, 1, l_CdBuffer, &l_CDMode);
		sceCdSync(0);

		// Get value of sector position
		o_SecNum = get_int(158);
		o_NumSectors = size_to_sectors(get_int(166));

		return;
	}

	int is_file_in_dir(int& io_StartSec, int& io_NumSectors, const char *i_Filename)
	{
		int cur_sector;
		std::string cur_name;

		// loop through all the sectors, checking for directory or filename
		for( cur_sector = 0 ; cur_sector < io_NumSectors ; cur_sector++ )
		{ 
			int dir_record_loc = 0;

			sceCdRead(io_StartSec + cur_sector, 1, l_CdBuffer, &l_CDMode);
			sceCdSync(0);

			while( l_CdBuffer[dir_record_loc] )
			{
				// Copy name from directory record
				cur_name.assign((char*)(l_CdBuffer + dir_record_loc + 33), l_CdBuffer[dir_record_loc+32]);

				// compare directory record to filename being searched
				// if they are equal return the sector position and length
				if( strcmp(cur_name.c_str(), i_Filename) == 0 )
				{
					// Get value of sector position
					int length = get_int(dir_record_loc+10);
					io_StartSec = get_int(dir_record_loc+2);
					io_NumSectors = size_to_sectors(length);
					return length;
				}

				dir_record_loc += l_CdBuffer[dir_record_loc];
			}
		}

		return 0;
	}
}

//============================================================================
//	SetSystemParams is a PS2 specific function that controls aspects of how
//	the the game boots and runs.  If reboot IOP is true it will reboot the
//	IOP to replace the IOP modules.  If i_CDFiles is true it will expect all
//	IOP modules to be in a directory "IOP" on a CD in the drive, and
//	additionally all game files will be loaded from CD.
//	The final game will use "true, true" for these parameters.
//	This function should be called before the fsPackage::Init().
//============================================================================
void SetSystemParams(bool i_RebootIOP, bool i_CDFiles)
{
	l_RebootIOP = i_RebootIOP;
	l_CDFiles = i_CDFiles;
}

//============================================================================
//	GetCDMode will return true if game files are to be loaded from CD/DVD,
//	as opposed to networked host computer.
//============================================================================
bool GetCDMode()
{
	return l_CDFiles;
}

//============================================================================
//	CDSearchFile works like sceCdSearchFile, but works properly with > 30
//	files/directory.  It returns true if the file is found; in such a case,
//	the file's first sector and size will also be filled in.  The size is in
//	bytes.
//============================================================================
bool CDSearchFile(int& o_Sector, int& o_Size, const fsLocator& i_Locator)
{
	DirRecord* cur_dir = &l_Root;
	int cur_loc_index;
	int loc_depth = i_Locator.GetNumNames();
	char cur_name[16];
	
	//	skip the first name, which is "cdrom0:"
	for( cur_loc_index = 1 ; cur_loc_index < loc_depth ; ++cur_loc_index ) 
	{
		//	convert current directory to upper/ansi
		const itString& cur_loc_string = i_Locator.GetName(cur_loc_index);
		int i;
		int num = cur_loc_string.GetLength();
		for( i = 0 ; i < num ; ++i )
			cur_name[i] = toupper(cur_loc_string[i]);

		bool last_name = cur_loc_index == (loc_depth - 1);
		if( last_name )
		{
			cur_name[i++] = ';';
			cur_name[i++] = '1';
		}

		cur_name[i++] = 0;		

		//	find subdir in cur_dir
		num = cur_dir->m_SubDirs.size();
		bool found_match = false;
		DirRecord* test_dir = NULL;
		for( i = 0 ; i < num ; ++i )
		{
			test_dir = &(cur_dir->m_SubDirs[i]);
			if( strcmp(cur_name, test_dir->m_Name) == 0 )
			{
				//	match - either keep recursing, or finish
				if( last_name )
				{
					o_Sector = test_dir->m_Sector;
					o_Size = test_dir->m_Size;
					return true;
				}
				else
				{
					found_match = true;
					break;
				}
			}
		}
	
		if( !found_match )
			return false;

		cur_dir = test_dir;
	}

	return false;

/*	std::string filename;
	LocatorToCDFilename(i_Locator, filename, false);

	int i;

	int secNum = l_RootSecNum;
	int numSectors = l_RootSectors;
	int numBytes;

	std::string::size_type next_dir = 0;
	std::string dir_name;

	//	iterate over subdirectories
	//	we know we only need to do this 8 times as that is the directory number limit for ISO 9660
	for( i = 0 ; i < 8 ; i++ )
	{
		//	get next directory name
		std::string::size_type cur = next_dir + 1;
		next_dir = filename.find('\\', cur);

		if( next_dir == std::string::npos )
		{
			//	no more subdirectories, find the file itself
			if( (numBytes = is_file_in_dir(secNum, numSectors, filename.c_str() + cur)) )
			{
				o_Sector = secNum;
				o_Size = numBytes;
				return true;
			}
			else
				return false;
		}
		else
		{
			dir_name = filename.substr(cur, next_dir - cur);
			// check that directory exists and use it's
			// pos / size in the next loop
			if( !is_file_in_dir(secNum, numSectors, dir_name.c_str()) )
				return false;
		}
	}

	return 0;*/
}


//============================================================================
//	GetCDDirectoryFileInfo()
//
//	Get all files under the given directory. Subdirectories are not included.
//============================================================================
bool GetCDDirectoryFileInfo( const fsLocator& i_Locator, std::vector<itString>& o_FileInfo )
{
	DirRecord* cur_dir = &l_Root;
	int cur_loc_index;
	int loc_depth = i_Locator.GetNumNames();
	char cur_name[16];
	
	//	skip the first name, which is "cdrom0:"
	for( cur_loc_index = 1 ; cur_loc_index < loc_depth ; ++cur_loc_index ) 
	{
		//	convert current directory to upper/ansi
		const itString& cur_loc_string = i_Locator.GetName(cur_loc_index);
		int i;
		int num = cur_loc_string.GetLength();
		for( i = 0 ; i < num ; ++i )
			cur_name[i] = toupper(cur_loc_string[i]);

		bool last_name = cur_loc_index == (loc_depth - 1);
/*
		if( last_name )
		{
			cur_name[i++] = ';';
			cur_name[i++] = '1';
		}
*/
		cur_name[i++] = 0;		

		//	find subdir in cur_dir
		num = cur_dir->m_SubDirs.size();
		bool found_match = false;
		DirRecord* test_dir = NULL;
		for( i = 0 ; i < num ; ++i )
		{
			test_dir = &(cur_dir->m_SubDirs[i]);
			if( strcmp(cur_name, test_dir->m_Name) == 0 )
			{
				//	match - either keep recursing, or finish
				if( last_name )
				{
					int cur_record;
					int record_num = test_dir->m_SubDirs.size();
					for ( cur_record = 0; cur_record < record_num; ++cur_record )
					{
						DirRecord* cur_info = &(test_dir->m_SubDirs[cur_record]);
						if ( !cur_info->m_IsDir )
						{
							o_FileInfo.push_back( itString(cur_info->m_Name) );
						}
					}
					return true;
				}
				else
				{
					found_match = true;
					break;
				}
			}
		}
	
		if( !found_match )
			return false;

		cur_dir = test_dir;
	}

	return false;
}

//============================================================================
//	CreateFile creates a file with the given name.  This must be done before
//	reading or writing with a fsFileStream.
//============================================================================
void CreateFile(const fsLocator& i_Filename)
{
	std::string filename;
	LocatorToANSIFilename(i_Filename, filename);
	int file_handle = sceOpen(filename.c_str(), SCE_CREAT | SCE_EXCL, 0x01ff);
	if( file_handle < 0 )
		handle_file_error(file_handle, i_Filename);	//	this will throw
		
	sceClose(file_handle);
}

//============================================================================
//	RenameFile renames a file.  This can be used to move a file to different
//	directories, etc.
//============================================================================
void RenameFile(const fsLocator& i_From, const fsLocator& i_To)
{
	std::string from_name, to_name;
	LocatorToANSIFilename(i_From, from_name);
	LocatorToANSIFilename(i_To, to_name);
	int ret_val = sceRename(from_name.c_str(), to_name.c_str());
	if( ret_val < 0 )
		handle_file_error(ret_val, i_From);
}

//============================================================================
//	DeleteFile deletes a file.  The file must exist, or an exception will be
//	thrown.
//============================================================================
void DeleteFile(const fsLocator& i_Filename)
{
	std::string filename;
	LocatorToANSIFilename(i_Filename, filename);
	int ret_val = sceRemove(filename.c_str());
	if( ret_val < 0 )
		handle_file_error(ret_val, i_Filename);	//	this will throw
}

//============================================================================
//	CopyFile copies an entire file from one location to another.  Of course,
//	the file must exist.
//============================================================================
void CopyFile(const fsLocator& i_From, const fsLocator& i_To)
{
	DBG_ASSERT0(false, "please implement me!");
	throw fsUnknownX(i_From);
}

//============================================================================
//	CopyDirectory copies a directory with the given name and all sub-dirs.
//============================================================================
void CopyDirectory(const fsLocator& i_DirectoryName)
{
	DBG_ASSERT0( false, "please implement this function." );
	throw fsUnknownX(i_DirectoryName);
}

//============================================================================
//	CreateDirectory creates a directory with the given name.
//
//============================================================================
void CreateDirectory(const fsLocator& i_DirectoryName)
{
	std::string filename;
	LocatorToANSIFilename(i_DirectoryName, filename, true);
	int ret_val = sceMkdir(filename.c_str(), 0x01ff);
	if( ret_val < 0 )
		handle_file_error(ret_val, i_DirectoryName);	//	this will throw
}

//============================================================================
//	DeleteDirectory destroys a directory with the given name.  The directory
//	must be empty or a fsDirectoryNotEmpty exception will be thrown.
//============================================================================
void DeleteDirectory(const fsLocator& i_DirectoryName)
{
	std::string filename;
	LocatorToANSIFilename(i_DirectoryName, filename, true);
	int ret_val = sceRmdir(filename.c_str());
	if( ret_val < 0 )
		handle_file_error(ret_val, i_DirectoryName);	//	this will throw
}

//============================================================================
//	FileExists tests if a file exists.
//============================================================================
bool FileExists(const fsLocator& i_Filename)
{
	if( i_Filename.GetName(0) != lc_CDRomString )
	{
//		return false;	//	not supported on host right now
		std::string filename;
		LocatorToANSIFilename( i_Filename, filename, true );
		int file_handle = sceOpen( filename.c_str(), SCE_RDONLY );
		if ( file_handle < 0 )
		{
			return false;
		}
		else
		{
			sceClose( file_handle );
			return true;	
		}
	}

	std::string filename;
	LocatorToCDFilename(i_Filename, filename);
	
	sceCdlFILE file_info;
	int ret_val = sceCdSearchFile(&file_info, filename.c_str());
	return ret_val != 0;
}

//============================================================================
//	DirectoryExists tests if a directory exists.
//
//============================================================================
bool DirectoryExists(const fsLocator& i_Filename)
{
	if( i_Filename.GetName(0) != lc_CDRomString )
		return false;	//	not supported on host right now

	std::string filename;
	LocatorToCDFilename(i_Filename, filename, true);
	
	sceCdlFILE file_info;
	int ret_val = sceCdSearchFile(&file_info, filename.c_str());
	return ret_val != 0;
}

//========================================================================
// ANSIFilenameToLocator returns a locator from an ANSI pathname
//========================================================================
void ANSIFilenameToLocator(const std::string& i_String, fsLocator& o_Locator)
{
	o_Locator.Clear();
	int k;
	int nBegin;
	itString itName(i_String.c_str());
	itString Piece;
	nBegin = 0;
	
	char dir_sep;
	if( itName.HasSubString(lc_CDRomString) )
		dir_sep = lc_CDRomDirSep;
	else
		dir_sep = lc_HostDirSep;
	
	for (k = 0; k < itName.GetLength(); k++)
	{
		if (itName[k] == itString::CharType(dir_sep) )
		{
			Piece = itString(nBegin, (k - nBegin), itName);
			nBegin = k + 1;
			o_Locator.Push(Piece);
		}
	}
	Piece = itString(nBegin, (itName.GetLength() - nBegin), itName);
	o_Locator.Push(Piece);
}

//============================================================================
//	LocatorToANSIFilename is used internally by the PS2 PACs in the fs
//	package.  It converts a fsLocator to a ANSI filename (for use by
//	PS2 library functions).
//============================================================================
void LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, bool i_Directory)
{
	char work_string[lc_MaxPath];
	int num_names = i_Locator.GetNumNames();
	int i;

	if ( num_names <= 0 )
		return;

	char dir_sep;
	if( i_Locator.GetName(0) == lc_CDRomString )
		dir_sep = lc_CDRomDirSep;
	else if( i_Locator.GetName(0) == lc_HostString )
		dir_sep = lc_HostDirSep;
	else
		dir_sep = lc_HostDirSep;	//	hard disk?	

	// iterate and add each name to the path
	//
	int num_chars = 0;
	for ( i = 0 ; i < num_names ; i++ )
	{
		const itString& cur_string = i_Locator.GetName(i);
		int cur_char;
		int cur_length = cur_string.GetLength();
		
		for( cur_char = 0 ; cur_char < cur_length ; ++cur_char )
		{
			char ansi_char = char(cur_string[cur_char]);
			
			if( i > 0 )		//	don't upper the device name
				ansi_char = toupper(ansi_char);
	
			work_string[num_chars++] = ansi_char;
		}
		if( i < (num_names - 1) )
			work_string[num_chars++] = dir_sep;
	}

	if( (dir_sep == lc_CDRomDirSep) && !i_Directory )
	{
		work_string[num_chars++] = ';';
		work_string[num_chars++] = '1';
	}

	work_string[num_chars++] = 0;

	o_String = work_string;
}

//============================================================================
//	LocatorToCDFilename is used internally by the PS2 PACs in the fs
//	package.  It converts a cdrom0 locator to a format useable by 
//	sceCDSearchFile.
//============================================================================
void LocatorToCDFilename(const fsLocator& i_Locator, std::string& o_String, bool i_Directory)
{
	char work_string[lc_MaxPath];
	int num_names = i_Locator.GetNumNames();
	int i;

	if ( num_names <= 1 )
		return;

	char dir_sep;
	DBG_ASSERT0( i_Locator.GetName(0) == lc_CDRomString, "LocatorToCDFilename expects a cdrom0: path")

	// iterate and add each name to the path
	//
	int num_chars = 0;
	for ( i = 1 ; i < num_names ; i++ )
	{
		work_string[num_chars++] = lc_CDRomDirSep;
		
		const itString& cur_string = i_Locator.GetName(i);
		int cur_char;
		int cur_length = cur_string.GetLength();
		
		for( cur_char = 0 ; cur_char < cur_length ; ++cur_char )
			work_string[num_chars++] = toupper(cur_string[cur_char]);
	}

	if( !i_Directory )
	{
		work_string[num_chars++] = ';';
		work_string[num_chars++] = '1';
	}

	work_string[num_chars++] = 0;

	o_String = work_string;
}

//============================================================================
//	LocatorToANSIFilename is used internally by the PS2 PACs in the fs
//	package.  It converts a fsLocator to a ANSI filename (for use by
//	PS2 library functions).
//============================================================================
/*void 
LocatorToANSIFilename(const fsLocator& i_Locator, std::string& o_String, const int i_StartNum, const int i_EndNum )
{
	char work_string[lc_MaxPath];
	int num_names = i_Locator.GetNumNames();
	int i;

	DBG_ASSERT1( i_StartNum >= 0, "Locator start index out of range (%d)", i_StartNum );
	DBG_ASSERT2( i_EndNum <= num_names, "Locator end index out of range (%d > %d)", i_EndNum, num_names );

	if ( num_names <= 0 )
		return;

	// iterate and add each name to the path
	//
	int num_chars = 0;
	for ( i = i_StartNum ; i <= i_EndNum ; i++ )
	{
		const itString& cur_string = i_Locator.GetName(i);
		int cur_char;
		
		for( cur_char = 0 ; cur_char < cur_string.GetLength() ; ++cur_char )
			work_string[num_chars++] = char(cur_string[cur_char]);

		if( i < (num_names - 1) )
			work_string[num_chars++] = lc_DirSep;
	}

	work_string[num_chars++] = 0;

	o_String = work_string;
}
*/
//============================================================================
//	IsReadOnly returns true if the file is read-only, meaning that write
//	operations to the file will fail.
//============================================================================
bool IsReadOnly(const fsLocator& i_Locator)
{
	std::string filename;
	LocatorToANSIFilename(i_Locator, filename);
	sce_stat file_stat;
	
	int ret_val = sceGetstat(filename.c_str(), &file_stat);

	if( ret_val < 0 )
		handle_file_error(ret_val, i_Locator);

	return file_stat.st_mode & (1 << 7) == 0;
}

#define CD_MODULES cdrom0:
#define HOST_MODULES host0:/sce/iop/modules

#undef REBOOT_IOP
#undef LOAD_CD

#define LOAD_MODULES HOST_MODULES

#define CD_IMAGE_FILE "cdrom0:\\IOP\\"IOP_IMAGE_FILE";1"
//#define CD_IMAGE_FILE "cdrom0:\\IOP\\"IOP_IMAGE_FILE""
#define HOST_IMAGE_FILE "host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file

//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void Init()
{
	int ret_val;

	if( l_RebootIOP )
	{
		//	reboot IOP
		sceSifInitRpc(0);
		
		if( l_CDFiles )
			sceCdInit(SCECdINIT);
		
		// Reboot IOP, replace default modules 
		if( l_CDFiles )
		{
			while ( !sceSifRebootIop(CD_IMAGE_FILE) ); // (Important) Unlimited retries 
		}
		else
		{
			while ( !sceSifRebootIop(HOST_IMAGE_FILE) ); // (Important) Unlimited retries 
		}

		while( !sceSifSyncIop() );
	}

	// Reinitialize 
	sceSifInitRpc(0);
//	sceSifLoadFileReset();
//	sceFsReset();

	// Startup non-default modules
	if( l_CDFiles )
	{
		sceCdInit(SCECdINIT);
		sceCdMmode(SCECdCD);

		l_CDMode.trycount = 0;
		l_CDMode.spindlctrl = SCECdSpinNom;
		l_CDMode.datapattern = SCECdSecS2048;

		get_root_dir_details(l_RootSecNum, l_RootSectors);
		recurse_add_dirs(l_Root, l_RootSecNum, l_RootSectors);

		while( sceSifLoadModule("cdrom0:\\IOP\\SIO2MAN.IRX;1", 0, NULL) < 0 )
		{
			DBG_LOG0("Failed to load SIO2MAN.IRX");
		}
			
		while( sceSifLoadModule("cdrom0:\\IOP\\DBCMAN.IRX;1", 0, NULL) < 0 )
		{
			DBG_LOG0("Failed to load DBCMAN.IRX");
		}	

		while( sceSifLoadModule("cdrom0:\\IOP\\SIO2D.IRX;1", 0, NULL) < 0 )
		{
			DBG_LOG0("Failed to load SIO2D.IRX");
		}	

		while( sceSifLoadModule("cdrom0:\\IOP\\DS2O.IRX;1", 0, NULL) < 0 )
		{
			DBG_LOG0("Failed to load DS2O.IRX");
		}	
	}
	else
	{
		while( sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2man.irx", 0, NULL) < 0 )
		{
			DBG_LOG0("Failed to load SIO2MAN.IRX");
		}
		
		while( sceSifLoadModule("host0:/usr/local/sce/iop/modules/dbcman.irx", 0, NULL) < 0 )
		{
			DBG_LOG0("Failed to load DBCMAN.IRX");
		}	

		while( sceSifLoadModule("host0:/usr/local/sce/iop/modules/sio2d.irx", 0, NULL) < 0 )
		{
			DBG_LOG0("Failed to load SIO2D.IRX");
		}	

		int i;
		for( i = 0 ; i < 2; ++i )
		{
			while( sceSifLoadModule("host0:/usr/local/sce/iop/modules/pad2/ds2o.irx", 0, NULL) < 0 )
			{
				DBG_LOG0("Failed to load DS2O.IRX");
			}	
		}
	}
}

void CleanUp() throw()
{
}

}
