/*****************************************************************************
**  MaxExportOptions.hpp
**
**	class which holds the GUI and data-binding members of
**  the max export options
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_MAXEXPORTOPTIONS_HPP
#error MAXEXP_MAXEXPORTOPTIONS_HPP multuply defined!!
#endif
#define MAXEXP_MAXEXPORTOPTIONS_HPP

#include "MaxCommon.hpp"

#include "max.h"
#include "resource.h"

class fxXMLWriter;
namespace MaxExp
{
	class ExportLogger;
}
using namespace MaxExp;

TCHAR* GetString(int id);

//Interface ID for SgpuExportOptions
#define SGPU_EXPORT_OPTIONS_FP_INTERFACE_ID Interface_ID(0x7a51957, 0x61937d95)

#define GetSgpuExportOptionsInterface(obj) \
   ( (ISgpuExportOptionsFP*) obj->GetInterface( SGPU_EXPORT_OPTIONS_FP_INTERFACE_ID ) 


//Interface
class ISgpuExportOptionsFP: public FPMixinInterface 
{
public:
	enum OpID {
		kFoo,
		
		k_LogToString,
		k_InitFromString,

		kGet_ExporterVersion,	
		kGet_ExportLibVersion,

		kGet_CurMaxFilepath,
		kSet_CurMaxFilepath,
		
		kSet_AnimRange,
		
		kGet_bExportPolyAsTriangle,
		kSet_bExportPolyAsTriangle,

		kGet_bExportSelected,
		kSet_bExportSelected,

		kGet_bExportParentIfChildExports,
		kSet_bExportParentIfChildExports,

		kGet_bExportChildIfParentExports,
		kSet_bExportChildIfParentExports,
		
		kGet_bSupportXref,
		kSet_bSupportXref,
		
		kGet_bExportPivotAsSeparateNode,
		kSet_bExportPivotAsSeparateNode,

		kGet_bMergeBasedOnMtls,
		kSet_bMergeBasedOnMtls,
		
		kGet_bExportGeom,
		kSet_bExportGeom,

		
		kGet_bCompressVertexAnim,
		kSet_bCompressVertexAnim,
		
		kGet_nMaxNumTrianglesInMergedMesh,
		kSet_nMaxNumTrianglesInMergedMesh,
		
		kGet_eExportIntent,
		kSet_eExportIntent,
		
		kGet_fStartFrame,
		kSet_fStartFrame,

		
		kGet_fEndFrame,
		kSet_fEndFrame,
		
		kGet_fStepFrame,
		kSet_fStepFrame,

		kGet_fMaxEndFrame,
		kSet_fMaxEndFrame,

		
		kGet_fStartTime,
		kSet_fStartTime,

		
		kGet_fEndTime,
		kSet_fEndTime,
		
		kGet_fStepTime,
		kSet_fStepTime,

		kGet_fToleranceVertexAnim,
		kSet_fToleranceVertexAnim,

		
		kGet_fRenderImageAspect,
		kSet_fRenderImageAspect,
		
		kGet_fRenderApertureWidth,
		kSet_fRenderApertureWidth
		
	};

	BEGIN_FUNCTION_MAP

		VFN_1(kFoo, foo, TYPE_DWORD );

		FN_0( k_LogToString, TYPE_TSTR_BV, LogToString );

		VFN_1(k_InitFromString, InitFromString, TYPE_TSTR_BR );

		FN_0( kGet_ExporterVersion, TYPE_TSTR_BV, Get_ExporterVersion );

		FN_0( kGet_ExportLibVersion, TYPE_TSTR_BV, Get_ExportLibVersion );

		FN_0( kGet_CurMaxFilepath, TYPE_TSTR_BV, Get_CurMaxFilepath );

		VFN_1(kSet_CurMaxFilepath, Set_CurMaxFilepath, TYPE_TSTR_BR );

		VFN_3(kSet_AnimRange, Set_AnimRange, TYPE_INT, TYPE_FLOAT, TYPE_FLOAT );
		
		PROP_FNS( kGet_bExportPolyAsTriangle, Get_bExportPolyAsTriangle,
					kSet_bExportPolyAsTriangle, Set_bExportPolyAsTriangle, TYPE_BOOL);
		
		PROP_FNS( kGet_bExportSelected, Get_bExportSelected,
					kSet_bExportSelected, Set_bExportSelected, TYPE_BOOL);
		
		PROP_FNS( kGet_bExportParentIfChildExports, Get_bExportParentIfChildExports,
					kSet_bExportParentIfChildExports, Set_bExportParentIfChildExports, TYPE_BOOL);
		
		PROP_FNS( kGet_bExportChildIfParentExports, Get_bExportChildIfParentExports,
					kSet_bExportChildIfParentExports, Set_bExportChildIfParentExports, TYPE_BOOL);
		
		PROP_FNS( kGet_bSupportXref, Get_bSupportXref,
					kSet_bSupportXref, Set_bSupportXref, TYPE_BOOL);
		
		PROP_FNS( kGet_bExportPivotAsSeparateNode, Get_bExportPivotAsSeparateNode,
					kSet_bExportPivotAsSeparateNode, Set_bExportPivotAsSeparateNode, TYPE_BOOL);

		PROP_FNS( kGet_bMergeBasedOnMtls, Get_bMergeBasedOnMtls,
					kSet_bMergeBasedOnMtls, Set_bMergeBasedOnMtls, TYPE_BOOL);
		
		PROP_FNS( kGet_bExportGeom, Get_bExportGeom,
					kSet_bExportGeom, Set_bExportGeom, TYPE_BOOL);
		
		PROP_FNS( kGet_bCompressVertexAnim, Get_bCompressVertexAnim,
					kSet_bCompressVertexAnim, Set_bCompressVertexAnim, TYPE_BOOL);

		PROP_FNS( kGet_nMaxNumTrianglesInMergedMesh, Get_nMaxNumTrianglesInMergedMesh,
					kSet_nMaxNumTrianglesInMergedMesh, Set_nMaxNumTrianglesInMergedMesh, TYPE_INT );
		
		PROP_FNS( kGet_eExportIntent, Get_eExportIntent,
					kSet_eExportIntent, Set_eExportIntent, TYPE_INT );
		
		PROP_FNS( kGet_fStartFrame, Get_fStartFrame,
					kSet_fStartFrame, Set_fStartFrame, TYPE_FLOAT );
		
		PROP_FNS( kGet_fEndFrame, Get_fEndFrame,
					kSet_fEndFrame, Set_fEndFrame, TYPE_FLOAT );		
		
		PROP_FNS( kGet_fStepFrame, Get_fStepFrame,
					kSet_fStepFrame, Set_fStepFrame, TYPE_FLOAT );
		
		PROP_FNS( kGet_fMaxEndFrame, Get_fMaxEndFrame,
					kSet_fMaxEndFrame, Set_fMaxEndFrame, TYPE_FLOAT );
		
		PROP_FNS( kGet_fStartTime, Get_fStartTime,
					kSet_fStartTime, Set_fStartTime, TYPE_FLOAT );
		
		PROP_FNS( kGet_fEndTime, Get_fEndTime,
					kSet_fEndTime, Set_fEndTime, TYPE_FLOAT );		
		
		PROP_FNS( kGet_fStepTime, Get_fStepTime,
					kSet_fStepTime, Set_fStepTime, TYPE_FLOAT );
		
		PROP_FNS( kGet_fToleranceVertexAnim, Get_fToleranceVertexAnim,
					kSet_fToleranceVertexAnim, Set_fToleranceVertexAnim, TYPE_FLOAT );
		
		PROP_FNS( kGet_fRenderImageAspect, Get_fRenderImageAspect,
					kSet_fRenderImageAspect, Set_fRenderImageAspect, TYPE_FLOAT );		
		
		PROP_FNS( kGet_fRenderApertureWidth, Get_fRenderApertureWidth,
					kSet_fRenderApertureWidth, Set_fRenderApertureWidth, TYPE_FLOAT );
		
	END_FUNCTION_MAP


	virtual void foo( DWORD iArg )=0;


	virtual MSTR LogToString()=0;
	virtual void InitFromString( MSTR& i_filePath )=0;
	virtual MSTR Get_ExporterVersion()=0;
	virtual MSTR Get_ExportLibVersion()=0;

	virtual MSTR Get_CurMaxFilepath()=0;
	virtual void Set_CurMaxFilepath( MSTR& i_filePath )=0;

	virtual void Set_AnimRange( int i_nTicksPerFrame, float i_fStartFrame, float i_fEndFrame )=0;

	virtual BOOL Get_bExportPolyAsTriangle()=0;
	virtual void Set_bExportPolyAsTriangle( BOOL i_bool )=0;

	virtual BOOL Get_bExportSelected()=0;
	virtual void Set_bExportSelected( BOOL i_bool )=0;

	virtual BOOL Get_bExportParentIfChildExports()=0;
	virtual void Set_bExportParentIfChildExports( BOOL i_bool )=0;
	
	virtual BOOL Get_bExportChildIfParentExports()=0;
	virtual void Set_bExportChildIfParentExports( BOOL i_bool )=0;
	
	virtual BOOL Get_bSupportXref()=0;
	virtual void Set_bSupportXref( BOOL i_bool )=0;

	
	virtual BOOL Get_bExportPivotAsSeparateNode()=0;
	virtual void Set_bExportPivotAsSeparateNode( BOOL i_bool )=0;

	
	virtual BOOL Get_bMergeBasedOnMtls()=0;
	virtual void Set_bMergeBasedOnMtls( BOOL i_bool )=0;

	
	virtual BOOL Get_bExportGeom()=0;
	virtual void Set_bExportGeom( BOOL i_bool )=0;

	
	virtual BOOL Get_bCompressVertexAnim()=0;
	virtual void Set_bCompressVertexAnim( BOOL i_bool )=0;
	
	virtual int Get_nMaxNumTrianglesInMergedMesh()=0;
	virtual void Set_nMaxNumTrianglesInMergedMesh( int i_bool )=0;

	
	virtual int Get_eExportIntent()=0;
	virtual void Set_eExportIntent( int i_bool )=0;

	
	virtual float Get_fStartFrame()=0;
	virtual void Set_fStartFrame( float i_float )=0;

	
	virtual float Get_fEndFrame()=0;
	virtual void Set_fEndFrame( float i_float )=0;
	
	
	virtual float Get_fStepFrame()=0;
	virtual void Set_fStepFrame( float i_float )=0;
	
	virtual float Get_fMaxEndFrame()=0;
	virtual void Set_fMaxEndFrame( float i_float )=0;

	virtual float Get_fStartTime()=0;
	virtual void Set_fStartTime( float i_float )=0;

	virtual float Get_fEndTime()=0;
	virtual void Set_fEndTime( float i_float )=0;	
	
	virtual float Get_fStepTime()=0;
	virtual void Set_fStepTime( float i_float )=0;
	
	virtual float Get_fToleranceVertexAnim()=0;
	virtual void Set_fToleranceVertexAnim( float i_float )=0;
	
	virtual float Get_fRenderImageAspect()=0;
	virtual void Set_fRenderImageAspect( float i_float )=0;	
	
	virtual float Get_fRenderApertureWidth()=0;
	virtual void Set_fRenderApertureWidth( float i_float )=0;

	//functions  inherited from FPMixinInterface
	FPInterfaceDesc* GetDescByID(Interface_ID id);
	//function inherited from FPInterface
	FPInterfaceDesc* GetDesc(); 
};


class SgpuExportOptions :  public ISgpuExportOptionsFP
{
public:

	bool m_bExportPolyAsTriangle;
	bool m_bExportSelected;
	bool m_bExportParentIfChildExports;	
	bool m_bExportChildIfParentExports;
	bool m_bSupportXref;
	bool m_bExportPivotAsSeparateNode;
	bool m_bMergeBasedOnMtls;
	int m_nMaxNumTrianglesInMergedMesh;
	Intent m_eExportIntent;
	float m_fStartFrame;
	float m_fEndFrame;
	float m_fMaxEndFrame;
	float m_fStepFrame;
	TimeValue m_StartTime;
	TimeValue m_EndTime;
	TimeValue m_StepTime;
	bool m_bExportGeom; 
	bool	m_bCompressVertexAnim;
	float	m_fToleranceVertexAnim;
	float	m_fRenderImageAspect;
	float	m_fRenderApertureWidth;
	std::wstring m_wsCurMaxFilepath;
	
	void Init( bool bExportSelected );
	void InitAnimRangeParams( int i_TicksPerFrame, TimeValue i_StartTime, TimeValue i_EndTime );		
	void InitRenderCameraParams( float i_fRenderImageAspect, float i_fRenderApertureWidth );
	void Log( ExportLogger &logger ) const;

	void Init( const std::string &inFile );
	void Log( std::string &outFile ) const;

	SgpuExportOptions()
	{
		Init( false );
	}

	void foo( DWORD i_Arg ) { }

	MSTR LogToString()
	{
		std::string slog;
		Log( slog );
		return slog.c_str();
	}
	void InitFromString( MSTR& i_xmlString )
	{
		std::string sXmlString( i_xmlString.data() );
		Init( sXmlString );
	}

	MSTR Get_ExporterVersion();
	MSTR Get_ExportLibVersion();

	MSTR Get_CurMaxFilepath();
	void Set_CurMaxFilepath( MSTR& i_filepath );

	void Set_AnimRange( int i_nTicksPerFrame, float i_fStartFrame, float i_fEndFrame );

	BOOL Get_bExportPolyAsTriangle(){ return static_cast< BOOL > ( m_bExportPolyAsTriangle );}
	void Set_bExportPolyAsTriangle( BOOL i_bool ) { m_bExportPolyAsTriangle = static_cast< bool > (i_bool ); }

	BOOL Get_bExportSelected(){ return static_cast< BOOL > ( m_bExportSelected );}
	void Set_bExportSelected( BOOL i_bool ) { m_bExportSelected = static_cast< bool > (i_bool ); }

	
	BOOL Get_bExportParentIfChildExports(){ return static_cast< BOOL > ( m_bExportParentIfChildExports );}
	void Set_bExportParentIfChildExports( BOOL i_bool ) { m_bExportParentIfChildExports = static_cast< bool > (i_bool ); }

	
	BOOL Get_bExportChildIfParentExports(){ return static_cast< BOOL > ( m_bExportChildIfParentExports );}
	void Set_bExportChildIfParentExports( BOOL i_bool ) { m_bExportChildIfParentExports = static_cast< bool > (i_bool ); }
	
	BOOL Get_bSupportXref(){ return static_cast< BOOL > ( m_bSupportXref );}
	void Set_bSupportXref( BOOL i_bool ) { m_bSupportXref = static_cast< bool > (i_bool ); }
	
	BOOL Get_bExportPivotAsSeparateNode(){ return static_cast< BOOL > ( m_bExportPivotAsSeparateNode );}
	void Set_bExportPivotAsSeparateNode( BOOL i_bool ) { m_bExportPivotAsSeparateNode = static_cast< bool > (i_bool ); }

	
	BOOL Get_bMergeBasedOnMtls(){ return static_cast< BOOL > ( m_bMergeBasedOnMtls );}
	void Set_bMergeBasedOnMtls( BOOL i_bool ) { m_bMergeBasedOnMtls = static_cast< bool > (i_bool ); }

	BOOL Get_bExportGeom(){ return static_cast< BOOL > ( m_bExportGeom );}
	void Set_bExportGeom( BOOL i_bool ) { m_bExportGeom = static_cast< bool > (i_bool ); }

	BOOL Get_bCompressVertexAnim(){ return static_cast< BOOL > ( m_bCompressVertexAnim );}
	void Set_bCompressVertexAnim( BOOL i_bool ) { m_bCompressVertexAnim = static_cast< bool > (i_bool ); }
	
	int Get_nMaxNumTrianglesInMergedMesh(){ return static_cast< int > ( m_nMaxNumTrianglesInMergedMesh );}
	void Set_nMaxNumTrianglesInMergedMesh( int i_int ) { m_nMaxNumTrianglesInMergedMesh = static_cast< int > (i_int ); }

	int Get_eExportIntent(){ return static_cast< int > ( m_eExportIntent );}
	void Set_eExportIntent( int i_int ) { m_eExportIntent = static_cast< Intent > (i_int ); }
	
	float Get_fStartFrame(){ return static_cast< float > ( m_fStartFrame );}
	void Set_fStartFrame( float i_float ) { m_fStartFrame = static_cast< float > (i_float ); }
	
	float Get_fEndFrame(){ return static_cast< float > ( m_fEndFrame );}
	void Set_fEndFrame( float i_float ) { m_fEndFrame = static_cast< float > (i_float ); }
	
	float Get_fStepFrame(){ return static_cast< float > ( m_fStepFrame );}
	void Set_fStepFrame( float i_float ) { m_fStepFrame = static_cast< float > (i_float ); }
	
	float Get_fMaxEndFrame(){ return static_cast< float > ( m_fMaxEndFrame );}
	void Set_fMaxEndFrame( float i_float ) { m_fMaxEndFrame = static_cast< float > (i_float ); }
	
	float Get_fStartTime(){ return static_cast< float > ( m_StartTime );}
	void Set_fStartTime( float i_float ) { m_StartTime = static_cast< TimeValue > (i_float ); }
	
	float Get_fEndTime(){ return static_cast< float > ( m_EndTime );}
	void Set_fEndTime( float i_float ) { m_EndTime = static_cast< TimeValue > (i_float ); }
	
	float Get_fStepTime(){ return static_cast< float > ( m_StepTime );}
	void Set_fStepTime( float i_float ) { m_StepTime = static_cast< TimeValue > (i_float ); }	
	
	float Get_fToleranceVertexAnim(){ return static_cast< float > ( m_fToleranceVertexAnim );}
	void Set_fToleranceVertexAnim( float i_float ) { m_fToleranceVertexAnim = static_cast< float > (i_float ); }
	
	float Get_fRenderImageAspect(){ return static_cast< float > ( m_fRenderImageAspect );}
	void Set_fRenderImageAspect( float i_float ) { m_fRenderImageAspect = static_cast< float > (i_float ); }
	
	float Get_fRenderApertureWidth(){ return static_cast< float > ( m_fRenderApertureWidth );}
	void Set_fRenderApertureWidth( float i_float ) { m_fRenderApertureWidth = static_cast< float > (i_float ); }
	
};

//ClassID
#define SGPU_EXPORTER_OPTIONS_CLASS_ID Class_ID(0x67892b3f, 0x235a2d07)
class SgpuExportOptionsClassDesc:public ClassDesc2 {
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE);
	const TCHAR *	ClassName() { return GetString(IDS_SGPU_EXPORT_OPTIONS ); }
	SClass_ID		SuperClassID() { return UNKNOWN_CLASS_ID; }
	Class_ID		ClassID() { return SGPU_EXPORTER_OPTIONS_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_SGPU_EXPORT_OPTIONS);  }
	static SgpuExportOptionsClassDesc theDesc;
	const TCHAR*	InternalName() { return GetString(IDS_SGPU_EXPORT_OPTIONS); }	
	HINSTANCE		HInstance() { return hInstance; }
};



