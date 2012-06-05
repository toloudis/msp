/****************************************************************************\
**  sgpuUtilsImpl.hpp
**
**      sgpuUtilsImpl.hpp defines private implementation of common utilities
**		used in the exporter SDK
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_UTILSIMPL_HPP
#define SGPU_UTILSIMPL_HPP

#include "sgpuExportLib.hpp"
#include "sgpuConstructor.hpp"

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <sstream>
#include <vector>
#include <string>
bool fEpsilonEqual( float a, float b );


extern const char *c_ExportLib_Version;

struct UniqueVertex
{
	UniqueVertex() :	m_NormalIndex(-1),
		m_UVIndex(-1),
		m_PosIndex(-1), 
		m_Index(-1){}

	UniqueVertex(	int i_PosIndex,
		int i_NormalIndex,
		int i_UVIndex, int i_Index) :	m_PosIndex(i_PosIndex),
		m_NormalIndex(i_NormalIndex),
		m_UVIndex(i_UVIndex), m_Index(i_Index){}

	bool operator == (const UniqueVertex& rhs) const 
	{	
		return	(m_NormalIndex == rhs.m_NormalIndex) &&
			(m_UVIndex ==  rhs.m_UVIndex) &&
			(m_PosIndex == rhs.m_PosIndex); 
	}

	bool operator < (const UniqueVertex& rhs) const
	{
		if( m_PosIndex == rhs.m_PosIndex )
		{
			if( m_NormalIndex == rhs.m_NormalIndex )
				return m_UVIndex < rhs.m_UVIndex;
			else
				return m_NormalIndex < rhs.m_NormalIndex;
		}
		else
			return m_PosIndex < rhs.m_PosIndex;
	}

	int m_NormalIndex;
	int m_UVIndex;
	int m_PosIndex;
	int m_Index;
};


struct PolyFace 
{
	PolyFace(int nSides, const sgpuMaterial &i_Mtl);
	PolyFace( const PolyFace &i_Other );
	PolyFace & operator=( const PolyFace& i_Other );
	shared_ptr< std::vector<sgpuConstructor::FaceVertex> > m_FaceVertices;
	sgpuMaterial m_Mtl;
};

template< int NStaticSize >
struct CharacterBuffer
{
public:
	CharacterBuffer(int nSize):
	  m_pDynamicBuffer( NULL ),
		  m_Size( NStaticSize )
	  {
		  if( nSize > NStaticSize )
		  {
			  m_pDynamicBuffer = new envType::UInt8 [ nSize ];
			  m_Size = nSize;
		  }
	  }
	  ~CharacterBuffer()
	  {
		  if( m_pDynamicBuffer )
		  {
			  delete [] m_pDynamicBuffer;
		  }
	  }

	  envType::UInt8 *GetBuffer()
	  {
		  return ( m_pDynamicBuffer ) ? m_pDynamicBuffer : &m_StaticBuffer[0];
	  }
	  const envType::UInt8 *GetBuffer() const
	  {
		  return ( m_pDynamicBuffer ).? m_pDynamicBuffer : &m_StaticBuffer[0];
	  }
	  int GetSize() const 
	  {
		  return m_Size;
	  }
	  envType::UInt8 m_StaticBuffer[ NStaticSize ];
	  envType::UInt8 *m_pDynamicBuffer;
	  int m_Size;
};

#define NO_IMPL_EXCEPTION(a, b) if( ( a == NULL ) ) {\
	std::stringstream ss;\
	ss << "no implementation of " << b << " present";\
	throw sgpuException( sgpuString( ss.str().c_str() ) );\
}

#define INVALID_RANGE_EXCEPTION(a, b, c) if( ( a ) < 0 && ( a ) >= ( b ) ) { \
	std::stringstream ss;\
	ss << a << "not in the range"  << 0 << "-" << b << "context: " << c;\
	throw sgpuException( sgpuString( ss.str().c_str() ) );\
}

#define REPORT_OBSOLETE( a ) {\
}

#define ITER_END_EXCEPTION( a, b, c, d) if( (a) == (b) )\
{\
	std::stringstream ss;\
	ss << c << " "  << d  << "doesnt not exist";\
	throw sgpuException( sgpuString( ss.str().c_str() ) );\
}
#endif // #ifndef SGPU_UTILSIMPL_HPP

template <  class T >
maPoint3d Point3d(T &i_Vec)
{
	return maPoint3d((float)i_Vec[0], (float)i_Vec[1], (float)i_Vec[2]);
}