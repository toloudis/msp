/**********************************************************
**  fsFileStreamWin.cpp
**
**      This is the Xbox version of the fsFileStream
**	implementation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include "fsFileStream.hpp"

#include <xtl.h>

// Windows unfortunately defines several functions as macros to conditionally
// compile with Unicode or ANSI.  We will undefine them here and call the
// specific versions (which we have to do anyway).
//
#undef CreateFile

//	The windows documentation mentions INVALID_SET_FILE_POINTER,
//	but it is stupidly never defined in a windows header.  So I must
//	define it here.  Normally I would never use a #define but I
//	will try to emulate how Windows would have done it.  sigh...
#define INVALID_SET_FILE_POINTER ((DWORD)-1)


#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "envErrorDisplayForwarder.hpp"
#include "envPackageErrorIndices.hpp"
#include "fsErrorCodes.hpp"
#include "fsFileUtilPAC.hpp"
#include "fsFileX.hpp"

#include "appApplicationPAC.hpp"

struct fsFileStreamImp
{
	HANDLE m_File;
	fsFileStream::AccessType m_Access;
	int m_FilePointer;
	fsLocator m_Locator;
};

namespace
{

//========================================================================
//========================================================================
void handle_windows_error(const fsLocator& i_Locator)
{
	DWORD error = ::GetLastError();

	switch ( error )
	{
		case ERROR_FILE_NOT_FOUND:
			throw fsFileDoesntExistX(i_Locator);
		break;

		case ERROR_PATH_NOT_FOUND:
			throw fsDirectoryDoesntExistX(i_Locator);
		break;

		case ERROR_INVALID_DRIVE:
			throw fsInvalidLocatorX(i_Locator);
		break;

		case ERROR_SHARING_VIOLATION:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_LOCK_VIOLATION:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_FILE_EXISTS:
			throw fsFileExistsX(i_Locator);
		break;

		case ERROR_DRIVE_LOCKED:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_INVALID_NAME:
			throw fsInvalidLocatorX(i_Locator);
		break;

		case ERROR_DIR_NOT_EMPTY:
			throw fsDirectoryNotEmptyX(i_Locator);
		break;

		case ERROR_PATH_BUSY:
			throw fsFileInUseX(i_Locator);
		break;

		case ERROR_ACCESS_DENIED:
			throw fsReadOnlyX(i_Locator);
		break;

		default:
			throw fsUnknownX(i_Locator);
		break;
	}
}

//========================================================================
//========================================================================
bool load_string(char* o_String, int i_ID, int i_NumBufferChars)
{
	DBG_ASSERT0(false, "No window title for Xbox!");
	return false;
/*
	int num_chars = ::LoadString(	::GetModuleHandle(NULL),
									i_ID,
									o_String,
									i_NumBufferChars);

	return num_chars != 0;
*/
}

//========================================================================
//	returns true if the file open needs to be tried again, false if the
//	open should fail.
//========================================================================
bool test_cd_removal(const fsLocator& i_Locator)
{
	DBG_WARNING0(" test_cd_removal() not implemented for Xbox!");
  	return false;
/*
	DWORD error = ::GetLastError();

	if( error == ERROR_NOT_READY )
	{
		DBG_WARNING0("CD Removed (fsFileStream)!!!");
		while( ::ShowCursor(true) < 1 )
			;

		envErrorDisplayForwarder::ErrorDisplayReturnCodes ret_val;
		ret_val = envErrorDisplayForwarder::DisplayError(envErrorDisplayForwarder::e_OkCancelDisplay,
															envPackageErrorIndices::e_Fs,
															fsErrorCodes::e_ReInsertCD);

		while( ::ShowCursor(false) >= 0 )
			;

		if( ret_val == envErrorDisplayForwarder::e_Ok )
			return true;
		else
			return false;
	}
	else
		return false;
*/
}

}

