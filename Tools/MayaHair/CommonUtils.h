#if !defined(COMMON_UTILS_HPP_)
#define COMMON_UTILS_HPP_

#include <maya/MStatus.h>
#include <string>


class gfFileBin;
class chBinWriter;
class MTime;
class chWriter;

namespace sgpuMaya
{
	//utility file wrapper that closes the file upon destruction
	template< typename F > 
	struct FileWrapper
	{
		FileWrapper( const MString &i_Filename ):
	m_File( i_Filename.asChar() )
	{

	}
	~FileWrapper()
	{
		if( m_File.is_open() ) {
			m_File.close();
		}
	}
	F m_File;
	};

	//return the input fie name with the new extension
	template< class Str >
	Str ChangeExtension( const  Str & i_FilePath, const Str &i_NewExt );
	template<>
	std::string ChangeExtension< std::string > ( const  std::string & i_FilePath, const std::string  &i_NewExt );
	template<>
	std::wstring ChangeExtension< std::wstring > ( const  std::wstring & i_FilePath, const std::wstring  &i_NewExt );

	//If the input file is <file path dir>/<file name>.<ext>,
	//return the string <file path dir>/<file name>_<ext>.<new ext>
	template< class Str >
	Str ChangeExtensionCustom( const  Str & i_FilePath, const Str &i_NewExt );
	template<>
	std::string ChangeExtensionCustom< std::string > ( const  std::string & i_FilePath, const std::string  &i_NewExt );
	template<>
	std::wstring ChangeExtensionCustom< std::wstring > ( const  std::wstring & i_FilePath, const std::wstring  &i_NewExt );

	//return the file name sans the ext
	//Eg: c:/users/kgeorge/documents/3dsmax/scenes/foo.max"
	//returns foo
	template< class Str >
	Str GetFileTitle( const  Str & i_FilePath );
	template<>
	std::string GetFileTitle< std::string > ( const  std::string & i_FilePath );
	template<>
	std::wstring GetFileTitle< std::wstring > ( const  std::wstring & i_FilePath );


	//return the file extension 
	//Eg: c:/users/kgeorge/documents/3dsmax/scenes/foo.Max"
	//returns .Max
	template< class Str >
	Str GetFileExt( const  Str & i_FilePath );
	template<>
	std::string GetFileExt< std::string > ( const  std::string & i_FilePath );
	template<>
	std::wstring GetFileExt< std::wstring > ( const  std::wstring & i_FilePath );



	//A wrapper for governing the
	//life time semantics of opening
	//and closing of a sgpu bin file
	struct SgpuFileWriterLifeTimeKeeper
	{
		SgpuFileWriterLifeTimeKeeper(const std::string &i_FilePath );
		~SgpuFileWriterLifeTimeKeeper();
		void Cleanup();
		gfFileBin *m_pFile;
		chBinWriter *m_pWriter;		
		std::string		m_FilePath;
	};

	//If a is approximately equal to b return true
	//see SGPU_EPSILON_EQUAL_PRECISION
	bool EpsilonEqual( float a, float b );


	
//------------------------------------------------------------------------
	//Maya routines
//------------------------------------------------------------------------
	//If status != MStatus::kSuccess,
	// 1) does status.perror( < the input vargs message >
	// 2) throws status as an exception

	void MCheck(MStatus &status, const char *pszFormat, ...);


	//If !bVal,
	// status = MStatus::kFailure;
	// 1) does status.perror( < the input vargs message >
	// 2) throws status as an exception

	void MCheck(bool bVal, const char *pzFormat, ...);

	//If !bVal && _DEBUG is defined
	// status = MStatus::kFailure;
	// 1) does status.perror( < the input vargs message >
	// 2) throws status as an exception

	void MAssert(bool bVal, const char *pzFormat, ...);	

	//If status != MStatus::kSuccess && _DEBUG is defined
	// 1) does status.perror( < the input vargs message >
	// 2) throws status as an exception
	void MAssert( MStatus status, const char *pzFormat, ...);

	//Get start and endFrame numbers from MAnimControl
	void GetTimelineRangeFromAnimControl(double &o_Min, double &o_Max);

	//Get the current time from Maya
	MTime GetCurrentTime();

	//set the current time in Maya
	void SetCurrentTime(MTime time);

	//get the current frame rate from Maya and write it
	void WriteCurrentFrameRate( chWriter &o_Writer );

	//write the current beginFrame rate
	void WriteBeginFrame(chWriter &o_Writer, float i_BeginFrame);

	//write the current export version
	void WriteExporterVersionStamp( chWriter &o_Writer );

}
#endif
