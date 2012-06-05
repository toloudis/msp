
#include <iostream>
#include <fstream>
#include "HairReader.h"



using namespace std;

namespace hair
{

	HairInfo::HairInfo( int nStrands, int nHairVertices, int nVerticesPerStrand ):
		m_nStrands( nStrands ),
		m_nHairVertices( nHairVertices ),
		m_nVerticesPerStrand( nVerticesPerStrand )
{

	m_RootRadii.resize( m_nStrands );
	m_TipRadii.resize( m_nStrands );
	m_RootColor.resize( m_nStrands );
	m_TipColor.resize( m_nStrands );
	m_SurfaceNormal.resize( m_nStrands );
	
	m_Opacity.resize( m_nStrands );
	
	m_Specular.resize( m_nStrands );
	
	m_Gloss.resize( m_nStrands );
	
	m_AmbDiff.resize( m_nStrands );

	m_Vertices.resize( m_nHairVertices + m_nStrands * 2  );
	DBG_ASSERT0( (m_nHairVertices >= m_nVerticesPerStrand * m_nStrands), "logical inconsistency , should be  (m_nHairVertices >= m_nVerticesPerStrand * m_nStrands)" );

}

}
