#include "CommonUtils.h"



#include <cassert>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <maya/MAnimControl.h>
#include <maya/MCommandResult.h>
#include <maya/MGlobal.h>


#undef CreateFile
#undef DeleteFile
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Core/gf/gfFileBin.hpp"
using namespace std;
#define SGPU_EPSILON_EQUAL_PRECISION  1.0e-5f

namespace sgpuMaya
{
	const char *c_ExporterVersion = "1.3.6.3";

	const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
	const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
	const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');
	const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');


	//========================================================================
	//return a copy of the input file name  with the new extension
	//========================================================================
	template<>
	string ChangeExtension< string >( const  string & i_FilePath, const string &i_NewExt )
	{

		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];

		strcpy_s( ext, i_NewExt.c_str() );
		_splitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);
		_makepath_s(newPath, drive, dir, fileName, i_NewExt.c_str());
		return string(newPath);

	}

	template<>
	wstring ChangeExtension< wstring >( const  wstring & i_FilePath, const wstring &i_NewExt )
	{

		wchar_t	newPath[ _MAX_DIR ];
		wchar_t	drive[ _MAX_DRIVE ];
		wchar_t	dir[ _MAX_DIR ];
		wchar_t	fileName[ _MAX_FNAME ];
		wchar_t	ext[ _MAX_EXT ];

		wcscpy_s( ext, i_NewExt.c_str() );
		_wsplitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);
		_wmakepath_s(newPath, drive, dir, fileName, i_NewExt.c_str());
		return wstring(newPath);

	}
	//========================================================================
	//return a copy of the input file name  with the new extension
	//========================================================================
	template<>
	string ChangeExtensionCustom< string >( const  string & i_FilePath, const string &i_NewExt )
	{

		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];

		strcpy_s( ext, i_NewExt.c_str() );
		_splitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);
		strcat_s( fileName, "_" );
		char *ext_1 = ext;
		if ( strlen( ext ) > 0 && ext[0] == '.' )
		{
			ext_1 = ext + 1;
		}
		strcat_s( fileName, ext_1 );
		_makepath_s(newPath, drive, dir, fileName, i_NewExt.c_str());
		return string(newPath);

	}

	template<>
	wstring ChangeExtensionCustom< wstring >( const  wstring & i_FilePath, const wstring &i_NewExt )
	{

		wchar_t	newPath[ _MAX_DIR ];
		wchar_t	drive[ _MAX_DRIVE ];
		wchar_t	dir[ _MAX_DIR ];
		wchar_t	fileName[ _MAX_FNAME ];
		wchar_t	ext[ _MAX_EXT ];

		wcscpy_s( ext, i_NewExt.c_str() );
		_wsplitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);		
		wcscat_s( fileName, L"_" );
		wchar_t *ext_1 = ext;
		if ( wcslen( ext ) > 0 && ext[0] == L'.' )
		{
			ext_1 = ext + 1;
		}
		wcscat_s( fileName, ext_1 );
		_wmakepath_s(newPath, drive, dir, fileName, i_NewExt.c_str());
		return wstring(newPath);

	}

	//========================================================================
	//return the file name sans the ext
	//Eg: c:/users/kgeorge/documents/3dsmax/scenes/foo.max"
	//returns foo
	//========================================================================
	template<>
	std::string GetFileTitle< std::string > ( const  std::string & i_FilePath )
	{
		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];

		_splitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);
		return std::string( fileName );
	}

	template<>
	std::wstring GetFileTitle< std::wstring > ( const  std::wstring & i_FilePath )
	{

		wchar_t	newPath[ _MAX_DIR ];
		wchar_t	drive[ _MAX_DRIVE ];
		wchar_t	dir[ _MAX_DIR ];
		wchar_t	fileName[ _MAX_FNAME ];
		wchar_t	ext[ _MAX_EXT ];

		_wsplitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);		
		return std::wstring( fileName );

	}


	//========================================================================
	//return the file name sans the ext
	//Eg: c:/users/kgeorge/documents/3dsmax/scenes/foo.max"
	//returns foo
	//========================================================================
	template<>
	std::string GetFileExt< std::string > ( const  std::string & i_FilePath )
	{
		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];

		_splitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);
		return std::string( ext );
	}

	template<>
	std::wstring GetFileExt< std::wstring > ( const  std::wstring & i_FilePath )
	{

		wchar_t	newPath[ _MAX_DIR ];
		wchar_t	drive[ _MAX_DRIVE ];
		wchar_t	dir[ _MAX_DIR ];
		wchar_t	fileName[ _MAX_FNAME ];
		wchar_t	ext[ _MAX_EXT ];

		_wsplitpath_s(i_FilePath.c_str(), drive, dir, fileName, ext);		
		return std::wstring( ext );

	}



	//=============================================================================	
	//A wrapper for governing the
	//life time semantics of opening
	//and closing of the exported file
	//=============================================================================

	SgpuFileWriterLifeTimeKeeper::SgpuFileWriterLifeTimeKeeper(const string &i_FilePath ):
	m_FilePath(i_FilePath ),
		m_pFile(NULL),
		m_pWriter(NULL)
	{
		fsLocator locator;
		itString itFilePath( i_FilePath.c_str()  );
		fsFileUtil::UnicodeStringToLocator( itFilePath,  locator);
		if( fsFileUtil::FileExists(locator) )
		{
			try 
			{
				fsFileUtil::DeleteFile(locator);
			}
			catch( ... )
			{
				stringstream ss;
				ss <<  "cant write to file %s" << m_FilePath;
				throw std::runtime_error( ss.str() );
			}
		}
		fsFileUtil::CreateFile(locator);
		m_pFile = new gfFileBin (locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		m_pFile->WriteHeader();		
		m_pWriter = new chBinWriter (*m_pFile);
	}

	SgpuFileWriterLifeTimeKeeper::~SgpuFileWriterLifeTimeKeeper()
	{
		Cleanup();
	}

	void SgpuFileWriterLifeTimeKeeper::Cleanup()
	{
		delete m_pWriter;
		delete m_pFile;
		m_pFile = NULL;
		m_pWriter = NULL;
		m_FilePath.clear();
	}

	//If a is approximately equal to b return true
	//see SGPU_EPSILON_EQUAL_PRECISION
	bool EpsilonEqual( float a, float b )
	{
		return fabs(b - a) < SGPU_EPSILON_EQUAL_PRECISION ;
	}


	//========================================================================
	//Maya routines, see CommonUtils.hpp
	//========================================================================
	void MCheck(MStatus &status, const char *pszFormat, ...)
	{
		const static  int va_buf_len = 1024;
		static char va_buf[ va_buf_len ];
		if (status != MStatus::kSuccess ) 
		{
			va_list vaArgs;
			va_start(vaArgs,  pszFormat);			
			int len=vsprintf_s(va_buf,pszFormat,vaArgs);
			assert( len <= va_buf_len );
			va_end(vaArgs);
			if ( va_buf[0] != '\0')
			{
				status.perror( va_buf );
			}
			throw status;
		}

	}


	void MCheck(bool bVal, const char *pszFormat, ...)
	{		
		const static  int va_buf_len = 1024;
		static char va_buf[ va_buf_len ];
		if ( !bVal ) 
		{
			MStatus status = MStatus::kFailure;
			va_list vaArgs;
			va_start(vaArgs,  pszFormat);			
			int len=vsprintf_s(va_buf,pszFormat,vaArgs);
			assert( len <= va_buf_len );
			va_end(vaArgs);
			if ( va_buf[0] != '\0')
			{
				status.perror( va_buf );
			}
			throw status;
		}

	}



	void MAssert(bool bVal, const char *pszFormat, ...)
	{		
		const static  int va_buf_len = 1024;
		static char va_buf[ va_buf_len ];
#if defined(_DEBUG)
		if ( !bVal ) 
		{
			MStatus status = MStatus::kFailure;
			va_list vaArgs;
			va_start(vaArgs,  pszFormat);			
			int len=vsprintf_s(va_buf, pszFormat,vaArgs);
			assert( len <= va_buf_len );
			va_end(vaArgs);
			if ( va_buf[0] != '\0')
			{
				status.perror( va_buf );
			}
			throw status;
		}
#endif
	}


	void MAssert( MStatus status, const char *pszFormat, ...)
	{		
		const static  int va_buf_len = 1024;
		static char va_buf[ va_buf_len ];
#if defined(_DEBUG)
		if ( status != MStatus::kSuccess ) 
		{
			va_list vaArgs;
			va_start(vaArgs,  pszFormat);			
			int len=vsprintf_s(va_buf,pszFormat,vaArgs);
			assert( len <= va_buf_len );
			va_end(vaArgs);
			if ( va_buf[0] != '\0')
			{
				status.perror( va_buf );
			}
			throw status;
		}
#endif
	}
	//Get start and endFrame numbers from MAnimControl
	void GetTimelineRangeFromAnimControl(double &o_Min, double &o_Max)
	{
		MAnimControl query_time;

		o_Min = query_time.animationStartTime().value();
		o_Max = query_time.animationEndTime().value();
	}

	//Get the current time from Maya
	MTime GetCurrentTime()
	{
		MString cmd("currentTime -query;");

		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		double frame = 0;
		MStatus status = result.getResult(frame);
		if ( !status ) 
		{
			cout << "Error getting current time.";
		}
		//cout << "Current frame: " << frame << endl;
		return MTime(frame, MTime::uiUnit());
	}
	
	//Set the current time in Maya
	void SetCurrentTime(MTime time)
	{
		double frame = time.value();
		char buffer[256];
		::sprintf_s(buffer, "currentTime -edit %f;", frame);

		MStatus status = MGlobal::executeCommand(MString(buffer));
		if ( !status ) 
		{
			cout << "Error setting current time.";
		}
	}
	
	//Get the current frame rate from Maya
	float GetCurrentFrameRate()
	{
		MTime::Unit uiUnit = MTime::uiUnit();
		//cout << "current uiUnit: "<< uiUnit << endl;
		// Define a time as one second
		MTime one_sec(1.0, MTime::kSeconds);
		// Then convert this time to uiUnits
		//cout << "value of uiUnits for one second: " << one_sec.as(uiUnit) << endl;
		return (float) one_sec.as(uiUnit);
	}
	
	
	//========================================================================
	// Add a chunk with the animation's frame rate to the chunk writer
	//========================================================================
	void WriteCurrentFrameRate( chWriter &o_Writer )
	{
		o_Writer.WriteChunkHeader( c_AFPS, 0, false );
		float fps = GetCurrentFrameRate();
		o_Writer.Write( fps );
		o_Writer.FinishChunk();
	}

	//========================================================================
	// Add a chunk with the animation's start frame number to the chunk writer
	//========================================================================
	void WriteBeginFrame(chWriter &o_Writer, float i_BeginFrame)
	{
		o_Writer.WriteChunkHeader(c_BGFR, 0, false);
		o_Writer.Write( i_BeginFrame );
		o_Writer.FinishChunk();
	}

	//========================================================================
	// Get the exporter version
	//========================================================================
	const char* GetExporterVersion()
	{
		return c_ExporterVersion;
	}


	//========================================================================
	// WriteExporterVesionStamp
	//========================================================================
	void WriteExporterVersionStamp(chWriter &o_Writer)
	{
		char date_string[64];
		char time_string[64];
		_strdate_s(date_string);
		_strtime_s(time_string);

		// Write as strings so readable from bin viewer
		//
		o_Writer.WriteChunkHeader(c_EXPV, 0, false);
		o_Writer.Write( GetExporterVersion() );
		o_Writer.Write(date_string);
		o_Writer.Write(time_string);
		o_Writer.FinishChunk();
	}

}