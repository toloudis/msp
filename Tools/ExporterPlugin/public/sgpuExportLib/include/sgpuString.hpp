/****************************************************************************\
**  sgpuString.hpp
**
**      sgpuString.hpp defines a rudimentary string class for use in the SDK.
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_STRING_HPP
#define SGPU_STRING_HPP

#include "sgpuExportLib.hpp"

struct sgpuStringImpl;
class sgpuModelExportScene;
class sgpuMaterial;
class sguNode;
class sgpuPathReference;
class sgpuSubdiv;
class sgpuSubdivConstructor;
class sgpuMesh;
class sgpuMeshConstructor;
class sgpuPropertyValue;
//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuString
{
public:

	//========================================================================
	//	constructors, destructor, assignment operator
	//========================================================================
	sgpuString();	
	sgpuString( const sgpuString &i_Other );
	explicit sgpuString( const char *i_pzValue );
	explicit sgpuString( const wchar_t *i_pzWValue );
	~sgpuString();
	sgpuString& operator=(const sgpuString& i_CopyFrom);
	bool operator==( const sgpuString& i_Other )const;
	
	//========================================================================
	//	other functions
	//========================================================================
	/*
		Number of bytes required to represent this string
		as a zero delimited MBCS string
	*/
	int GetNumBytes_UTF8() const;
	/*
		Number of bytes required to represent this string
		as a zero delimited wchar_t string
	*/
	int GetNumBytes_WChart() const;
	/*
		copies the string as a MByte string to the buffer o_pzValue.
		o_pzValue should b as big as i_nBufferSizeInBytes.
		If i_nBufferSizeInBytes >= GetNumBytes_MByte(),
			then the string is converted and returned through the buffer
			Also returns, the number of bytes actually taken for representing
			the NULL terminated buffer string.
		else, return 0.
	*/
	int GetData_UTF8( char * o_pzValue, int i_nBufferSizeInBytes ) const;
	/*
		copies the string as a wchar_t string to the buffer o_pzValue.
		o_pzValue should b as big as i_nBufferSizeInBytes.
		If i_nBufferSizeInBytes >= GetNumBytes_WChart(),
			then the string is converted and returned through the buffer
			Also returns, the number of bytes actually taken for representing
			the NULL terminated buffer string.
		else, return 0.
	*/
	int GetData_WChart( wchar_t * o_pzValue, int i_nBufferSize ) const;
	friend sgpuModelExportScene;
	friend sgpuMaterial;
	friend sguNode;
	friend sgpuPathReference;
	friend sgpuSubdiv;
	friend sgpuSubdivConstructor;
	friend sgpuMesh;
	friend sgpuMeshConstructor;
	friend sgpuPropertyValue;
public:
	sgpuStringImpl *m_pImpl;
};

#endif // #ifndef SGPU_STRING_HPP
