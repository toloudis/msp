
#include <string>
#include <deque>

#include "Core/ma/maVector3d.hpp"

class maVector3;

namespace hair
{
	

	class HairInfo
	{
	public:
		HairInfo( int nStrands, int nHairVertices, int nVerticesPerStrand );
		~HairInfo(){}
		std::string m_sHairNodeName;
		int m_nStrands;
		int m_nHairVertices;
		int m_nVerticesPerStrand;
		std::deque< float > m_RootRadii;
		std::deque< float > m_TipRadii;
		std::deque< maVector3d > m_RootColor;
		std::deque< maVector3d > m_TipColor;
		std::deque< maVector3d > m_SurfaceNormal;
		std::deque< float > m_Opacity;
		std::deque< float > m_Specular;
		std::deque< float > m_Gloss;
		std::deque< float > m_AmbDiff;
		std::deque< maVector3d > m_Vertices;
	};
}