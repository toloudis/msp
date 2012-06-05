/*****************************************************************************
**  MaxInstanceMaker.hpp
**  The main plugin file
**
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#if !defined( MAX_INSTANCEMAKER_HPP )
#define  MAX_INSTANCEMAKER_HPP 

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#include "InstanceWork.hpp"

#include "max.h"
#include "iparamb2.h"
#include "resource.h"
#include "utilapi.h"
#include "iFnPub.h"

#include <map>
#include <string>

using namespace std;


extern HINSTANCE hInstance;
#define UNUSED(a)
MCHAR* GetString(int id);
#define SGPU_INSTANCEMAKER_UTIL_CLASS_ID	Class_ID(0x60f25aab, 0x9170afe)


//========================================================================
//	A pair of INode-s 
//	
//========================================================================

class INodePair
{
public:
	INodePair():m_first(NULL), m_second(NULL){}
	INodePair( INode * pNode1, INode * pNode2 )
	{
		m_first = pNode1;
		m_second = pNode2;
	}

	INodePair( const INodePair &rhs):
	m_first( rhs.m_first ),
		m_second(rhs.m_second)
	{}

	//========================================================================
	// lexicographic comparison with another INodePair		
	//========================================================================

	bool operator<( const INodePair & rhs ) const
	{
		const INode *pf1 = m_first;
		if( m_first < rhs.m_first )
		{
			return true;
		} else if ( m_first == rhs.m_first )
		{
			return m_second < rhs.m_second;
		} else
		{
			return false;
		}
	}
	
	INode *m_first;
	INode *m_second;
};




//========================================================================
//	SgpuInstanceMaker is a utility plugin. 
//	
//========================================================================

class SgpuInstanceMaker : public UtilityObj {
public:
	IUtil *m_pIUtil;
	Interface *m_pInterface;		
	HWND m_hWnd;

	SgpuInstanceMaker():
	m_pInterface( NULL ),
		m_pIUtil( NULL ),
		m_hWnd( NULL )
	{}
	//inherited by UtilityObj
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);
	void DeleteThis() {}
	//Given two selected objects,
	// guess the transformation that transforms the second object into the first object
	bool Do();
	// Does the geomtry, topology and material assignments of each of the two objects
	// allow us to represent the first one as an instance of the second
	bool CanExpressAsInstances( INode *i_pNode1, INode *i_pNode2 );
	// get the transformation that transforms the second object
	//to the first object
	Matrix3 GetTransform( INode *i_pNode1, INode *i_pNode2 );	
	//get the maximum error of any component difference from the first
	//vertex to the transformed second vertex
	float GetError( INode *i_pNode1, INode *i_pNode2 );
	INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//single instance of the plugin object
	static SgpuInstanceMaker theSgpuInstanceMaker;
	
protected:
	// is the submaterial assignmment for the triangle objects i_pTriObj1, and i_pTriObj2 are the same.
	bool IsConsistentMtl( TriObject *i_pTriObj1, TriObject *i_pTriObj2, Mtl *i_pMtl1, Mtl * i_pMtl2  );
	// does the triange objects i_pTriObj1 and i_pTriObj2 has the same geometry and topology
	bool IsConsistentGeomAndTop( TriObject *i_pTriObj1, TriObject *i_pTriObj2 );
	// guess the matrix that transforms the vertices of i_pNode2 to that of i_pNode1
	bool InstanceResolve( INode *i_pNode1, INode *i_pNode2, Matrix3 *&pret, float &error );
	InstanceWork m_InstanceWork;
	//Make a cache of the computed work,
	//so that if asked again, we can retreive the info,
	//wthout bothering to compute again.
	typedef std::pair< shared_ptr< Matrix3 >, float> TData;
	typedef std::map< INodePair, TData > TCache;
	TCache m_Cache;
};


//========================================================================
//	class description 
//	
//========================================================================
class SgpuInstanceMakerClassDesc: public ClassDesc2 {
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return &SgpuInstanceMaker::theSgpuInstanceMaker; }
	const MCHAR *	ClassName() { return GetString( IDS_SGPU_INSTANCEMAKER ); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return SGPU_INSTANCEMAKER_UTIL_CLASS_ID; }
	const MCHAR* 	Category() { return GetString( IDS_SGPU_INSTANCEMAKER );  }
	static SgpuInstanceMakerClassDesc theSgpuInstanceMakerClassDesc;
	const MCHAR*	InternalName() { return GetString( IDS_SGPU_INSTANCEMAKER  ); }	
	HINSTANCE		HInstance() { return hInstance; }	

	//Added by sgpu
	int				ExtCount();	// Number of extensions supported
	const MCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
};


#endif