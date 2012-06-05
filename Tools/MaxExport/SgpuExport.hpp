/*****************************************************************************
**  SgpuExporter.hpp
**
**	Highest level export class, derived from SceneExport of 3dsMax SDK
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_SGPUEXPORT_HPP
#error MAXEXP_SGPUEXPORT_HPP multuply defined!!
#endif

#include "max.h"
#include "iparamb2.h"
#include "resource.h"
#include "iparamm2.h"
#include "iFnPub.h"
#include "guplib.h"


extern HINSTANCE hInstance;
#define UNUSED(a)
TCHAR* GetString(int id);

#define SGPU_EXPORT_FP_MIXIN_INTERFACE_ID Interface_ID(0x21fab135, 0x7d266000)
/*

#define GetSgpuExportInterface(obj) \
   ( (ISgpuExportFP*) obj->GetInterface( SGPU_MODEL_EXPORT_FP_INTERFACE_ID ) ) 

class ISgpuExportFP : public FPMixinInterface {
	public:
		enum OpID {
			kFoo,
			kDoExport			
			};
		
		BEGIN_FUNCTION_MAP	
			VFN_1(kFoo, foo, TYPE_DWORD );
			//FN_3(kDoExport,  TYPE_DWORD, DoExport, TYPE_FILENAME,  TYPE_BOOL, TYPE_DWORD)					
		END_FUNCTION_MAP
		virtual void foo( DWORD iArg )=0;
		//virtual DWORD DoExport(const TCHAR *fname,  BOOL suppressPrompts, DWORD options )=0;	
		
		//functions  inherited from FPMixinInterface
		FPInterfaceDesc* GetDescByID(Interface_ID id);
		//function inherited from FPInterface
		FPInterfaceDesc* GetDesc(); 
	};
	*/
class SgpuExporter :/*public IObject,*/public SceneExport /*, public ISgpuExportFP*/
{
	friend INT_PTR CALLBACK ExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	
	SgpuExporter();
	~SgpuExporter();

#if defined(SGPU_EXPORT_IOBJECT)
	// IObject methods
	MCHAR* GetIObjectName() { return _M("SgpuExport"); }
	//TCHAR* GetName() { return _T("SgpuExport"); }
	int NumInterfaces() { return 1; }
	
	BaseInterface* GetInterfaceAt(int i)
	{
		if (i == 0) return (ISgpuExportFP*)this;
		else return NULL;
	}

	BaseInterface* GetInterface(Interface_ID id)
	{
		if (id == SGPU_EXPORT_FP_MIXIN_INTERFACE_ID ) return (ISgpuExportFP*)this;
		else return IObject::GetInterface( id );
	}
	void AcquireIObject() 
	{ 
		int k=0;
	}
	void ReleaseIObject()
	{ 
		int k=0;
	}
	void DeleteIObject()
	{ 
		int k=0;
	}
#endif
/*
	//functions  inherited from ISgpuExportFP
	//DWORD DoExport(const TCHAR *fname,  BOOL suppressPrompts, DWORD options );	
	void foo( DWORD iArg)
	{
		int i=0;
	}
*/
	//functions inherited from SceneExporter
	int				ExtCount();					// Number of extensions supported
	BOOL            MaxUVs;                     // TRUE if generating extra verts for mismatched UV coords
	const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1
	const TCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	int				DoExport(const MCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options);	// Export file	
	BOOL			SupportsOptions(int ext, DWORD options);


	static int m_NumInstances;

};

// Statics
#define SGPU_EXPORTER_CLASS_ID	Class_ID(0xb333b85, 0x282107c6)


class SgpuExportClassDesc:public ClassDesc2 {
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE);
	const TCHAR *	ClassName() { return GetString(IDS_SGPU_EXPORT); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return SGPU_EXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_SCENEEXPORT);  }
	static SgpuExportClassDesc theSgpuExportClassDesc;
	const TCHAR*	InternalName() { return GetString(IDS_SGPU_EXPORT); }	
	HINSTANCE		HInstance() { return hInstance; }	

	//Added by sgpu
	int				ExtCount();	// Number of extensions supported
	const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
};
