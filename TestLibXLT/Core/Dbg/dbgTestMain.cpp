//#include <stdio.h>
//#include <windows.h>
//#include <stdlib.h>
#include <fstream>
//#include <iostream>
//#include <vector>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/dbg/dbgSystemInfo.hpp"
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

#define WINVER 0x500
#include <windows.h>

namespace
{
#define LOG_MSG( msg ) \
{ \
	std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); \
	for(int i = 0; i < _osList.size(); ++i){if(_osList[i]->isStreamActive())if(_osList[i]->logEnabled())*(dbgMsg::WriteLog( __FILE__, __LINE__ , _osList[i])) << msg << std::endl;} \
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDbgWrite2()
{
	//std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); 
	for(int j = 0; j < 50; j++)
	{
		//*(dbgMsg::WriteLog( __FILE__, __LINE__ , _osList[0])) << "Hello World" << j << std::endl;
		DBG_LOG("Hello World" << j << "hello" << 392373);
	}

	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDbgWrite()
{
	std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); 
	for(int i = 0; i < _osList.size(); ++i)
	{
		*(dbgMsg::WriteLog( __FILE__, __LINE__ , _osList[i])) << "Hello World" << std::endl;
	}
	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDbgMacro()
{
	int x = 5;
	LOG_MSG( "Hello " << "World" << " This" << " Is" << " a Test " << " plus " << x );		
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestToggles()
{
	dbgMsg::enableTimeStamp("default", true);
	LOG_MSG( "Show the time and date" << "." );
	dbgMsg::enableTimeStamp("default",false);

	dbgMsg::enableFileNameStamp("default",false);
	LOG_MSG( "No File name" << "." );
	dbgMsg::enableFileNameStamp("default",true);

	dbgMsg::enableFilePathStamp("default",true);
	LOG_MSG( "Now give the full file path" << "." );
	dbgMsg::enableFilePathStamp("default",false);

	dbgMsg::enableLevelStamp("default",false);
	LOG_MSG( "No debug level" << "." );
	dbgMsg::enableLevelStamp("default",true);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestAddOstream()
{
	dbgMsg::addStream("App2", new std::ofstream("App2.log"));
	LOG_MSG("This Message should appear in 2 streams ");

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestEnableStream()
{
	dbgMsg::enableDbgStream("App2", false);
	LOG_MSG("Now we will disable output to the second stream");

	dbgMsg::enableDbgStream("App2", true);
	LOG_MSG("and now we will enable output to the second file again");

}
void TestEnableMsgLevels()
{
	dbgMsg::enableStreamWarning("App2", false);
	dbgMsg::enableStreamError("default", false);
	DBG_LOG("default stream will not output Error messages");
	DBG_LOG("App2 stream will not output Warning messages");
	DBG_ERROR("This will appear only in App2");
	DBG_WARNING("This will appear only in the default stream");
	dbgMsg::enableStreamWarning("App2", true);
	dbgMsg::enableStreamError("default", true);
	DBG_LOG("restore both streams to enable warning and error messages");

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestRemoveOstream()
{
	dbgMsg::removeStream("App2");
	//LOG_MSG("Now completely remove the second stream");
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestConsoleOut()
{
	dbgMsg::addStream("console", dynamic_cast<std::ostream*>(&std::cout));
	DBG_LOG("This will appear in the file and in the console window");
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDbgLogMacro()
{
	DBG_LOG("This is a log");
	DBG_LOG0("This is an old version of debug log");


}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDbgWarningMacro()
{
	DBG_WARNING("This is a warning");
	DBG_WARNING0("This is an old version of debug warning");
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDbgErrorMacro()
{
	DBG_ERROR("This is an error");
	DBG_ERROR0("This is an old version of debug error");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestDbgTraceMacro()
{
	DBG_TRACE("This is a trace");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestStreamItString()
{
	itString cur_string("This is an itString");
	DBG_LOG(cur_string);
	DBG_LOG(cur_string << " accompanied with a normal string.");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestStreamFsLocator()
{
	itString it_string("myText.txt");
	fsLocator cur_locator;
	fsFileUtil::ANSIFilenameToLocator(std::string("X:here/there/everywhere/blah.txt"), cur_locator);
	DBG_LOG("The current locator path is: " << cur_locator);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestAssert()
{
	DBG_ASSERT((0 == 0), 1 << " does not equal " << 0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestUserInfo()
{
	TCHAR userID[256]; 
	DWORD _buff = 256;
	GetUserName(LPSTR(&userID), &_buff);
	DBG_LOG("User Name: " << userID);

	_buff = 256;
	GetComputerName(LPSTR(&userID), &_buff) ;
	DBG_LOG("Computer Name: " << userID);

	MEMORYSTATUS _memStatus;
	GlobalMemoryStatus(&_memStatus);
	DBG_LOG("Total RAM: " <<  _memStatus.dwTotalVirtual/(1024*1024) << "MB");

	OSVERSIONINFOEX _osInfo;
	_osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx ((OSVERSIONINFO *) &_osInfo);
	switch(_osInfo.dwMajorVersion)
	{
		case 6:
			if( _osInfo.dwMinorVersion == 0 )
			{
				if(	_osInfo.wProductType == VER_NT_WORKSTATION)
				{
					DBG_LOG("OS Version: Windows Vista, " << _osInfo.szCSDVersion);
				}
				else
				{
					DBG_LOG("OS Version: Windows Server 2008, " << _osInfo.szCSDVersion);
				}
			}
			break;
		case 5:
			if( _osInfo.dwMinorVersion == 2 )
			{
				DBG_LOG("OS Version: Windows Server 2003, " << _osInfo.szCSDVersion);
			}
			if( _osInfo.dwMinorVersion == 1 )
			{
				DBG_LOG("OS Version: Windows XP, " << _osInfo.szCSDVersion);
			}
			if( _osInfo.dwMinorVersion == 0 )
			{
				DBG_LOG("OS Version: Windows 2000, " << _osInfo.szCSDVersion);
			}
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestSystemInfo()
{
	DBG_LOG("printing out user information");
	dbgSystemInfo::logSystemInfo();
	DBG_LOG("DONE");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestLoop()
{
	std::string test = "TestString";
	for(int i = 0; i < 50; i++)
	{
		DBG_LOG(test << i << " is not " << test);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestLoop2()
{
	DBG_LOG(::GetUserName);
	for(int j = 0; j < 50; j++)
	{
		//*(dbgMsg::WriteLog( __FILE__, __LINE__ , _osList[0])) << "Hello World" << j << std::endl;
		DBG_LOG("Hello World   " << j << "   hello" << 392373);
	}
	//::LPTSTR d;
	//::LPDWORD size;
	//DBG_LOG(::GetUserName(d,size));
	
}

void TestSingleLineMacro()
{
	if(1==2)
		DBG_LOG("hello");
	else
		DBG_LOG("Goodbye");
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoTests()
{
	
	TestDbgWrite();
	TestDbgMacro();
	TestToggles();
	TestAddOstream();
	TestEnableStream();
	TestEnableMsgLevels();
    TestRemoveOstream();
	TestConsoleOut();
	TestDbgLogMacro();
	TestDbgWarningMacro();
	TestDbgErrorMacro();
	TestDbgTraceMacro();
	TestStreamItString();
	TestStreamFsLocator();
	TestAssert();
	//TestUserInfo();
	TestSystemInfo();
	TestSingleLineMacro();
	//TestLoop2();
}

} //end namespace


void main()
{
	envPackage::Init();
	dbgPackage::Init();
	fsPackage::Init();

	DoTests();

	fsPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();

}
