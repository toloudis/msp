
/*****************************************************************************
**  MaxModelClusterer.hpp
** 
**  Does the clustering of objects in a max scene
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#if !defined( MAX_MODELCLUSTERER_HPP )
#define  MAX_MODELCLUSTERER_HPP 


#include "max.h"
#include "iparamb2.h"
#include "resource2.h"
#include "utilapi.h"
#include "iFnPub.h"

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
using boost::shared_ptr;
using boost::weak_ptr;




extern HINSTANCE hInstance;
#define UNUSED(a)
MCHAR* GetString(int id);
#define SGPU_MODELCLUSTERER_UTIL_CLASS_ID	Class_ID(0x3a5d7d49, 0x588c6ae0)



//========================================================================
//	SgpuModelClusterer is a utility plugin. 
//	
//========================================================================
class SgpuModelClusterer : public UtilityObj {
public:
	IUtil *m_pIUtil;
	Interface *m_pInterface;		
	HWND m_hWnd;

	SgpuModelClusterer():
	m_pInterface( NULL ),
		m_pIUtil( NULL ),
		m_hWnd( NULL )
	{}
	//inherited from the UtilityObj
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);
	void DeleteThis() {}
	//Make clusters of all the objects in the scene
	//1)All objects should be editable meshes
	//2)All objects should have the same material and same number of texture mapping(multi texturing)
	//3)All objects should be children of the root node
	bool Do();	
	// Main plugin dialog callback as a member function
	INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);	
	// Main plugin dialog callback as a static member function
	static INT_PTR CALLBACK DlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//Get the tri-object crresponding to this node
	//THis is cached
	TriObject *GetTriObject ( INode *i_pNode );
	
	static SgpuModelClusterer theSgpuModelClusterer;
protected:
	//Cache for the triobject
	typedef std::map< INode *, TriObject * > TNodeCache;
	TNodeCache m_NodeCache;
};


class SgpuModelClustererClassDesc: public ClassDesc2 {
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return &SgpuModelClusterer::theSgpuModelClusterer; }
	const MCHAR *	ClassName() { return GetString( IDS_SGPU_MODELCLUSTERER ); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return SGPU_MODELCLUSTERER_UTIL_CLASS_ID; }
	const MCHAR* 	Category() { return GetString( IDS_SGPU_MODELCLUSTERER );  }
	static SgpuModelClustererClassDesc theSgpuModelClustererClassDesc;
	const MCHAR*	InternalName() { return GetString( IDS_SGPU_MODELCLUSTERER  ); }	
	HINSTANCE		HInstance() { return hInstance; }	

	//Added by sgpu
	int				ExtCount();	// Number of extensions supported
	const MCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
};


#endif  //MODELCLUSTERER