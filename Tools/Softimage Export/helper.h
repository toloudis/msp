//*****************************************************************************
/*!	\file helper.h
	\brief Helper classes for reading and writing mesh data.
	
	
*/
//*****************************************************************************

#ifndef _HELPER_H_
#define _HELPER_H_

#include <stdio.h>
#include <tchar.h>
#include <math.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <xsi_x3dobject.h>


#include "sgpuReportProgress.hpp"
class sgpuString;

class XSI::CString;
class XSI::CValue;
class XSI::CStatus;
class XSI::CRef;
class sgpuNode;

using namespace std;int SafeLongToInt( LONG l );

//*****************************************************************************
/*! \class CMeshFileWriter helper.h
	Helper class for writing mesh data to a text file.
	A mesh exported with CMeshFileWriter can be imported with CMeshFileReader.
 */
//*****************************************************************************

class CMeshFileWriter
{
	public:

	CMeshFileWriter();
	~CMeshFileWriter();

	XSI::CStatus Init(const XSI::CString& in_outFile);

	void Header( const XSI::CString& in_str );
	void Footer();
	void Write( const XSI::CString& in_str );
	void Write( const wchar_t* in_pstr );
	void Write( XSI::CValue in_val );	
	void EOL();
		
	private:
	FILE* p;
};

//*****************************************************************************
/*! \class CMeshFileReader helper.h
	Helper class for importing a mesh into Softimage from a text file generated 
	by CMeshFileWriter.
 */
//*****************************************************************************
class CMeshFileReader
{
	public:
	CMeshFileReader();
	~CMeshFileReader();

	XSI::CStatus Init(const XSI::CString& in_inFile);
	
	XSI::CStatus GotoSection(const XSI::CString& in_header);
	bool EndSection();
	bool IsEOF();

	LONG GetLongValues(LONG** out_pVals);
	LONG GetDoubleValues(double** out_pVals);
	
	private:
	const char* GetStringValues();
	const char* GetRow();

	char* m_pszRow;
	LONG* m_pLongs;
	double* m_pDoubles;
	FILE* p;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR* _tstr(const XSI::CString & xstr);

template< int NStaticSize , typename C >
struct CharacterBuffer
{
public:
	CharacterBuffer(int nSize):
	  m_pDynamicBuffer( NULL ),
		  m_Size( NStaticSize )
	  {
		  if( nSize > NStaticSize )
		  {
			  m_pDynamicBuffer = ( new unsigned char [ nSize ] );
			  m_Size = nSize;
		  }
	  }
	  ~CharacterBuffer()
	  {
		  if( m_pDynamicBuffer )
		  {
			  delete [] ( m_pDynamicBuffer );
		  }
	  }

	  C *GetBuffer()
	  {
		  unsigned char *retVal = ( m_pDynamicBuffer ) ? m_pDynamicBuffer : &m_StaticBuffer[0];
		  return reinterpret_cast< C * > ( retVal );
	  }
	  const C *GetBuffer() const
	  {
		  const unsigned char *retVal = ( m_pDynamicBuffer ).? m_pDynamicBuffer : &m_StaticBuffer[0];
		  return reinterpret_cast< const C* >( retVal );
	  }
	  int GetSize() const 
	  {
		  return m_Size;
	  }
	  unsigned char 		m_StaticBuffer[ NStaticSize ];
	  unsigned char			*m_pDynamicBuffer;
	  int m_Size;
};

inline bool EpsilonEqualZero( float a , float eps = 1.0e-6) { return fabsf(a) < eps; }


int GetWString( std::wstring &outWString, const sgpuString &sgpuInput );



struct AnimParams
{
	AnimParams():
m_StartFrame(0),
m_EndFrame(0),
m_StepFrame(0),
m_FrameRate(0)
{}
float	m_StartFrame;
float	m_EndFrame;
float	m_StepFrame;
float	m_FrameRate;
};


struct ReportProgress : public sgpuReportProgress
{
	ReportProgress( ){}
	void operator()( float i_fProgressSoFar )
	{
		int i=0;
	}
};

class Locator : private std::vector< std::wstring > 
{
public:
	Locator( const std::wstring &init );
	Locator( const Locator &other ):
		std::vector< std::wstring>(other)
	{
	}
	Locator & operator=( const Locator & other )
	{
		std::vector< std::wstring >::operator =( other );
		return *this;
	}
	bool operator==( const Locator &other ) const;
	void Push( const std::wstring &component )
	{
		push_back( component );
	}
	void Pop()
	{
		pop_back();
	}
	void Clear()
	{
		clear();
	}
	const std::wstring & GetLastName() const
	{
		assert( size() > 0 );
		return operator[]( size()-1 );
	}
	const std::wstring & GetName( int i )const;
	std::wstring ToPath() const;
	int GetNumNames() const;

};

XSI::CString GetSceneFilepath();
bool GetRelativePath( const XSI::CString &absParentpath, const XSI::CString & absArgpath, Locator &result );
void CopyFile(const XSI::CString& src, const XSI::CString& dst);
bool HasExportableGeometry( const XSI::X3DObject &xobj );
bool GetMasterInstance( const XSI::X3DObject & possibleInstance, XSI::CRef &ret );
sgpuNode FindNodeRec(  sgpuNode & node, const XSI::CString & name );
bool IsUsefulSgpuNode( const sgpuNode & node );


class TObjectsTobeExported
{
public:
	