//========================================================================
//	Constructor.  This could throw an exception if the file doesn't
//	exist, or something.  So be ready!
//========================================================================
fsFileStream::fsFileStream(const fsLocator& i_Locator, AccessType i_DesiredAccess)
{
	DWORD access;
	DWORD share_mode;

	switch ( i_DesiredAccess )
	{
		case e_ReadOnly:
			access = GENERIC_READ;
			share_mode = FILE_SHARE_READ;
		break;

		case e_WriteOnly:
			access = GENERIC_WRITE;
			share_mode = 0;
		break;

		case e_ReadWrite:
			access = GENERIC_READ | GENERIC_WRITE;
			share_mode = 0;
		break;

		default:
			DBG_ASSERT0(false, "Invalid Access type");
		break;
	}

	HANDLE new_file = INVALID_HANDLE_VALUE;

	while( true )
	{
		if ( fsFileUtilPAC::MustUseANSIFilenames() )
		{
			std::string filename;
			fsFileUtilPAC::LocatorToANSIFilename(i_Locator, filename);

			if ( filename.size() == 0 )
				throw fsInvalidLocatorX(i_Locator);

			new_file = ::CreateFileA(	filename.c_str(),
//			new_file =	CreateFileA(	filename.c_str(),
//			new_file =	CreateFileA(	"UltimateRideX\\basicstr.tsf",
//			new_file =	CreateFileA(	"D:\\basicstr.tsf",
										access,
										share_mode,
										NULL,
										OPEN_EXISTING,
										0,
//										FILE_ATTRIBUTE_NORMAL,
										NULL);
			if ( new_file == INVALID_HANDLE_VALUE )
			{
				const char* temp = filename.c_str();
				DBG_WARNING1( "failed to open file:%s", temp );
			}
		}
		else
		{
			DBG_ASSERT0(false, "Unicode not implemented!");
/*
			itString filename;
			fsFileUtilPAC::LocatorToUnicodeFilename(i_Locator, filename);
		
			if ( filename.GetLength() == 0 )
				throw fsInvalidLocatorX(i_Locator);

			new_file = ::CreateFileW(	filename.GetString(),
										access,
										share_mode,
										NULL,
										OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL,
										NULL);
*/
		}

		if ( new_file == INVALID_HANDLE_VALUE )
		{
			if( test_cd_removal(i_Locator) )
				continue;
			else
				handle_windows_error(i_Locator); // this will end up throwing, and abort construction
		}

		break;
	}

	// save our work
	m_pImp = new fsFileStreamImp;
	m_pImp->m_File = new_file;
	m_pImp->m_Access = i_DesiredAccess;
	m_pImp->m_FilePointer = 0;
	m_pImp->m_Locator = i_Locator;
}

//========================================================================
//	Destructor
//========================================================================
fsFileStream::~fsFileStream()
{
	::CloseHandle(m_pImp->m_File);
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
	DBG_ASSERT0(m_pImp->m_Access != e_WriteOnly, "Wrong access type for Read");

	DWORD num_read = 0;
	BOOL ret_val = 0;

	ret_val = ::ReadFile(	m_pImp->m_File,
							o_Buffer,
							DWORD(i_NumBytes),
							&num_read,
							NULL);

	if ( ret_val == 0 )
	{
		// handle failure
		handle_windows_error(m_pImp->m_Locator);
	}
	else
	{
		m_pImp->m_FilePointer += int(num_read);
	}

	return int(num_read);
}

//========================================================================
//	Write writes i_NumBytes from o_Buffer into the file.  The file 
//	pointer will also be advanced.
//========================================================================
void fsFileStream::Write(int i_NumBytes, const void* i_Buffer)
{
	DBG_ASSERT0(m_pImp->m_Access != e_ReadOnly, "Wrong access type for Write");

	DWORD num_written = 0;
	BOOL ret_val = 0;

	ret_val = ::WriteFile(	m_pImp->m_File,
							i_Buffer,
							i_NumBytes,
							&(num_written),
							NULL);

	if ( ret_val == 0 )
	{
		// handle failure
		handle_windows_error(m_pImp->m_Locator);
	}
	else
	{
		m_pImp->m_FilePointer += int(num_written);
	}
}

//========================================================================
//	GetFilePos returns the current position of the file pointer.
//========================================================================
int fsFileStream::GetFilePos() const
{
	return m_pImp->m_FilePointer;
}

//========================================================================
//	GetLocator returns this filestream's associated locator
//========================================================================
const fsLocator& fsFileStream::GetLocator() const
{
	return m_pImp->m_Locator;
}


//========================================================================
//	SetFilePos sets the file pointer to the given value.  It is valid
//	to move the file pointer beyond the end of the file.
//========================================================================
void fsFileStream::SetFilePos(  int i_Pos, AccessPointType i_DesiredAccessPoint )
{
	DWORD ret_val = INVALID_SET_FILE_POINTER;
	DWORD move_method;

	if( i_DesiredAccessPoint == e_Beginning )
	{
		DBG_ASSERT0( i_Pos >= 0,  "Negative seek attempted" );
		move_method = FILE_BEGIN;
	}
	else if( i_DesiredAccessPoint == e_Current )
	{
		DBG_ASSERT0( m_pImp->m_FilePointer - i_Pos >= 0, "Negative seek attempted" );
		move_method = FILE_CURRENT;
	}
	else
	{
		move_method = FILE_END;
	}

	ret_val = ::SetFilePointer(	m_pImp->m_File,
								i_Pos,
								NULL,
								move_method );

	if ( ret_val == INVALID_SET_FILE_POINTER )
	{
		// handle failure
		handle_windows_error(m_pImp->m_Locator);
	}
	else
	{
		m_pImp->m_FilePointer = int(ret_val);
	}
	
}
