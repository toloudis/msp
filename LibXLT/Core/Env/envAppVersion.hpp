/****************************************************************************\
**	envAppVersion.hpp
**
**		envAppVersion.hpp supplies a class that can be used to version software.
**	The implementation is 4 integers: major, minor, revision, and build numbers.
**
**	Major.Minor.Revision.Build
**
**	StudioGPU
**	Copyright(C) 2008-10 - All Rights Reserved
\****************************************************************************/
#ifdef ENV_APPVERSION_HPP
#error envAppVersion.hpp multiply included
#endif
#define ENV_APPVERSION_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class envAppVersion
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	envAppVersion();
	envAppVersion(int32_t i_vMaj,int32_t i_vMin,int32_t i_vRevision,int32_t i_vBuild);
	explicit envAppVersion(const std::string& i_VerString);
	explicit envAppVersion(const char* i_VerString);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set(int32_t i_vMaj,int32_t i_vMin,int32_t i_vRevision,int32_t i_vBuild);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Get(int32_t& o_vMaj, int32_t& o_vMin, int32_t& o_vRevision, int32_t& o_vBuild) const;
	int32_t GetMajor() const;
	int32_t GetMinor() const;
	int32_t GetRevision() const;
	int32_t GetBuild() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool operator < (const envAppVersion& i_Other) const;
	bool operator <= (const envAppVersion& i_Other) const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool operator > (const envAppVersion& i_Other) const;
	bool operator >= (const envAppVersion& i_Other) const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool operator == (const envAppVersion& i_Other);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetFromString( const std::string& i_VerString );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	std::string GetString() const;

	//------------------------------------------------------------------------
	//	operator << performs writing to an ostream
	//------------------------------------------------------------------------
	friend std::ostream& operator << (std::ostream& io_os, const envAppVersion& i_Version);

private:
	int32_t m_Major, m_Minor, m_Revision, m_Build;
};