	void PushBack( XSI::CRef & obj )
	{
		m_array.Add( obj );
	}

	LONG GetCount() const
	{
		return m_array.GetCount();
	}

	LONG GetIndex( const XSI::X3DObject xobj ) const
	{
		LONG retVal = -1;
		LONG nCount = m_array.GetCount();
		for( LONG i=0; i < nCount; ++i )
		{
			const XSI::X3DObject xobj_2( m_array[i] );
			if( xobj.GetUniqueName() == xobj_2.GetUniqueName() )
			{
				retVal = i;
			}
		}
		return retVal;
	}

	bool IsPresent( const XSI::X3DObject xobj ) const
	{
		return GetIndex( xobj ) >= 0;
	}

	void Append( const TObjectsTobeExported &toAdd )
	{
		LONG cnt = toAdd.m_array.GetCount();
		for( LONG i=0; i < cnt; ++i )
		{
			m_array.Add( toAdd.m_array[i] );
		}
	}
	//Re-order elements in the srcArray and put them in the dstArray
	//The sort order is determined by the order in which the items appear 
	//in the itemsInOrder array
	static void AddToArrayBasedOnSortIndex( XSI::CRefArray &srcArray, XSI::CRefArray &dstArray, TObjectsTobeExported &itemsInOrder );

private:
	XSI::CRefArray m_array;
};



class TransformStack
{
public:
	TransformStack( bool bExportAnimation, int curFrame ):
		m_bExportAnimation( bExportAnimation ),
		m_curFrame( curFrame )
		{
			Init();
		}
	
	void Cleanup()
	{
		m_sTransform.clear();
	}
	
	void Init()
	{
		Cleanup();
		XSI::MATH::CMatrix4 m;
		m.SetIdentity();
		m_sTransform.push_back( m );
	}

	virtual void PushStackWithCurrentObjectTm( const XSI::X3DObject &xobj );

	virtual void ApplyTmToSgpuNode ( sgpuNode & node );

	virtual void PopStackWithCurrentObjectTm( const XSI::X3DObject &xobj )
	{
		m_sTransform.pop_back();
	}

	const XSI::MATH::CMatrix4 &GetStackTopTM()
	{
		return m_sTransform.back();
	}

	std::vector <XSI::MATH::CMatrix4>	m_sTransform;
	const bool							m_bExportAnimation;
	double								m_curFrame;
protected:
	TransformStack( const TransformStack &other ):
	m_bExportAnimation( other.m_bExportAnimation ){}
	TransformStack & operator=( const TransformStack & other ) { return *this; }
};


class TransformStackForDeInstancing : public TransformStack
{
public:
	TransformStackForDeInstancing( const XSI::CRef &referencingObjRef, const XSI::CRef &referencedObjRef, bool bExportAnimation, double curFrame );	

	void PushStackWithCurrentObjectTm( const XSI::X3DObject &xobj );

	XSI::MATH::CMatrix4		m_reReferencingModulatorTm;
private:	
	TransformStackForDeInstancing( const TransformStackForDeInstancing &other ):TransformStack( other ) {}
	TransformStackForDeInstancing & operator=( const TransformStackForDeInstancing & other ) { return *this; }
};


#endif