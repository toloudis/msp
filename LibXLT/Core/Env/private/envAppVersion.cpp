//****************************************************************************
//	envAppVersion.cpp
//
//		see .hpp
//
//	StudioGPU
//	Copyright(C) 2008-10 - All Rights Reserved
//****************************************************************************
#include "Core/env/envAppVersion.hpp"

#include <sstream>


//--------------------------------------------------------------------
//--------------------------------------------------------------------
envAppVersion::envAppVersion()
:	m_Major(0),
	m_Minor(0),
	m_Revision(0),
	m_Build(0)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envAppVersion::envAppVersion(int32_t i_vMaj,int32_t i_vMin,int32_t i_vRevision,int32_t i_vBuild)
:	m_Major(i_vMaj),
	m_Minor(i_vMin),
	m_Revision(i_vRevision),
	m_Build(i_vBuild)
{}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envAppVersion::envAppVersion(const std::string& i_VerString)
:	m_Major(0),
	m_Minor(0),
	m_Revision(0),
	m_Build(0)
{
	SetFromString( i_VerString );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envAppVersion::envAppVersion(const char* i_VerString)
:	m_Major(0),
	m_Minor(0),
	m_Revision(0),
	m_Build(0)
{
	std::istringstream str(i_VerString);
	char separator = '.';
	str >> m_Major >> separator >> m_Minor >> separator >> m_Revision >> separator >> m_Build;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envAppVersion::Set(int32_t i_vMaj,int32_t i_vMin,int32_t i_vRevision,int32_t i_vBuild)
{
	m_Major = i_vMaj;
	m_Minor = i_vMin;
	m_Revision = i_vRevision;
	m_Build = i_vBuild;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envAppVersion::Get(int32_t& o_vMaj, int32_t& o_vMin, int32_t& o_vRevision, int32_t& o_vBuild) const
{
	o_vMaj = m_Major;
	o_vMin = m_Minor;
	o_vRevision = m_Revision;
	o_vBuild = m_Build;
}
int32_t envAppVersion::GetMajor() const
{
	return m_Major;
}
int32_t envAppVersion::GetMinor() const
{
	return m_Minor;
}
int32_t envAppVersion::GetRevision() const
{
	return m_Revision;
}
int32_t envAppVersion::GetBuild() const
{
	return m_Build;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool envAppVersion::operator < (const envAppVersion& i_Other) const
{
	// check major version first, and then so on...
	if (m_Major < i_Other.m_Major)
		return true;
	if (m_Major > i_Other.m_Major)
		return false;
	
	if (m_Minor < i_Other.m_Minor)
		return true;
	if (m_Minor > i_Other.m_Minor)
		return false;
	
	if (m_Revision < i_Other.m_Revision)
		return true;
	if (m_Revision > i_Other.m_Revision)
		return false;
	
	if (m_Build < i_Other.m_Build)
		return true;
	if (m_Build > i_Other.m_Build)
		return false;
	
	// versions are the same!
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool envAppVersion::operator <= (const envAppVersion& i_Other) const
{
	// check major version first, and then so on...
	if (m_Major < i_Other.m_Major)
		return true;
	if (m_Major > i_Other.m_Major)
		return false;
	
	if (m_Minor < i_Other.m_Minor)
		return true;
	if (m_Minor > i_Other.m_Minor)
		return false;
	
	if (m_Revision < i_Other.m_Revision)
		return true;
	if (m_Revision > i_Other.m_Revision)
		return false;
	
	if (m_Build < i_Other.m_Build)
		return true;
	if (m_Build > i_Other.m_Build)
		return false;
	
	// versions are the same!
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool envAppVersion::operator > (const envAppVersion& i_Other) const
{
	// check major version first, and then so on...
	if (m_Major > i_Other.m_Major)
		return true;
	if (m_Major < i_Other.m_Major)
		return false;
	
	if (m_Minor > i_Other.m_Minor)
		return true;
	if (m_Minor < i_Other.m_Minor)
		return false;
	
	if (m_Revision > i_Other.m_Revision)
		return true;
	if (m_Revision < i_Other.m_Revision)
		return false;
	
	if (m_Build > i_Other.m_Build)
		return true;
	if (m_Build < i_Other.m_Build)
		return false;
	
	// versions are the same!
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool envAppVersion::operator >= (const envAppVersion& i_Other) const
{
	// check major version first, and then so on...
	if (m_Major > i_Other.m_Major)
		return true;
	if (m_Major < i_Other.m_Major)
		return false;
	
	if (m_Minor > i_Other.m_Minor)
		return true;
	if (m_Minor < i_Other.m_Minor)
		return false;
	
	if (m_Revision > i_Other.m_Revision)
		return true;
	if (m_Revision < i_Other.m_Revision)
		return false;
	
	if (m_Build > i_Other.m_Build)
		return true;
	if (m_Build < i_Other.m_Build)
		return false;
	
	// versions are the same!
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool envAppVersion::operator == (const envAppVersion& i_Other)
{
	// check major version first, and then so on...
	if (m_Major != i_Other.m_Major)
		return false;
	if (m_Minor != i_Other.m_Minor)
		return false;
	if (m_Revision != i_Other.m_Revision)
		return false;
	if (m_Build != i_Other.m_Build)
		return false;
	
	// versions are the same!
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envAppVersion::SetFromString( const std::string& i_VerString )
{
	std::istringstream str(i_VerString);
	char separator = '.';
	str >> m_Major >> separator >> m_Minor >> separator >> m_Revision >> separator >> m_Build;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
std::string envAppVersion::GetString() const
{
	std::ostringstream s;
	s << *this;
	return s.str();
}

//------------------------------------------------------------------------
//	operator << performs writing to an ostream
//------------------------------------------------------------------------
std::ostream& operator << (std::ostream& io_os, const envAppVersion& i_Version) 
{
	char separator = '.';
	io_os << i_Version.m_Major << separator << i_Version.m_Minor << separator << i_Version.m_Revision << separator << i_Version.m_Build;
	return io_os;
}

