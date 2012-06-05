
#include "..\HairReader.h"

#include "Graphics/mdl/mdlHairInfo.hpp"

using namespace std;

namespace 
{
	int l_CurLine = 0;

	//line parsed variables
	int l_iStrand = 0;	//Strand number
	int l_iVert = 0;	//Vertex number in strand
	string l_Param;	//parameter name
	string l_String; //string
	int l_iVal = 0;	//single integer
	float l_fVal = 0;	//single float 
	maVector3d l_Vec3; //3 floats

	//Label, string
	bool fnReadString( stringstream& ss, mdlHairInfo* pInfo );
	//Label, int
	bool fnReadInt( stringstream& ss, mdlHairInfo* pInfo );
	//Strand, Label, float
	bool fnReadStrandFloat( stringstream& ss, mdlHairInfo* pInfo );
	//Strand, Label, Vector
	bool fnReadStrandVector( stringstream& ss, mdlHairInfo* pInfo );
	//Strand, Vertex, Label, Vector
	bool fnReadVertexVector( stringstream& ss, mdlHairInfo* pInfo );

	typedef enum
	{
		eName = 0,
		eNumHairs,
		eNumVertices,
		eNumHairVertices,
		eNumVerticesPerStrand,
		eRootRadii,
		eTipRadii,
		eRootColor,
		eTipColor,
		eSurfaceNormal,
		eOpacity,
		eSpecular,
		eGloss,
		eAmbientDiffuse,
		eVertex,
//		eVelocity,	
//		eUVW,
		eParseStateTypeNum
	} EParseStateType;

	struct ParseState
	{
		EParseStateType m_State;
		char* m_Label;
		bool (*m_Function)( stringstream& ss, mdlHairInfo* pInfo );
	};

	ParseState HairStates[eParseStateTypeNum] =
	{
		{eName, "Name:", fnReadString },
		{eNumHairs, "numHairs:", fnReadInt },
		{eNumVertices, "numVertices:", fnReadInt },
		{eNumHairVertices, "numHairVertices:", fnReadInt },
		{eNumVerticesPerStrand, "numVerticesPerStrand:", fnReadInt },
		{eRootRadii, "rootRadii:", fnReadStrandFloat },
		{eTipRadii, "tipRadii:", fnReadStrandFloat },
		{eRootColor, "rootColor:", fnReadStrandVector },
		{eTipColor, "tipColor:", fnReadStrandVector },
		{eSurfaceNormal, "surfaceNormal:", fnReadStrandVector },
		{eOpacity, "opacity:", fnReadStrandFloat },
		{eSpecular, "specular:", fnReadStrandFloat },
		{eGloss, "gloss:", fnReadStrandFloat },
		{eAmbientDiffuse, "ambDiff:", fnReadStrandFloat },
		{eVertex, "vertex:", fnReadVertexVector },
//		{eVelocity, "velocity:", fnReadVertexVector },
//		{eUVW, "uvw:", fnReadVertexVector }
	};

	ParseState* l_curState = HairStates;
	mdlHairVertex* l_curVertex = NULL;
	mdlHairStrand* l_curStrand = NULL;

	//Label, string
	bool fnReadString( stringstream& ss, mdlHairInfo* pInfo )
	{
		ss >> l_Param;
		ss >> l_String;
		if( l_Param != l_curState->m_Label ) return false;
		pInfo->m_HairName = l_String;
		return true;
	}

	//Label, int
	bool fnReadInt( stringstream& ss, mdlHairInfo* pInfo )
	{
		ss >> l_Param;
		ss >> l_iVal;
		if( l_Param != l_curState->m_Label ) return false;
		switch( l_curState->m_State )
		{
			case eNumHairs: pInfo->m_Strands.resize( l_iVal ); break;
			case eNumHairVertices: pInfo->m_nHairVertices = l_iVal; break;
			case eNumVerticesPerStrand: pInfo->m_nVerticesPerStrand = l_iVal; break;
		}
		return true;
	}

