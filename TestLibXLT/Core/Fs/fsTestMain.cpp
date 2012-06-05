#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envError.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsFilePosSaver.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsPackage.hpp"
#include "Core/fs/fsFileEnum.hpp"
#include "Core/fs/fsXMLReader.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Core/Gf/gfPackage.hpp"

#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <direct.h>

using namespace std;

namespace
{
//============================================================================
//============================================================================
class ListingTarget : public fsFileEnum::EnumTarget
{
	public:
		virtual bool Notify(const fsLocator& i_Directory, const fsLocator& i_File);
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool ListingTarget::Notify(const fsLocator& i_Directory, const fsLocator& i_File)
{
	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_File, filename);
	itString directory;
	fsFileUtil::LocatorToUnicodeString(i_Directory, directory);

	DBG_LOG("  file name: (" << std::setw( 26 ) << filename << ") directory: " << directory );
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestFileExists()
{
	DBG_LOG("Test File Exists");
	ListingTarget target;
	bool bExists;
	fsLocator dir;
	dir.Push(itString(L"."));
	dir.Push(itString(L"TestDir"));
	dir.Push(itString(L"TestFile.txt"));

	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	dir.Pop();
	dir.Push(itString(L"BlahBlah.txt"));
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File " << dir << " -- " << (bExists ? "Exists":"Does not exist"));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDirExists()
{
	DBG_LOG("Test Directory Exists");
	ListingTarget target;
	bool bExists;
	fsLocator dir;
	dir.Push(itString(L"."));
	dir.Push(itString(L"TestDir"));

	bExists = fsFileUtil::DirectoryExists( dir );
	DBG_LOG("  File " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	dir.Pop();
	dir.Push(itString(L"BlahBlah"));
	bExists = fsFileUtil::DirectoryExists( dir );
	DBG_LOG("  File " << dir << " -- " << (bExists ? "Exists":"Does not exist"));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestFileUNC()
{
	DBG_LOG("Test File UNC");
	ListingTarget target;
	itString full_path;
	fsLocator dir;
	bool bExists;

	full_path = itString("\\?\\C:\\Projects\\SourceCode\\TestLibXLT\\Core\\Fs\\TestDir\\TestFile.txt");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (local 2 back slashes) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	full_path = itString("\\?\\UNC\\Sgpuserver\MachStudio Pro\Install Versions\DVD art\MSP_CD_engrave_dark.jpg");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (2 back slashes) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	full_path = itString("\\\\?\\UNC//Sgpuserver/MachStudio Pro/Install Versions/DVD art/MSP_CD_engrave_dark.jpg");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (2 fwd slashes) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	full_path = itString("\\\\?\\UNC\\Sgpuserver\\MachStudio Pro\\Install Versions\\DVD art\\MSP_CD_engrave_dark.jpg");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (4 back slashes) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	full_path = itString("\\\\?\\UNC\\Sgpu069\\DriveC\\Temp\\ext18866\\install.exe");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (4 back slashes) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	full_path = itString("\\\\Sgpu069\\DriveC\\Temp\\ext18866\\install.exe");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (4 back slashes - no UNC tag) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	full_path = itString("\\Sgpu069\\DriveC\\Temp\\ext18866\\install.exe");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (2 back slashes - no UNC tag) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));

	full_path = itString("//Sgpu069/DriveC/Temp/ext18866/install.exe");
	fsFileUtil::UnicodeStringToLocator( full_path, dir );
	bExists = fsFileUtil::FileExists( dir );
	DBG_LOG("  File (2 fwd slashes - no UNC tag) " << dir << " -- " << (bExists ? "Exists":"Does not exist"));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestFileEnum()
{
	DBG_LOG("All files in C:");
	ListingTarget target;
	fsLocator dir;
	dir.Push(itString(L"C:"));
	fsFileEnum::EnumerateFiles(dir, target);

	DBG_LOG("  All files containg '.BAT' in  C:");
	itString substring(L".BAT");
	fsFileEnum::EnumerateFiles(dir, target, substring);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDirEnum()
{
	DBG_LOG("All directories in C:");
	ListingTarget target;
	fsLocator dir;
	dir.Push(itString(L"C:"));
	fsFileEnum::EnumerateDirectories(dir, target);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestFilePosSaver()
{
	DBG_LOG("Testing FilePosSaver");
	fsLocator test_file;
	//test_file.Push(itString("."));
	//test_file.Push(itString("C:"));
	//test_file.Push(itString("Projects"));
	//test_file.Push(itString("fsTest"));
	//test_file.Push(itString("fsTest.bin"));
	//DBG_LOG("   file " << test_file );

	wchar_t buff[_MAX_PATH];
	if (_wgetcwd(buff, _MAX_PATH) == NULL)
		DBG_ERROR("Cannot get cwd");

	itString buffer(buff);
	DBG_LOG("    cwd " << buffer);
	fsFileUtil::UnicodeStringToLocator( buffer, test_file );
	test_file.Push(itString(L"fsTest.bin"));
	DBG_LOG("   file " << test_file );

	DBG_LOG("All files in " << buffer);
	ListingTarget target;
	fsLocator dir;
	fsFileUtil::UnicodeStringToLocator( buffer, dir );
	fsFileEnum::EnumerateFiles(dir, target);

	DBG_LOG("   creating test" );
	try
	{
		fsFileUtil::CreateFile(test_file);
	}
	catch (const fsFileDoesntExistX& ex)
	{
		DBG_ERROR("fsFileDoesntExistX");
	}
	catch (const fsDirectoryDoesntExistX& ex)
	{
		DBG_ERROR("fsDirectoryDoesntExistX");
	}
	catch (const fsInvalidLocatorX& ex)
	{
		DBG_ERROR("fsInvalidLocatorX");
	}
	catch (const fsFileInUseX& ex)
	{
		DBG_ERROR("fsFileInUseX");
	}
	catch (const fsFileExistsX& ex)
	{
		DBG_ERROR("fsFileExistsX");
	}
	catch (const fsDirectoryNotEmptyX& ex)
	{
		DBG_ERROR("fsDirectoryNotEmptyX");
	}
	catch (const fsReadOnlyX& ex)
	{
		DBG_ERROR("fsReadOnlyX");
	}
	catch (const fsDiskFullX& ex)
	{
		DBG_ERROR("fsDiskFullX");
	}
	catch (const fsUnknownX& ex)
	{
		DBG_ERROR("fsUnknownX");
	}

	DBG_LOG("   writing test" );

	fsFileStream* write_stream = new fsFileStream(test_file, fsFileStream::e_WriteOnly);

	int i;
	for( i = 0 ; i < 1000 ; i++ )
	{
		envType::Int16 val = i;
		write_stream->Write(2, &val);
	}

	delete write_stream;

	DBG_LOG("   reading test" );
	try
	{
		fsFileStream* read_stream = new fsFileStream(test_file, fsFileStream::e_ReadOnly);

		DBG_LOG("		running PosSaver test" );

		//	pick 2 random positions in the file, set the file position to the first one
		//	then create a saver, set the 2nd position, then delete the saver.
		//	on deleting the saver, the file should go back to the first position.
		//
		for( int test_num = 0 ; test_num < 100 ; test_num++ )
		{
			int first_pos = (rand() % 2000) / 2;
			int second_pos = (rand() % 2000) / 2;

			read_stream->SetFilePos(first_pos);
			
			fsFilePosSaver<fsFileStream>* saver = new fsFilePosSaver<fsFileStream>(*read_stream);
			read_stream->SetFilePos(second_pos);
			delete saver;

			DBG_ASSERT( read_stream->GetFilePos() == first_pos, "Position not restored correctly");
		}
		delete read_stream;
	}
	catch (const fsFileDoesntExistX& ex)
	{
		DBG_ERROR("fsFileDoesntExistX");
	}
	catch (const fsDirectoryDoesntExistX& ex)
	{
		DBG_ERROR("fsDirectoryDoesntExistX");
	}
	catch (const fsInvalidLocatorX& ex)
	{
		DBG_ERROR("fsInvalidLocatorX");
	}
	catch (const fsFileInUseX& ex)
	{
		DBG_ERROR("fsFileInUseX");
	}
	catch (const fsFileExistsX& ex)
	{
		DBG_ERROR("fsFileExistsX");
	}
	catch (const fsDirectoryNotEmptyX& ex)
	{
		DBG_ERROR("fsDirectoryNotEmptyX");
	}
	catch (const fsReadOnlyX& ex)
	{
		DBG_ERROR("fsReadOnlyX");
	}
	catch (const fsDiskFullX& ex)
	{
		DBG_ERROR("fsDiskFullX");
	}
	catch (const fsUnknownX& ex)
	{
		DBG_ERROR("fsUnknownX");
	}

	DBG_LOG("   delete test" );

	try
	{
		fsFileUtil::DeleteFile(test_file);
	}
	catch (const fsFileInUseX& ex)
	{
		DBG_ERROR("fsFileInUseX - file locked");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestReadOnlyFlag()
{
	DBG_LOG("Testing ReadOnly");

	fsLocator read_only_file;
//	read_only_file.Push(L"..");
	read_only_file.Push(itString(L"fsTest.sln"));

	//DBG_ASSERT0(fsFileUtil::IsReadOnly(read_only_file), "fsTest.sln should be read-only");

	fsLocator read_write_file;
	read_write_file.Push(itString(L"TestDir"));
	read_write_file.Push(itString(L"TestFile.txt"));
	if (!fsFileUtil::IsReadOnly(read_write_file))
		DBG_LOG( "File: " << read_write_file.GetLastName() << " is read-write");
	else
		DBG_ERROR( "File: " << read_write_file.GetLastName() << " is NOT read-write");

	read_write_file.Pop();
	read_write_file.Push(itString(L"TestFile-ReadOnly.txt"));
	if (fsFileUtil::IsReadOnly(read_write_file))
		DBG_LOG( "File: " << read_write_file.GetLastName() << " is read only");
	else
		DBG_ERROR( "File: " << read_write_file.GetLastName() << " is NOT read only");
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestXMLRead()
{
	DBG_LOG("-------XML read start");

	char exePath[256];
	int PathLen = 0;
	PathLen = GetModuleFileNameA(NULL, exePath, 256);

	std::string fname("TestXML.cfg");
	fsXMLReader xmlrdr(fname);
	xmlrdr.Open();

	std::string key, text;
	fsXMLData::fs_Node_Type node_type;

	while ( (node_type = xmlrdr.ReadNode(key, text)) != fsXMLData::e_EOF )
	{
		switch( node_type )
		{
		case fsXMLData::e_Element:
			DBG_LOG("Element " << text.c_str());
			break;
		case fsXMLData::e_EndElement:
			DBG_LOG("End Element " << text.c_str());
			break;
		case fsXMLData::e_Text:
			DBG_LOG("Text " << text.c_str());
			break;
		case fsXMLData::e_None:
			DBG_LOG("None " << text.c_str());
			break;
		}
	}

	xmlrdr.Close();

	DBG_LOG("-------XML read end");
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestXMLWrite()
{
	DBG_LOG("-------XML write start");
//> wchar []Result;
//> Result.length = 256;
//> //Result.length = GetModuleFileNameW( Module, cast( wchar * )Result,
//>Result.length ) / wchar.sizeof;
 
	wchar_t exePath[256];
	int PathLen = 0;
	PathLen = GetModuleFileName(NULL, exePath, 256);
	itString exe_path(exePath);
	fsLocator filename;
	fsFileUtil::UnicodeStringToLocator(exe_path, filename);
//	filename.Pop();
	filename.Pop();
	filename.Push(itString(L"TextXMLwrite.cfg"));

	if (!fsFileUtil::FileExists(filename))
		fsFileUtil::CreateFile(filename);
		
	fsXMLWriter xmlwrtr(filename);
	xmlwrtr.Open();

	xmlwrtr.WriteStartElement(std::string("Root"));

	int var_int = 100;	xmlwrtr.WriteElement(std::string("var_int"), var_int);
	bool var_bool = false;	xmlwrtr.WriteElement(std::string("var_bool"), var_bool);
	short var_short = 100;	xmlwrtr.WriteElement(std::string("var_short"), var_short);
	float var_float = 123.456f;	xmlwrtr.WriteElement(std::string("var_float"), var_float);
	const char * var_constchar = "text value"; xmlwrtr.WriteElement(std::string("var_const char"), *var_constchar);
	const std::string var_conststring("const string");	xmlwrtr.WriteElement(std::string("var_conststring"), var_conststring);
	std::string var_string("string");	xmlwrtr.WriteElement(std::string("var_string"), var_string);
	fsLocator var_fsLocator(itString("locator"));	xmlwrtr.WriteElement(std::string("var_fsLocator"), var_fsLocator);
	envType::Int8 var_Int8 = -100;	xmlwrtr.WriteElement(std::string("var_Int8"), var_Int8);
	envType::UInt8 var_UInt8 = 100;	xmlwrtr.WriteElement(std::string("var_UInt8"), var_UInt8);
	envType::UInt16 var_UInt16 = 100;	xmlwrtr.WriteElement(std::string("var_UInt16"), var_UInt16);
	envType::UInt32 var_UInt32 = 100;	xmlwrtr.WriteElement(std::string("var_UInt32"), var_UInt32);
	envType::UInt64 var_UInt64 = 100;	xmlwrtr.WriteElement(std::string("var_UInt64"), var_UInt64);
	envType::Float64 var_Float64 = 123.456;	xmlwrtr.WriteElement(std::string("var_Float64"), var_Float64);

	xmlwrtr.WriteEndElement();

	xmlwrtr.Close();

	DBG_LOG("-------XML write end");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestXML()
{
	TestXMLRead();
	TestXMLWrite();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoTests()
{
	try
	{
		TestFileExists();
		TestDirExists();
		TestFileUNC();
		TestFileEnum();
		TestDirEnum();
		TestFilePosSaver();
		TestReadOnlyFlag();
		TestXML();
	}
	catch (const fsReadOnlyX& roe)
	{
		DBG_LOG("Locator is invalid " << roe.GetLocator());
	}
	catch (const fsInvalidLocatorX& ile)
	{
		DBG_LOG("Locator is invalid" << ile.GetLocator());
	}
	catch (const fsFileDoesntExistX& fdee)
	{
		DBG_LOG("file does not exist. " << fdee.GetLocator());
	}
}

}


void main()
{
	DBG_LOG("Init - gf");
	gfPackage::Init(); // unfortunately needs to be first
	DBG_LOG("Init - env");
	envPackage::Init();
	DBG_LOG("Init - dbg");
	dbgPackage::Init();
	dbgMsg::addStream(std::wstring(L"Log2"), new std::ofstream(L"Log2.log"));
	DBG_LOG("Init - fs");
	fsPackage::Init();

	DBG_LOG("Starting fs tests");

	DoTests();

	DBG_LOG("Finished fs tests");
	DBG_LOG0("TEST of OLD DBG MESSAGE");

	DBG_LOG("CleanUp - fs");
	fsPackage::CleanUp();
	DBG_LOG("CleanUp - dbg");
	dbgPackage::CleanUp();
	DBG_LOG("CleanUp - env");
	envPackage::CleanUp();
	DBG_LOG("CleanUp - gf");
	gfPackage::CleanUp();

	DBG_LOG("Program finished.");
}
