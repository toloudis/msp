#if !defined( MAX_MODELBREAKER_HPP )
#define  MAX_MODELBREAKER_HPP 


#include "max.h"
#include "iparamb2.h"
#include "resource.h"
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
#define SGPU_MODELBREAKER_UTIL_CLASS_ID	Class_ID(0xd367ac7, 0x25e67984) 


class SgpuModelBreaker : public UtilityObj {
public:
	IUtil *m_pIUtil;
	Interface *m_pInterface;		
	HWND m_hWnd;

	SgpuModelBreaker():
	m_pInterface( NULL ),
		m_pIUtil( NULL ),
		m_hWnd( NULL )
	{}
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);
	void DeleteThis() {}
	bool Do();

	INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static SgpuModelBreaker theSgpuModelBreaker;
	
protected:

};


class SgpuModelBreakerClassDesc: public ClassDesc2 {
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return &SgpuModelBreaker::theSgpuModelBreaker; }
	const MCHAR *	ClassName() { return GetString( IDS_SGPU_MODELBREAKER ); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return SGPU_MODELBREAKER_UTIL_CLASS_ID; }
	const MCHAR* 	Category() { return GetString( IDS_SGPU_MODELBREAKER );  }
	static SgpuModelBreakerClassDesc theSgpuModelBreakerClassDesc;
	const MCHAR*	InternalName() { return GetString( IDS_SGPU_MODELBREAKER  ); }	
	HINSTANCE		HInstance() { return hInstance; }	

	//Added by sgpu
	int				ExtCount();	// Number of extensions supported
	const MCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
};


#endif