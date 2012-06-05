
#include <iostream>
#include <fstream>
#include <exception>
#include <sstream>

#include "HairReader.h"


using namespace std;
using namespace hair;
#define VERIFY_FIELD_NAME( token, name )\
	DBG_ASSERT0( (token == name), "expecting a "##name )


namespace
{
	//exception for parsing args
	class hrException :public std::runtime_error
	{
	public:
		hrException( const std::string & desc ):std::runtime_error( desc ){}

	};
	//exception for parsing args
	class hrSuccess :public std::runtime_error
	{
	public:
		hrSuccess( const std::string & desc ):std::runtime_error( desc ){}

	};

	//states of the parser
	//ie: first expect the name
	//then expect the numHairs,
	//then expect the numHairVertices etc..
	typedef enum {eUnknown = -1, eInit=0, eName, eNumHairs, eNumHairVertices, eRootRadii, eTipRadii, eRootColor, eTipColor, eSurfaceNormal,eOpacity, eSpecular, eGloss, eAmbDiff, eNumVerticesPerStrand, eVertex } EParseHairState;

	//state machine for parsing
	EParseHairState NextState ( EParseHairState curState )
	{
		EParseHairState nextState = eUnknown;
		switch( curState )
		{
		case eInit:
			nextState = eName;
			break;
		case eName:
			nextState = eNumHairs;
			break;
		case eNumHairs:
			nextState = eNumHairVertices;
			break;
		case eNumHairVertices:
			nextState =eNumVerticesPerStrand;
			break;
		case eNumVerticesPerStrand:
			nextState = eRootRadii;
			break;
		case eRootRadii:
			nextState = eTipRadii;
			break;
		case eTipRadii:
			nextState = eRootColor;
			break;
		case eRootColor:
			nextState = eTipColor;
			break;
		case eTipColor:
			nextState =eSurfaceNormal;
			break;
		
		case eSurfaceNormal:
			nextState = eOpacity;
			break;

		case eOpacity:
			nextState = eSpecular;
			break;

		
		case eSpecular:
			nextState = eGloss;
			break;
		
		case eGloss:
			nextState = eAmbDiff;
			break;
		
		case eAmbDiff:
			nextState = eVertex;
			break;

		case eVertex:
			nextState = eVertex;
			break;
		default:
			break;
		}
		return nextState;
	}

	//Read in the name and the initial size declarations of the hairInfo
	EParseHairState ReadInitData ( 
		EParseHairState i_CurState, 
		istream &i_File, 
		string &o_name,
		int &o_nStrands,
		int &o_nHairVertices,
		int &o_nVerticesPerStrand,
		int &o_lineNum
		)
	{

		const int bufLen = 1024;
		char lBuf[ bufLen ];
		std::string temp;
		EParseHairState curState = i_CurState;
		if( curState != eInit )
		{
			return eUnknown;
		}
		curState = NextState( curState );
		bool bCurStateRead = false;
		while( i_File.good() )
		{
		
			stringstream ss (stringstream::in | stringstream::out);
			bCurStateRead = false;
			i_File.getline( lBuf, bufLen );
			++o_lineNum;
			ss << lBuf;
			switch( curState )
			{
			case eName:
				ss >> temp;
				if( temp != "Name:" )
				{
					break;
				}
				VERIFY_FIELD_NAME( temp, "Name:" );
				ss >>  o_name;
				bCurStateRead = true;
				break;
			case eNumHairs:
				ss >> temp;		
				if( temp != "numHairs:" )
				{
					break;
				}
				VERIFY_FIELD_NAME( temp, "numHairs:" );
				ss >> o_nStrands;
				bCurStateRead = true;
				break;
			case eNumHairVertices:
				ss >>temp;
				if( temp != "numHairVertices:" )
				{
					break;
				}
				VERIFY_FIELD_NAME( temp, "numHairVertices:" );
				ss >> o_nHairVertices;
				bCurStateRead = true;
				break;
			case eNumVerticesPerStrand:
				ss >> temp;		
				if( temp != "numVerticesPerStrand:" )
				{
					break;
				}
				VERIFY_FIELD_NAME( temp, "numVerticesPerStrand:" );
				ss >> o_nVerticesPerStrand;
				bCurStateRead = true;
				return curState;
			default:
				break;
			}
			if ( bCurStateRead )
			{
				curState = NextState( curState );
			}
		}
		return eUnknown;
	}

//another higher level state for parsing
struct HairReadState 
	{
		HairReadState():
			m_CurState( eRootRadii ),
			m_nCurStrand(0),
			m_nCurVertex(0)
			{}
		EParseHairState m_CurState;
		int m_nCurStrand;
		int m_nCurVertex;
	};


//main parsing engine
void ReadData( HairInfo *i_pHInfo, char *i_pzLine,  HairReadState &i_State )
{

	string temp;
	int iTemp;
	int iVTemp;
	float fTemp0, fTemp1, fTemp2;
	stringstream ss (stringstream::in | stringstream::out);
	ss << i_pzLine;
	int vertexIndexInHInfo=0;
	switch( i_State.m_CurState )
	{
	case eRootRadii:
		ss >> iTemp;
		ss >> temp;
		if( temp != "rootRadii:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "rootRadii:" );
		ss >> fTemp0;
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_RootRadii.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_RootRadii.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_RootRadii[ i_State.m_nCurStrand ] =  fTemp0;
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;
	case eTipRadii:		
		ss >> iTemp;
		ss >> temp;
		if( temp != "tipRadii:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "tipRadii:" );
		ss >> fTemp0;		
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_TipRadii.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_TipRadii.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_TipRadii[ i_State.m_nCurStrand ] = fTemp0;		
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;
	case eRootColor:			
		ss >> iTemp;
		ss >> temp;
		if( temp != "rootColor:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "rootColor:" );
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		ss >> fTemp0;
		ss >> fTemp1;
		ss >> fTemp2;
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_RootColor.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_RootColor.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_RootColor[ i_State.m_nCurStrand ]= maVector3d( fTemp0, fTemp1, fTemp2 );		
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;
	
	case eTipColor:	
		ss >> iTemp;
		ss >> temp;
		if( temp != "tipColor:" )
		{
			break;
		}
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		VERIFY_FIELD_NAME( temp, "tipColor:" );
		ss >> fTemp0;
		ss >> fTemp1;
		ss >> fTemp2;
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_TipColor.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_TipColor.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_TipColor[ i_State.m_nCurStrand ] =  maVector3d( fTemp0, fTemp1, fTemp2 ) ;		
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;
	case eSurfaceNormal:	
		ss >> iTemp;
		ss >> temp;
		if( temp != "surfaceNormal:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "surfaceNormal:" );
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		ss >> fTemp0;
		ss >> fTemp1;
		ss >> fTemp2;
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_TipColor[ i_State.m_nCurStrand ] = maVector3d( fTemp0, fTemp1, fTemp2 ) ;		
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;
	
	case eOpacity:	
		ss >> iTemp;
		ss >> temp;
		if( temp != "opacity:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "opacity:" );
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		ss >> fTemp0;
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_Opacity[ i_State.m_nCurStrand ] = fTemp0;
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;
	
	case eSpecular:	
		ss >> iTemp;
		ss >> temp;
		if( temp != "specular:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "specular:" );
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		ss >> fTemp0;
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_Specular[ i_State.m_nCurStrand ] = fTemp0;
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;
	
	case eGloss:	
		ss >> iTemp;
		ss >> temp;
		if( temp != "gloss:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "gloss:" );
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		ss >> fTemp0;
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_Gloss[ i_State.m_nCurStrand ] = fTemp0;
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;

		
	case eAmbDiff:	
		ss >> iTemp;
		ss >> temp;
		if( temp != "ambDiff:" )
		{
			break;
		}
		VERIFY_FIELD_NAME( temp, "ambDiff:" );
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		ss >> fTemp0;
		//DBG_ASSERT0( ( static_cast< int > ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand ),\
		//	"Logican inconsistency ( i_pHInfo->m_SurfaceNormal.size() ) == i_State.m_nCurStrand )"\
		//	);
		i_pHInfo->m_AmbDiff[ i_State.m_nCurStrand ] = fTemp0;
		i_State.m_CurState = NextState( i_State.m_CurState );
		break;

	case eVertex:
		ss >> iTemp;
		ss >> iVTemp;
		ss >> temp;		
		if( temp != "vertex:" )
		{
			break;
		}
		DBG_ASSERT0( (iTemp == i_State.m_nCurStrand ), "Logical inconsistency, (iTemp == i_State.m_nCurStrand )");
		DBG_ASSERT0( (iVTemp == i_State.m_nCurVertex ), "Logical inconsistency, (iTemp == i_State.m_nCurVertex )");
		VERIFY_FIELD_NAME( temp, "vertex:" );
		ss >> fTemp0;
		ss >> fTemp1;
		ss >> fTemp2;
		vertexIndexInHInfo = ( i_State.m_nCurStrand * (i_pHInfo->m_nVerticesPerStrand + 2) );
		vertexIndexInHInfo += ( i_State.m_nCurVertex == 0) ? 0 :( i_State.m_nCurVertex+1 ) ;
		{
			int nVerts1 = static_cast< int > ( i_pHInfo->m_Vertices.size() );
			int nVerts2 = ( i_State.m_nCurStrand * (i_pHInfo->m_nVerticesPerStrand + 2) ) + ( ( i_State.m_nCurVertex == 0) ? 0 :i_State.m_nCurVertex+1 );
			//DBG_ASSERT0( ( nVerts1 == nVerts2), "Logican inconsistency, nVerts1 == nVerts2" );
		}
		i_pHInfo->m_Vertices[ vertexIndexInHInfo ] = maVector3d( fTemp0, fTemp1, fTemp2 );
		if( i_State.m_nCurVertex == 0 || i_State.m_nCurVertex == i_pHInfo->m_nVerticesPerStrand - 1)
		{
			//If it is the first or the last vertex of the strand
			//inset another copy
			i_pHInfo->m_Vertices[vertexIndexInHInfo+1 ] = maVector3d( fTemp0, fTemp1, fTemp2 ) ;
		}
		++i_State.m_nCurVertex;
		if( i_pHInfo->m_nVerticesPerStrand == i_State.m_nCurVertex )
		{
			//reset for next strand
			i_State.m_nCurVertex = 0;
			++i_State.m_nCurStrand;
			i_State.m_CurState = eRootRadii;
		} else
		{
			i_State.m_CurState = NextState( i_State.m_CurState );
		}
		break;
	default:
		break;
	}
	
	}
}


HairInfo * ParseHairFile( istream &i_File )
{
	const int bufLen = 1024;
	char lBuf[ bufLen ];
	HairInfo *pHInfo = NULL;
	std::string sname;
	EParseHairState curState = eInit;
	string  hairName;
	int nStrands=0;
	int nHairVertices=0;
	int nVerticesPerStrand=0;
	int lineNum=0;
	curState = ReadInitData ( 
		curState,
		i_File, 
		hairName,
		nStrands,
		nHairVertices,
		nVerticesPerStrand,
		lineNum
		);
	if ( curState != eNumVerticesPerStrand )
	{
		cout << "error in parsing the init data of hair" ;
		return pHInfo;
	}
	if( nStrands <= 0 || nHairVertices <=0 || nVerticesPerStrand <=0 )
	{
		cout << "error in parsing the init data of hair" ;
		return pHInfo;
	}
	pHInfo = new HairInfo( nStrands, nHairVertices, nVerticesPerStrand );

	stringstream ss;
	HairReadState hreadState;
	while( i_File.good() )
	{
		i_File.getline( lBuf, bufLen );
		ReadData( pHInfo, lBuf, hreadState );
		if( hreadState.m_CurState == eUnknown )
		{
			cout << "parse error happened while rading line " << lineNum;
			break;
		}
	}
	return pHInfo;
}



void Usage ( ostream &os )
{
	os << "Usage: HairReader -i <filePath>" << endl;
	os << "		: HairReader -h" << endl;
}

struct ParseOpts
{
	string m_FilePath;

};
ParseOpts  ParseArgs ( int argc, char **argv )
{
	ParseOpts opts;
	if( argc < 2 )
	{
		Usage( cout );
		throw hrException( "improper usage" );
	}
	if( argc >= 3  && !strcmp(argv[1], "-i") )
	{
		opts.m_FilePath = argv[2];
	} else if ( argc < 3 && !strcmp(argv[1], "-h") )
	{
		Usage( cout );
		throw hrSuccess("help");
	}
	return opts;
}

void DisplayVertex( HairInfo *pHInfo, int i_Strand, int i_Vertex )
{
	int idx = i_Strand * (pHInfo->m_nVerticesPerStrand + 2) + ( (i_Vertex==0) ? 0 : (i_Vertex + 1) );
	DBG_ASSERT0( (pHInfo->m_Vertices.size() > idx ), "logical inconsistency" );
	maVector3d &vtx = pHInfo->m_Vertices[ idx ];
	cout << "strand Idx = " << i_Strand << endl;
	cout << "vertexIdx = " << i_Vertex << endl;
	cout << vtx[0] << " " << vtx[1] << " " << vtx[2] << endl;
}

int main( int argc, char **argv )
{
	int retVal = 0;
	try
	{
		ifstream file;
		ParseOpts opts = ParseArgs( argc, argv );
		file.open( opts.m_FilePath.c_str() , ios_base::in );
		if( !file.is_open() )
		{
			stringstream ss;
			ss << "file " << opts.m_FilePath << " not found!";
			throw hrException( ss.str() );
		}
		HairInfo * pHInfo = NULL;
		pHInfo = ParseHairFile( file );
		//verification
		//display the last vertex read
		if( pHInfo )
		{
			DisplayVertex(pHInfo, pHInfo->m_nStrands-1, pHInfo->m_nVerticesPerStrand-1);
		}
		file.close();


	} 
	catch( hrException &ex )
	{
		ex = ex;
		retVal = -1;
	}
	catch( hrSuccess &ex )
	{
		ex = ex;
	}
	return retVal;
}