	//Strand, Label, float
	bool fnReadStrandFloat( stringstream& ss, mdlHairInfo* pInfo )
	{
		ss >> l_iStrand;
		ss >> l_Param;
		ss >> l_fVal;
		if( l_Param != l_curState->m_Label ) return false;	//check for matching label
		if( l_iStrand < 0 || l_iStrand >= pInfo->m_Strands.size() ) return false; //check for strand bounds

		l_curStrand = &pInfo->m_Strands[ l_iStrand ];	//set current strand

		switch( l_curState->m_State )
		{
			case eRootRadii: l_curStrand->Material.m_RootRadius = l_fVal; break;
			case eTipRadii: l_curStrand->Material.m_TipRadius = l_fVal; break;
			case eOpacity: l_curStrand->Material.m_Opacity = l_fVal; break;
			case eSpecular: l_curStrand->Material.m_Specular = l_fVal; break;
			case eGloss: l_curStrand->Material.m_Gloss = l_fVal; break;
			case eAmbientDiffuse: l_curStrand->Material.m_AmbientDiffuse = l_fVal; break;
		}
		return true;
	}
	//Strand, Label, Vector
	bool fnReadStrandVector( stringstream& ss, mdlHairInfo* pInfo )
	{
		ss >> l_iStrand;
		ss >> l_Param;
		ss >> l_Vec3.m_X;
		ss >> l_Vec3.m_Y;
		ss >> l_Vec3.m_Z;
		if( l_Param != l_curState->m_Label ) return false; //check for matching label

		if( l_iStrand < 0 || l_iStrand >= pInfo->m_Strands.size() ) return false; //check for strand bounds

		l_curStrand = &pInfo->m_Strands[ l_iStrand ];	//set current strand

		switch( l_curState->m_State )
		{
			case eRootColor: l_curStrand->Material.m_RootColor = l_Vec3; break;
			case eTipColor: l_curStrand->Material.m_TipColor = l_Vec3; break;
			case eSurfaceNormal: l_curStrand->Material.m_SurfaceNormal = l_Vec3; break;
		}

		return true;
	}
	//Strand, Vertex, Label, Vector
	bool fnReadVertexVector( stringstream& ss, mdlHairInfo* pInfo )
	{
		ss >> l_iStrand;
		ss >> l_iVert;
		ss >> l_Param;
		ss >> l_Vec3.m_X;
		ss >> l_Vec3.m_Y;
		ss >> l_Vec3.m_Z;
		if( l_Param != l_curState->m_Label ) return false;

		if( l_iStrand < 0 || l_iStrand >= pInfo->m_Strands.size() ) return false; //check for strand bounds
		l_curStrand = &pInfo->m_Strands[ l_iStrand ];	//set current strand

		//initialize strand control points
		if( l_curStrand->m_ControlPoints.size() == 0 ) l_curStrand->m_ControlPoints.resize( pInfo->m_nVerticesPerStrand );

		if( l_iVert < 0 || l_iVert >= pInfo->m_nVerticesPerStrand ) return false; //check for vertex bounds
		l_curVertex = &l_curStrand->m_ControlPoints[ l_iVert ];	//set current vertex

		switch( l_curState->m_State )
		{
			case eVertex: l_curVertex->Position = l_Vec3; break;
//			case eVelocity: l_curVertex->Velocity = l_Vec3; break;
//			case eUVW: l_curVertex->UVW = l_Vec3; break;
		}

		return true;
	}

	void NextState( int i_maxVerts )
	{
		switch( l_curState->m_State )
		{
		case eName:
			l_curState = &HairStates[ eNumHairs ];
			break;
		case eNumHairs:
			l_curState = &HairStates[ eNumVertices ];
			break;
		case eNumVertices:
			l_curState = &HairStates[ eNumHairVertices ];
			break;
		case eNumHairVertices:
			l_curState = &HairStates[ eNumVerticesPerStrand ];
			break;
		case eNumVerticesPerStrand:
			l_curState = &HairStates[ eRootRadii ];
			break;
		case eRootRadii:
			l_curState = &HairStates[ eTipRadii ];
			break;
		case eTipRadii:
			l_curState = &HairStates[ eRootColor ];
			break;
		case eRootColor:
			l_curState = &HairStates[ eTipColor ];
			break;
		case eTipColor:
			l_curState = &HairStates[ eSurfaceNormal ];
			break;
		case eSurfaceNormal:
			l_curState = &HairStates[ eOpacity ];
			break;
		case eOpacity:
			l_curState = &HairStates[ eSpecular ];
			break;
		case eSpecular:
			l_curState = &HairStates[ eGloss ];
			break;
		case eGloss:
			l_curState = &HairStates[ eAmbientDiffuse ];
			break;
		case eAmbientDiffuse:
			l_curState = &HairStates[ eVertex ];
			break;
		case eVertex:
/*			l_curState = &HairStates[ eVelocity ];
			break;
		case eVelocity:
			l_curState = &HairStates[ eUVW ];
			break;
		case eUVW:
			l_curState = &HairStates[ eVertex ];*/
			if( l_iVert >= (i_maxVerts-1) )	//all vertices accounted for, go to next strand
			{
				l_curState = &HairStates[ eRootRadii ];	
			}
			break;
		}
	}

	bool FindState()
	{
		int s = 0;
		while( s < eParseStateTypeNum )
		{
			if( HairStates[s].m_Label == l_Param )
			{
				l_curState = &HairStates[ s ];
				return true;
			}
			s++;
		}
		return false;
	}

	bool ParseLine( char *i_pzLine, mdlHairInfo &o_Info, std::stringstream &o_Errors )
	{
		stringstream ss (stringstream::in | stringstream::out);
		ss << i_pzLine;

		if( l_curState->m_Function( ss, &o_Info ) )	//parse expected state
		{
			NextState( o_Info.m_nVerticesPerStrand );
			return true;
		}
		else
		{
			//check for unexpected state
			if( FindState() )
			{
				o_Errors << "Unexpected parameter \"" << l_Param << "\" at line (" << l_CurLine << "), \"" << l_curState->m_Label << "\" expected." << endl;
				NextState( o_Info.m_nVerticesPerStrand );
				return true;
			}
			else
			{
				o_Errors << "Unknown parameter \"" << l_Param << "\" at line (" << l_CurLine << "), \"" << l_curState->m_Label << "\" expected." << endl;
			}
		}
		return false;
	}

}	//end namespace

//------------------------------------------------------------------------
//	Read in a mdlHairInfo from an ascii file using the testing file format
//------------------------------------------------------------------------
bool HairReader::ReadData( istream &i_File, mdlHairInfo &o_Info )
{
	const int bufLen = 1024;
	char lBuf[ bufLen ];

	l_CurLine = 0;
	l_curState = HairStates;
	l_curVertex = NULL;
	l_curStrand = NULL;

	bool bValid = true;
	std::stringstream errors;

	while( i_File.good() )
	{
		i_File.getline( lBuf, bufLen );
		if( !ParseLine( lBuf, o_Info, errors ) ) bValid = false;
		l_CurLine++;
	}
	return bValid;
}

