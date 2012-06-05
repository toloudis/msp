#include <stdio.h>
#include <windows.h>
#include <memory.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envError.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsPackage.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/gf/gfFileX.hpp"
#include "Core/gf/gfPackage.hpp"
#include "Core/gf/gfTextSetFile.hpp"
#include "Core/gf/gfPakFile.hpp"


//============================================================================
//============================================================================
namespace
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
enum UserDefGamePaths
{
	e_MyPath1 = gfPaths::e_FirstUserDefPath,
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoGamePathsTests()
{
	fsLocator locNewPath;
	locNewPath.Push(itString("C:"));
	locNewPath.Push(itString("gfTestDir"));
	std::string strLogging;
	
	gfPaths::SetPath((gfPaths::Paths)e_MyPath1, locNewPath);
	fsFileUtil::LocatorToANSIFilename(gfPaths::GetPath((gfPaths::Paths)e_MyPath1), strLogging);
	DBG_LOG1("First User Defined game path set to: %s", strLogging.c_str());

	fsFileUtil::LocatorToANSIFilename(gfPaths::GetPath(gfPaths::e_ExePath), strLogging);
	DBG_LOG1("Executable game path set to: %s", strLogging.c_str());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoFileBinTests()
{
	std::string strLogging;
	fsLocator locFile1(gfPaths::GetPath(gfPaths::e_ExePath));
	locFile1.Push(itString("locFile1.txt"));
	fsFileUtil::LocatorToANSIFilename(locFile1, strLogging);
	DBG_LOG1("LocFile1 locator set to: %s", strLogging.c_str());
	fsLocator locFile2(gfPaths::GetPath(gfPaths::e_ExePath));
	locFile2.Push(itString("locFile2.txt"));
	fsFileUtil::LocatorToANSIFilename(locFile2, strLogging);
	DBG_LOG1("LocFile2 locator set to: %s", strLogging.c_str());

	gfFileBin TestFile1(locFile1, fsFileStream::e_ReadWriteCreate);

	TestFile1.WriteHeader(2, 3);
	TestFile1.SetFilePos(0);
	gfFileBin::Header rHeader1;
	memset(&rHeader1, 0, sizeof(gfFileBin::Header));
	TestFile1.ReadHeader(&rHeader1);
	DBG_LOG4("The values of rHeader1 are: %d, %d, %d, %d", rHeader1.m_Mode, rHeader1.m_Major, rHeader1.m_Minor, rHeader1.m_Rev);

	gfFileBin TestFile2(locFile2, fsFileStream::e_ReadWriteCreate, gfFileBin::e_LittleEndian);

	gfFileBin::Header wHeader;
	memset(&wHeader, 0, sizeof(gfFileBin::Header));
	TestFile2.WriteHeader(wHeader);
	TestFile2.SetFilePos(0);

	gfFileBin::Header rHeader2;
	memset(&rHeader2, 0, sizeof(gfFileBin::Header));
	TestFile2.ReadHeader(&rHeader2);
	DBG_LOG4("The values of rHeader2 are: %d, %d, %d, %d", rHeader2.m_Mode, rHeader2.m_Major, rHeader2.m_Minor, rHeader2.m_Rev);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoFileUtilTests()
{
	envType::UInt8 i_TestValLE = 47;
	envType::UInt8 o_TestValLE = 0;
	envType::UInt8 i_TestValBE = 12;
	envType::UInt8 o_TestValBE = 0;
	std::string strLogging;
	fsLocator locFile3(gfPaths::GetPath(gfPaths::e_ExePath));
	locFile3.Push(itString("locFile3.txt"));
	fsFileUtil::LocatorToANSIFilename(locFile3, strLogging);
	DBG_LOG1("LocFile3 locator set to: %s", strLogging.c_str());
	fsLocator locFile4(gfPaths::GetPath(gfPaths::e_ExePath));
	locFile4.Push(itString("locFile4.txt"));
	fsFileUtil::LocatorToANSIFilename(locFile4, strLogging);
	DBG_LOG1("LocFile4 locator set to: %s", strLogging.c_str());

	gfFileBin TestFile3(locFile3, fsFileStream::e_ReadWriteCreate, gfFileBin::e_LittleEndian);

	TestFile3.WriteHeader(4, 5);
	DBG_LOG1("i_TestValLE: %d", i_TestValLE);
	gfFileUtil::Write(TestFile3, i_TestValLE);

	TestFile3.SetFilePos(0);
	TestFile3.ReadHeader();
	gfFileUtil::Read(TestFile3, o_TestValLE);
	DBG_LOG1("o_TestValLE: %d", o_TestValLE);

	gfFileBin TestFile4(locFile4, fsFileStream::e_ReadWriteCreate, gfFileBin::e_BigEndian);

	TestFile4.WriteHeader();
	DBG_LOG1("i_TestValBE: %d", i_TestValBE);
	gfFileUtil::Write(TestFile4, i_TestValBE);

	TestFile4.SetFilePos(0);
	TestFile4.ReadHeader();
	gfFileUtil::Read(TestFile4, o_TestValBE);
	DBG_LOG1("o_TestValBE: %d", o_TestValBE);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoTextSetFileTests()
{
	int LastSetIndex;
	std::string strLogging;
	fsLocator locFile5(gfPaths::GetPath(gfPaths::e_ExePath));
	locFile5.Push(itString("locFile5.txt"));
	fsFileUtil::LocatorToANSIFilename(locFile5, strLogging);
	DBG_LOG1("LocFile5 locator set to: %s", strLogging.c_str());

	if( !fsFileUtil::FileExists(locFile5) )
		fsFileUtil::CreateFile(locFile5);

	gfTextSetFile *TestFile5 = new gfTextSetFile(locFile5,false);
	TestFile5->MakeEntry(0, itString("TextFrag0"));
	LastSetIndex = 0;
	DBG_LOG2("Text Fragment at index %d set to: %s", LastSetIndex, dbgLog::UnicodetoANSI(TestFile5->GetText(LastSetIndex).GetString(), TestFile5->GetText(LastSetIndex).GetLength()).c_str());

	DBG_LOG1("Specifying index of %d for new text fragment", LastSetIndex + 1);
	TestFile5->MakeEntry(LastSetIndex + 1, itString("TextFrag#2"));
	LastSetIndex = 1;
	DBG_LOG2("Text Fragment at index %d set to: %s", LastSetIndex, dbgLog::UnicodetoANSI(TestFile5->GetText(LastSetIndex).GetString(), TestFile5->GetText(LastSetIndex).GetLength()).c_str());

	delete TestFile5;
	gfTextSetFile *TestFile6 = new gfTextSetFile(locFile5,false);
	DBG_LOG2("Preexisting text fragments: %s: %s", dbgLog::UnicodetoANSI(TestFile6->GetText(0).GetString(), TestFile6->GetText(0).GetLength()).c_str(), dbgLog::UnicodetoANSI(TestFile6->GetText(1).GetString(), TestFile6->GetText(1).GetLength()).c_str());

	TestFile6->MakeEntry(LastSetIndex + 1, itString("3 text fragment"));
	LastSetIndex = 2;
	DBG_LOG2("Text Fragment at index %d set to: %s", LastSetIndex, dbgLog::UnicodetoANSI(TestFile6->GetText(LastSetIndex).GetString(), TestFile6->GetText(LastSetIndex).GetLength()).c_str());

	TestFile6->SetText(itString("newer, longer TextFrag0"), 0);
	delete TestFile6;
	gfTextSetFile *TestFile7 = new gfTextSetFile(locFile5,false);
	DBG_LOG3("Preexisting text fragments: %s: %s: %s", dbgLog::UnicodetoANSI(TestFile7->GetText(0).GetString(), TestFile7->GetText(0).GetLength()).c_str(), dbgLog::UnicodetoANSI(TestFile7->GetText(1).GetString(), TestFile7->GetText(1).GetLength()).c_str(), dbgLog::UnicodetoANSI(TestFile7->GetText(2).GetString(), TestFile7->GetText(2).GetLength()).c_str());

	TestFile7->ClearText(0);
	delete TestFile7;
	gfTextSetFile TestFile8(locFile5);
	DBG_LOG1("NumFrags %d", TestFile8.GetNumFragments());
	DBG_LOG3("Preexisting text fragments: %s: %s: %s", dbgLog::UnicodetoANSI(TestFile8.GetText(0).GetString(), TestFile8.GetText(0).GetLength()).c_str(), dbgLog::UnicodetoANSI(TestFile8.GetText(1).GetString(), TestFile8.GetText(1).GetLength()).c_str(), dbgLog::UnicodetoANSI(TestFile8.GetText(2).GetString(), TestFile8.GetText(2).GetLength()).c_str());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoPakFileTests()
{
	int FileSize;
	std::string ToPrint;
	char * Buf;
	char * CompBuf;
	fsLocator TheLocator;
	fsLocator FileOnDisk;
	fsLocator Base(gfPaths::GetPath(gfPaths::e_ExePath));
	fsLocator zipFile = Base;
	zipFile.Push("..");
	zipFile.Push("PakTestUnComp.zip");
	gfPakFile * ThePak;
	ThePak = new gfPakFile(zipFile);
	DBG_LOG1("NumFilesInPakFile %d", ThePak->GetNumFiles());
	gfFileInPak * TheFile;
	TheFile = new gfFileInPak(ThePak);

	while ( TheFile->IsValid())
	{
		FileOnDisk = Base;
		DBG_LOG0("The File is Valid");
		TheLocator = TheFile->GetName();
		fsFileUtil::LocatorToANSIFilename(TheLocator, ToPrint);
		DBG_LOG1("Name of File is %s", ToPrint.c_str());
		FileSize = TheFile->GetSize();
		DBG_LOG1("Size of File is %d", FileSize);
		// for this test, a file size of zero indicates that the
		// file is one of the folder files WinZip creates that 
		// we won't be able to match on disk. 
		if (FileSize == 0)
		{
			++(*TheFile);
			continue;
		}
		// create file for actual file to compare
		FileOnDisk.Push(TheLocator);
		gfFileBin CompFile(FileOnDisk, fsFileStream::e_ReadOnly);
		Buf = new char [FileSize];
		CompBuf = new char [FileSize];
		TheFile->ReadFromFile(Buf, FileSize);
		CompFile.Read(FileSize, CompBuf);
		if ((memcmp(Buf, CompBuf, FileSize)) != 0)
		{
			DBG_LOG0("Values are not equal!");
		}
		delete [] Buf;
		delete [] CompBuf;
		++(*TheFile);
	}

	DBG_LOG0("The File is Not Valid (End of List of Files)");
	delete ThePak;
	delete TheFile;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoTests()
{
	std::string strLogging;

	try
	{
		DoGamePathsTests();
		DoFileBinTests();
		DoFileUtilTests();
		DoTextSetFileTests();

		// TODO - fix pak files so they work again!
//		DoPakFileTests();
	}	//WARNING! you'll need to set the locFile*.txt files to be writable and move them to your exe dir
	catch (const fsReadOnlyX& roe)
	{
		fsFileUtil::LocatorToANSIFilename(roe.GetLocator(), strLogging);
		DBG_LOG1("Locator: %s, is invalid", strLogging.c_str());
	}
	catch (const fsInvalidLocatorX& ile)
	{
		fsFileUtil::LocatorToANSIFilename(ile.GetLocator(), strLogging);
		DBG_LOG1("Locator: %s, is invalid", strLogging.c_str());
	}
	catch (const fsFileDoesntExistX& fdee)
	{
		fsFileUtil::LocatorToANSIFilename(fdee.GetLocator(), strLogging);
		DBG_LOG1("file: %s, does not exist.", strLogging.c_str());
	}
	catch (const gfFileNotInPakX& fnip)
	{
		fsFileUtil::LocatorToANSIFilename(fnip.GetLocator(), strLogging);
		DBG_LOG1("file: %s, does not exist in pakfile.", strLogging.c_str());
	}
}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void main()
{
	envPackage::Init();
	dbgPackage::Init();
	fsPackage::Init();
	gfPackage::Init();

	DoTests();

	gfPackage::CleanUp();
	fsPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
}

