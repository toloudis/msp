/*****************************************************************************
**  SgpuExporterFP.hpp
** Function publishing interface for the sgpuExporter
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_SGPUEXPORTFP_HPP
#error MAXEXP_SGPUEXPORTFP_HPP multuply defined!!
#endif

#include "max.h"
#include "iparamb2.h"
#include "resource.h"
#include "iparamm2.h"
#include "iFnPub.h"
#include "guplib.h"

class SgpuExportOptions;

extern HINSTANCE hInstance;
#define UNUSED(a)
TCHAR* GetString(int id);
class SgpuExporter;

#define SGPU_EXPORT_FP_INTERFACE_ID Interface_ID(0x2862690e, 0x65a47cc3)


class ISgpuExportFP : public FPStaticInterface
{
public:
	ISgpuExportFP()
	{
		
		_RPT0( _CRT_WARN, "ISgpuExportFP Constructor" );
		int k=0;
	}
	virtual ~ISgpuExportFP()
	{
		_RPT0( _CRT_WARN, "ISgpuExportFP Destructor" );
		int j=0;
	}
	public:
		virtual float GetNum() = 0;
		virtual void  SetNum(float x) = 0;
		virtual float products(float x, float y) = 0;
		virtual void  message() = 0;
		
		virtual SgpuExportOptions *Get_Options()=0;
		
		virtual int Do( FPInterface *i_pOptions, TSTR &i_ExportFilepath ) =0;

		virtual int Do_2( FPInterface *i_pOptions, TSTR &i_ExportFilepath, Tab<INode*>& i_Objects ) =0;

		virtual MSTR Get_ExporterVersion() =0;

		virtual MSTR Get_ExportLibVersion() =0;

		virtual bool Get_ObjectFlagBool( INode *i_pData , TSTR &i_flagName ) = 0;

		virtual void Set_ObjectFlagBool( INode *i_pData , TSTR &i_flagName, bool i_bVal ) = 0;

		enum { 
			em_getNum, 
			em_setNum, 
			em_products, 
			em_message, 
			em_getOptions, 
			em_Do , 
			em_Do_2, 
			em_GetExporterVersion, 
			em_GetExportLibVersion, 
			em_GetObjectFlagBool,
			em_SetObjectFlagBool
		};
};

class SgpuExportFP : public ISgpuExportFP
{
public:
	private:
		float m_Num;
	public:
		float GetNum();
		void  SetNum(float x);

		float products(float x, float y);
		void  message();

		//SgpuExporter *Get();

		virtual SgpuExportOptions *Get_Options();
		
		int Do( FPInterface *i_pOptions, TSTR &i_ExportFilepath );		

		int Do_2( FPInterface *i_pOptions, TSTR &i_ExportFilepath, Tab<INode*>& i_Objects );

		MSTR Get_ExporterVersion();
		
		MSTR Get_ExportLibVersion();

		bool Get_ObjectFlagBool( INode *i_pData , TSTR &i_flagName );

		void Set_ObjectFlagBool( INode *i_pData, TSTR &i_flagName, bool i_bVal );


	DECLARE_DESCRIPTOR( SgpuExportFP );
	BEGIN_FUNCTION_MAP;
		//PROP_FNS is an abbreviation for PROPerty FunctioNS. It requires getter and setter
		//functions, a getter and setter enumeration, and a type (Not in that order). 
		//They must be entered in the order shown below.
		PROP_FNS(SgpuExportFP::em_getNum, GetNum, SgpuExportFP::em_setNum, SetNum, TYPE_FLOAT);
		//FN_2 is an abbreviation for a function that takes two arguements.
		//Here the function is registered as returning a float value, the function name
		//is passed in as well as the types of the two arguments.
		FN_2(SgpuExportFP::em_products, TYPE_FLOAT, products, TYPE_FLOAT, TYPE_FLOAT);
		//VFN_0 is an abbreviation for a Void Function with 0 arguments. Hence a function
		//that closes with a (), i.e. foo.BarMethod()
		VFN_0(SgpuExportFP::em_message, message);
		//FN_2 is an abbreviation for a function that takes zero arguements
		//Here the function is registered as returning a IObject value
		FN_0(SgpuExportFP::em_getOptions, TYPE_INTERFACE, Get_Options);
		//VFN_0 is an abbreviation for a Void Function with 0 arguments. Hence a function
		//that closes with a (), i.e. foo.BarMethod()
		FN_2(SgpuExportFP::em_Do, TYPE_INT, Do, TYPE_INTERFACE, TYPE_TSTR_BR);

		FN_3( SgpuExportFP::em_Do_2, TYPE_INT, Do_2, TYPE_INTERFACE, TYPE_TSTR_BR , TYPE_INODE_TAB_BR );
		
		FN_0( SgpuExportFP::em_GetExporterVersion, TYPE_TSTR_BV, Get_ExporterVersion );
		
		FN_0( SgpuExportFP::em_GetExportLibVersion, TYPE_TSTR_BV, Get_ExportLibVersion );
		
		FN_2(SgpuExportFP::em_GetObjectFlagBool, TYPE_BOOL, Get_ObjectFlagBool, TYPE_INODE, TYPE_TSTR_BR );

		VFN_3(SgpuExportFP::em_SetObjectFlagBool, Set_ObjectFlagBool, TYPE_INODE, TYPE_TSTR_BR , TYPE_BOOL );

	END_FUNCTION_MAP;
	static int m_NumInstances;
};

#define SGPU_EXPORT_FP_GUP_CLASS_ID Class_ID(0x12061582, 0x1ee924d0)

class SgpuExportFP_GUP_ClassDesc;

class SgpuExportFP_GUP : public GUP
{

public:

	SgpuExportFP_GUP();
	~SgpuExportFP_GUP();


	// GUP Methods
	DWORD	Start( );
	void	Stop( );

	// Loading/Saving
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);
	static SgpuExportFP_GUP_ClassDesc theDesc;
	static int m_NumInstances;
};

class SgpuExportFP_GUP_ClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new SgpuExportFP_GUP(); }
	const TCHAR *	ClassName() { return GetString(IDS_SGPU_EXPORT_FP); }
	SClass_ID		SuperClassID() { return GUP_CLASS_ID; }
	Class_ID		ClassID() { return SGPU_EXPORT_FP_GUP_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_SGPU_EXPORT_FP_CATEGORY); }

	const TCHAR*	InternalName() { return _T("SgpuExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	

};