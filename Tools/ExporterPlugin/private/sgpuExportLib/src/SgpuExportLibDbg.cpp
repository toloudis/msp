#include <cassert>

#include "Core/Dbg/DbgAssert.hpp"
#include "Core/Dbg/DbgStream.hpp"
#include "Core/Dbg/DbgMsg.hpp"
#include "Core/Dbg/DbgLog.hpp"


using namespace std;
#if _MSC_VER < 1400
//============================================================================
//============================================================================
namespace dbgAssert
{
	void AssertMessage(const char* i_File, int i_Line, const char* i_Format, ...)
	{
		assert( false );
	}
}
namespace dbgMsg
{	std::ostream* WriteLog( std::wstring& i_File, int i_Line, dbgStream* io_Stream )
	{	

		return io_Stream->GetStream();
	}

//------------------------------------------------------------------------
	//  If an assertion call is passed, run this function
	//
    //------------------------------------------------------------------------
	void AssertMsg(std::wstring& i_File, int i_Line)
	{
		assert( false );
	}
}
namespace dbgMsg
{
	namespace 
	{		
		std::stringstream		m_AssertStream;
		std::vector<dbgStream*> m_StreamList;
	}

	std::ostream* WriteWarning(std::wstring& i_File, int i_Line, dbgStream* io_Stream )
	{
		
		return io_Stream->GetStream();
	}

	std::ostream* WriteError(std::wstring& i_File, int i_Line, dbgStream* io_Stream )
	{
		return io_Stream->GetStream();
	}

	std::ostream* WriteTrace(std::wstring& i_File, int i_Line, dbgStream* io_Stream )
	{
		return io_Stream->GetStream();
	}
	std::stringstream& GetAssertStream()
	{
		return m_AssertStream;
	}

	std::vector<dbgStream*> GetStreamList()
	{
		return m_StreamList;
	}
}

bool dbgStream::isStreamActive()
{
	return m_IsActive;
}
namespace dbgLog
{

	void Write(const char* i_Format, ...)
	{
		
	}

	//------------------------------------------------------------------------
	void WriteLog(std::wstring& i_File, int i_Line, const char* i_Format, ...)
	{

	}

	void WriteWarning(std::wstring& i_File, int i_Line, const char* i_Format, ...)
	{

	}
	void WriteError(std::wstring& i_File, int i_Line, const char* i_Format, ...)
	{
	}


}

//---------------------------
// Get the can write log flag
//---------------------------
bool dbgStream::logEnabled()
{
	return false;
}

//---------------------------
// get the warning flag
//---------------------------
bool dbgStream::warningEnabled()
{
	return false;
}

//---------------------------
// get the write error flag
//---------------------------
bool dbgStream::errorEnabled()
{
	return false;
}

//---------------------------
// get the write trace flag
//---------------------------
bool dbgStream::traceEnabled()
{
	return false;
}


//---------------------------
// return the stream belonging to this instance
//---------------------------
std::ostream* dbgStream::GetStream()
{
	return m_Stream;
}
#endif