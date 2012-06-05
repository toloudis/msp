/*****************************************************************************
**  MaxExportUtils.cpp
**
**	Collection of all utility functions and classes, 
**	that glues Sgpu api and max api
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MaxExportUtils.hpp"

#include "MaxCommon.hpp"
#include "ExportDoc.hpp"


#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Graphics/mat/matShaderParser.hpp"
//max include files

#include "MaxCommon.hpp"
#include "stdmat.h"
#include "buildver.h"
#include "shaders.h"
#include "XRef/iXrefObj.h"
#include "XRef/iXrefMaterial.h"
#include "CS/Bipexp.h"

#include <cstdio>
#include <algorithm>




using namespace std;

namespace MaxExp
{ 

	//========================================================================
	// Simple Profiler class 
	// that writes the time elapsed to the logger upon destruction
	//========================================================================
	SimpleProfile::SimpleProfile( const std::string &name ):m_Name(name), m_Start(0), m_Stop(0)
	{
		m_Start = clock();	
	}
	SimpleProfile::~SimpleProfile()
	{
		m_Stop = clock();
		LONG time_elapsed = m_Stop - m_Start;
		EXPLOG.WriteInfo("action '%s'  took '%d' millisecs", m_Name.c_str(), (m_Stop - m_Start));
	}

	//========================================================================
	// ExportLogger: This performs logging
	//	see .hpp
	//========================================================================

	//The singleton logger instance
	//static 
	ExportLogger ExportLogger::m_TheLogger;


	//Embedded wrap class
	ExportLogger::Wrap::Wrap( const wchar_t *i_Fname )
	{
		Init( i_Fname );
	}

	void ExportLogger::Wrap::Init( const wchar_t *i_pszExportFilename )
	{
		//an Init
		if( i_pszExportFilename )
		{
			ExportLogger::Get().Init( i_pszExportFilename );
		}
	}
	ExportLogger::Wrap::~Wrap()
	{
		//get the singleton instance and CleanUp
		ExportLogger::Get().CleanUp();
	}

	//Embedded wrap class for startelement
	ExportLogger::WriteStartElementWrap::WriteStartElementWrap(  const char *i_pzFmt, ... )
	{
		va_list vList;
		va_start( vList, i_pzFmt );
		vsnprintf(m_MsgBuffer, m_nMsgBufferLen, i_pzFmt, vList );
		ExportLogger::Get().WriteStartElement( string( m_MsgBuffer ) );
		va_end( vList );
	}

	ExportLogger::WriteStartElementWrap::~WriteStartElementWrap()
	{
		EXPLOG.WriteEndElement();
	}

	void ExportLogger::Init( const wchar_t *exportFilename )
	{	
		m_ExportFileName = wstring( exportFilename );
		assert( ! ( m_ExportFileName.empty() ? true : false ) );
		//log file name is the same as the export file name with a '.txt' extension
		m_LogFileName = ChangeExtensionCustom( m_ExportFileName, wstring(L".txt") );
		m_pFsLocator =  new fsLocator;
		itString itLogFileName( reinterpret_cast< const itString::CharType * > (  m_LogFileName.c_str() ) );
		fsFileUtil::UnicodeStringToLocator( itLogFileName, *m_pFsLocator );
		m_pFsXMLWriter = new fsXMLWriter( *m_pFsLocator );
		m_pFsXMLWriter->Open();

		m_pFsXMLWriter->WriteStartElement( string("MaxExport: ") +  envString::WideCharToUTF8( m_ExportFileName ).c_str() );
	}

	void ExportLogger::CleanUp()
	{
		if (NULL !=  m_pFsXMLWriter)
		{
			m_pFsXMLWriter->WriteEndElement();
			m_pFsXMLWriter->Close();
			delete m_pFsXMLWriter;
			m_pFsXMLWriter = NULL;
		}
		if( NULL != m_pFsLocator )
		{
			delete m_pFsLocator;
			m_pFsLocator = NULL;
		}
	}


	ExportLogger::~ExportLogger()
	{
		CleanUp();
	}


	void ExportLogger::WriteInfo( const char *fmt, ...)
	{
		if( m_pFsXMLWriter )
		{
			va_list vList;
			va_start( vList, fmt );
			vsnprintf(m_MsgBuffer, m_nMsgBufferLen, fmt, vList );
			m_pFsXMLWriter->WriteElement( m_Info, string( m_MsgBuffer ) );
			va_end( vList );
		}
	}

	void ExportLogger::WriteWarning( const char *fmt, ...)
	{
		if( m_pFsXMLWriter )
		{
			va_list vList;
			va_start( vList, fmt );
			vsnprintf(m_MsgBuffer, m_nMsgBufferLen, fmt, vList );
			m_pFsXMLWriter->WriteElement( m_Warning, string( m_MsgBuffer ) );
			va_end( vList );
		}
	}

	void ExportLogger::WriteError( const char *fmt, ...)
	{
		if( m_pFsXMLWriter )
		{
			va_list vList;
			va_start( vList, fmt );
			vsnprintf(m_MsgBuffer, m_nMsgBufferLen, fmt, vList );
			m_pFsXMLWriter->WriteElement( m_Error, string( m_MsgBuffer ) );
			va_end( vList );
		}
	}

	void ExportLogger::WriteHypothesisTest( const char *fmt, ...)
	{
		if( m_pFsXMLWriter )
		{
			va_list vList;
			va_start( vList, fmt );
			vsnprintf(m_MsgBuffer, m_nMsgBufferLen, fmt, vList );
			m_pFsXMLWriter->WriteElement( m_HypothesisTest, string( m_MsgBuffer ) );
			va_end( vList );
		}
	}

	void ExportLogger::WriteElement( string &key, const string & val)
	{
		if( m_pFsXMLWriter )
		{
			m_pFsXMLWriter->WriteElement( key, val );
		}
	}

	void ExportLogger::WriteStartElement( std::string &selement)
	{
		if( m_pFsXMLWriter )
		{
			m_pFsXMLWriter->WriteStartElement( selement );
		}
	}
	void ExportLogger::WriteEndElement()
	{
		if( m_pFsXMLWriter )
		{
			m_pFsXMLWriter->WriteEndElement( );
		}
	}

	//========================================================================
	// Get the string description of the class from the Class_ID
	// See the .hpp
	//========================================================================

#define SGPU_CLASSDESCS_INSERT( a ) \
	insert( value_type( Class_ID( a, 0x00), #a) )

#define SGPU_CLASSDESCS_INSERT2( a, b ) \
	insert( value_type( Class_ID( a, 0x00), b) )

#define SGPU_CLASSDESCS_INSERT3( a ) \
	insert( value_type(a, #a) )

#define SGPU_CLASSDESCS_INSERT4( a, b ) \
	insert( value_type( a, b) )

	ClassDescs::ClassDescs()
	{
		SGPU_CLASSDESCS_INSERT2( DMTL_CLASS_ID, "Standard Material Class_ID" );
		SGPU_CLASSDESCS_INSERT2( CMTL_CLASS_ID, "Top/Bottom Material Class_ID"  );
		SGPU_CLASSDESCS_INSERT2( MULTI_CLASS_ID, "Multi Material Class_ID" );
		SGPU_CLASSDESCS_INSERT( DOUBLESIDED_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( MIXMAT_CLASS_ID );
		//subclasses of TEXMAP_CLASS_ID
		SGPU_CLASSDESCS_INSERT2( CHECKER_CLASS_ID, "Checker Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( BMTEX_CLASS_ID, "Bitmap Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( MARBLE_CLASS_ID , "Marble 3D Texture Class_ID" );  
		SGPU_CLASSDESCS_INSERT2( MASK_CLASS_ID  , "Mask Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( NOISE_CLASS_ID  , "Noise Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( GRADIENT_CLASS_ID , "Gradient Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( TINT_CLASS_ID, "Tint Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( ACUBIC_CLASS_ID  , "Reflect/Refract Class_ID" );  
		SGPU_CLASSDESCS_INSERT2( MIRROR_CLASS_ID   , "Flat Mirror Class_ID" );
		SGPU_CLASSDESCS_INSERT2(  COMPOSITE_CLASS_ID   , "Composite Texture Class_ID" );  
		SGPU_CLASSDESCS_INSERT2(  RGBMULT_CLASS_ID , "RGB Multiply Class_ID" );
		SGPU_CLASSDESCS_INSERT2( FALLOFF_CLASS_ID , "Falloff Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( OUTPUT_CLASS_ID  , "Output Texture Class_ID" );  
		SGPU_CLASSDESCS_INSERT2(  PLATET_CLASS_ID  , "Plate Glass Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2(  VCOL_CLASS_ID   , "Vertex Color  Texture Class_ID" );
		SGPU_CLASSDESCS_INSERT2( MIX_CLASS_ID, "Mix Texture Class ID" );


		SGPU_CLASSDESCS_INSERT( PHONGClassID );
		SGPU_CLASSDESCS_INSERT( METALClassID );
		SGPU_CLASSDESCS_INSERT( BLINNClassID );
		SGPU_CLASSDESCS_INSERT( TRIOBJ_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( EDITTRIOBJ_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( POLYOBJ_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( PATCHOBJ_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( NURBSOBJ_CLASS_ID );
		SGPU_CLASSDESCS_INSERT3( EPOLYOBJ_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( BOXOBJ_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( SPHERE_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( CYLINDER_CLASS_ID );
		SGPU_CLASSDESCS_INSERT3( PLANE_CLASS_ID );
		SGPU_CLASSDESCS_INSERT3( PYRAMID_CLASS_ID );
		SGPU_CLASSDESCS_INSERT3( GSPHERE_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( CONE_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( TORUS_CLASS_ID );
		SGPU_CLASSDESCS_INSERT(  TUBE_CLASS_ID );
		SGPU_CLASSDESCS_INSERT(  HEDRA_CLASS_ID );
		SGPU_CLASSDESCS_INSERT2( BOOLOBJ_CLASS_ID, "Boolean Class_ID(obsolete)" );
		SGPU_CLASSDESCS_INSERT4( NEWBOOL_CLASS_ID, "Boolean Class_ID" );
		SGPU_CLASSDESCS_INSERT4( XREFOBJ_CLASS_ID, "Xref Object Class_ID" );
		SGPU_CLASSDESCS_INSERT4( XREFATMOS_CLASS_ID, "Xref atmospherics Class_ID");
		SGPU_CLASSDESCS_INSERT4( XREFMATERIAL_CLASS_ID, "Xref Material Class_ID" ); 
		SGPU_CLASSDESCS_INSERT4( XREFCTRL_CLASS_ID, "Xref Control Class_ID" );

		//controller subclasses
		SGPU_CLASSDESCS_INSERT( LININTERP_FLOAT_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( LININTERP_POSITION_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( LININTERP_ROTATION_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( LININTERP_SCALE_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( PRS_CONTROL_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( LOOKAT_CONTROL_CLASS_ID	);	

		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_FLOAT_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_POSITION_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_ROTATION_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_POINT3_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_SCALE_CLASS_ID	);
		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_COLOR_CLASS_ID	);
		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_POINT4_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( HYBRIDINTERP_FRGBA_CLASS_ID );

		SGPU_CLASSDESCS_INSERT( TCBINTERP_FLOAT_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( TCBINTERP_POSITION_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( TCBINTERP_ROTATION_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( TCBINTERP_POINT3_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( TCBINTERP_SCALE_CLASS_ID );
		SGPU_CLASSDESCS_INSERT( TCBINTERP_POINT4_CLASS_ID );

		SGPU_CLASSDESCS_INSERT( MASTERPOINTCONT_CLASS_ID );


		SGPU_CLASSDESCS_INSERT( WINDOBJECT_CLASS_ID  ); 
		//ToDo [kg: add a lot more ]
	}


	//singleton instance 
	//static
	ClassDescs ClassDescs::_theClassDesc;

	//unknown string which is returned if the map doesnt contain
	//the class name fora given Class_ID
	//static
	const string ClassDescs::m_Unknown("Unknown");




	//========================================================================
	// Sgpu specific classification of various max objects
	//========================================================================

	namespace  MaxObjectType
	{
		//========================================================================
		// type names returned when GetTypeName( TypeVal &type) is called
		//========================================================================
		const string sUnknown("Unknown");
		const string sBone("Bone");
		const string sMesh("Mesh");
		const string sCamera("Camera");
		const string sLight("Light");
		const string sMaterial("Material");
		const string sXrefObject("XrefObject"); 
		const string sXrefMtl("XrefMtl");
		const string sSpaceWrap("SpaceWrap");
		const string sTarget("Target");


		//========================================================================
		// Does this supre class id correspond to a derived object
		// (ie an object wkth modifiers)
		//========================================================================	
		bool IsSuperClassDerivedObj ( const SClass_ID &sid )
		{
			return ( sid == GEN_DERIVOB_CLASS_ID || sid == DERIVOB_CLASS_ID || sid == WSM_DERIVOB_CLASS_ID );
	
		}

		//========================================================================
		// Is the animatable an xref object
		//========================================================================	
		bool IsXrefObject( Animatable *i_pAni )
		{
#if SGPU_USE_MAX_8
			return IXRefObject8::Is_IXRefObject8( *i_pAni );
#elif SGPU_USE_MAX_7
			return b->SuperClassID() == SYSTEM_CLASS_ID && b->ClassID().PartA() == XREFOBJ_CLASS_ID;
#endif
		}

		//========================================================================
		// Is the animatable an xref object
		//========================================================================	
		bool IsXrefMaterial( Animatable *i_pAni )
		{
#if SGPU_USE_MAX_8
			return IXRefMaterial::Is_IXRefMaterial( *i_pAni );
#elif SGPU_USE_MAX_7
			//not supported
			return false;
#endif
		}


		//========================================================================
		//Get the TypeVal corresponding to the object referred to by the node
		//see .hpp
		//========================================================================
		TypeVal Get( INode *i_pCurNode , SgpuExportOptions &i_Options )
		{
			const MCHAR *szName = i_pCurNode->GetName();
			Object *obj = i_pCurNode->GetObjectRef();
			if( NULL == obj ) 
			{
				return Unknown;
			}
			return GetAux( i_pCurNode,i_Options, obj );
		}

		//========================================================================
		//The workhorse function evaluating the above
		//see .hpp
		//========================================================================
		TypeVal GetAux( INode *i_pCurNode , SgpuExportOptions &i_Options, Object *obj )
		{
			MCHAR *szName = i_pCurNode->GetName();
			Animatable *b = obj;
			SClass_ID sid = b->SuperClassID();
			while( IsSuperClassDerivedObj( sid ) )
			{
				IDerivedObject *dobj = (IDerivedObject *) b;
				b = dobj->GetObjRef();
				sid = b->SuperClassID();
			}


			if (IsXrefObject(b))
			{
				if ( i_Options.m_bSupportXref ) return XrefObject;
				else
				{
					ExportLogger::Get().WriteWarning("De-Referencing the xref object '%s' not supported", MBCSTOLPCSTR( szName ) );
					return Unknown;
				}
			}
			else if (IsXrefMaterial(b))
			{
				if ( i_Options.m_bSupportXref ) return XrefMtl;
				else
				{
					ExportLogger::Get().WriteWarning("De-Referencing the xref material '%s' not supported", MBCSTOLPCSTR( szName )) ;
					return Unknown;
				}
			}
			sid = b->SuperClassID();
			Class_ID  cid = b->ClassID();
			Control *c = NULL;
			switch( sid )
			{
			case GEOMOBJECT_CLASS_ID:
				if( cid == BONE_OBJ_CLASSID)
				{
					{
						//not part of export
						//for hypothesis testing only
						bool bBoneNodeOnOff = ( i_pCurNode->GetBoneNodeOnOff( ) ) ? true : false;
						if( !bBoneNodeOnOff )
						{
							EXPLOG.WriteHypothesisTest( "Bone node %s has GetBoneNodeOnOff = false", MBCSTOLPCSTR( szName ) ); 
						}
					}
	
						
					return Bone;
				}
				c = i_pCurNode->GetTMController();
				if( c != NULL )
				{
					Class_ID cid_c = c->ClassID();
					if( cid_c == BIPSLAVE_CONTROL_CLASS_ID ||
						cid_c == BIPBODY_CONTROL_CLASS_ID  ||
						cid_c == FOOTPRINT_CLASS_ID ||
						cid_c == BIPED_CLASS_ID
						)
					{
						{
							//not part of export
							//for hypothesis testing only
							bool bBoneNodeOnOff = ( i_pCurNode->GetBoneNodeOnOff( ) ) ? true : false;
							if( !bBoneNodeOnOff )
							{
								EXPLOG.WriteHypothesisTest( "node %s with bone controller has GetBoneNodeOnOff = false", MBCSTOLPCSTR( szName ) ); 
							}
						}

						return Bone;
					}
				}
				if( cid.PartA() == TARGET_CLASS_ID )
				{
					return Target;
				}
				{
					//not part of export
					//for hypothesis testing only
					bool bBoneNodeOnOff = ( i_pCurNode->GetBoneNodeOnOff( ) ) ? true : false;
					if( !bBoneNodeOnOff )
					{
						EXPLOG.WriteHypothesisTest( "MESH node with has GetBoneNodeOnOff = true", MBCSTOLPCSTR( szName ) ); 
					}
				}
				return Mesh;
				break;
			case HELPER_CLASS_ID: 
				if( cid.PartA() == BONE_CLASS_ID )
				{
					{
						//not part of export
						//for hypothesis testing only
						bool bBoneNodeOnOff = ( i_pCurNode->GetBoneNodeOnOff( ) ) ? true : false;
						if( !bBoneNodeOnOff )
						{
							EXPLOG.WriteHypothesisTest( "helper node with bone class id has GetBoneNodeOnOff = false", MBCSTOLPCSTR( szName ) ); 
						}
					}

					return Bone;

				} else
				{
					return Helper;
				}
				break;
			case CAMERA_CLASS_ID: 
				return Camera;
				break;
			case LIGHT_CLASS_ID:
				return Light;
				break;
			case MATERIAL_CLASS_ID: 
				return Material;
				break;
			case SHAPE_CLASS_ID:
				//Modifiers can act on a shape class to make a mesh
				if( b != obj )
					//ie there are modifiers
				{
					//evaluate the original object
					ObjectState os = ((IDerivedObject*)obj)->Eval( static_cast<TimeValue>( i_Options.m_StartTime) );
					return GetAux(i_pCurNode, i_Options, os.obj );
				}
			case WSM_OBJECT_CLASS_ID:
				return SpaceWrap;
			default:
				return Unknown;
				break;
			}
			return Unknown;
		}


		//========================================================================
		//Get  the typename from the typeval
		//========================================================================
		const string &GetTypeName( TypeVal objType )
		{
			switch( objType )
			{
			case Bone:
				return sBone;
			case Mesh:
				return sMesh;
			case Camera:
				return sCamera;
			case Light:
				return sLight;
			case Material:
				return sMaterial;
			case XrefObject:
				return sXrefObject;
			case XrefMtl:
				return sXrefMtl;
			case SpaceWrap:
				return sSpaceWrap;
			case Target:
				return sTarget;
			default: 
			case Unknown:
				return sUnknown;
			}
		}


		//========================================================================
		//Get  the type and typename from the nodePtr
		//========================================================================
	
		void Get( INode *i_pCurNode, SgpuExportOptions &i_Options,  string &o_StypeName, MaxObjectType::TypeVal &o_ObjType )
		{
			bool bSupportXref = i_Options.m_bSupportXref;
			o_ObjType = Get(i_pCurNode, i_Options );
			o_StypeName =GetTypeName( o_ObjType );	
		}


	} //namespace MaxOnbjectType



	//========================================================================
	// Get string names for unit enums rom max
	//========================================================================

	namespace  MaxUnit
	{

		//strings stainding for max units
		//static 
		const string sinch("inch");
		const string sfeet("feet");
		const string smile("mile");
		const string smillimeter("millimeter");
		const string scentimeter("centimeter");
		const string  smeter("meter");
		const string skilometer("kilometer");
		const string sunknown("unknown");

		//get the  name of the unit
		//unitType is a max enum
		const string & GetUnitName( int  unitType )
		{
			switch( unitType )
			{
			case UNITS_INCHES:
				return sinch;
				break;
			case UNITS_FEET:
				return sfeet;
				break;
			case UNITS_MILES:
				return smile;
				break;
			case UNITS_MILLIMETERS: 
				return smillimeter;
				break;
			case UNITS_CENTIMETERS:
				return scentimeter;
				break;
			case UNITS_METERS:
				return smeter;
				break;
			case UNITS_KILOMETERS:
				return skilometer;
				break;
			default:
				break;
			}
			return sunknown;
		}


	} //namespace MaxUnit


	//========================================================================
	// convert various max objects (especially elementary objects) 
	// to Sgpu equivalents
	//========================================================================

	namespace SgpuConvert
	{
		// max Matrix3 is converted to a maMatrix4x4	
		maMatrix4x4 Ma4x4 (const Matrix3 &inMat )
		{
			maMatrix4x4 retVal;
			const Point3 &maxMatrixRow_0 = inMat.GetRow( 0 );
			retVal(0, 0) = maxMatrixRow_0[0];
			retVal(0, 1) = maxMatrixRow_0[1];
			retVal(0, 2) = maxMatrixRow_0[2];

			const Point3 &maxMatrixRow_1 = inMat.GetRow( 1 );
			retVal(1, 0) = maxMatrixRow_1[0];
			retVal(1, 1) = maxMatrixRow_1[1];
			retVal(1, 2) = maxMatrixRow_1[2];


			const Point3 &maxMatrixRow_2 = inMat.GetRow( 2 );
			retVal(2, 0) = maxMatrixRow_2[0];
			retVal(2, 1) = maxMatrixRow_2[1];
			retVal(2, 2) = maxMatrixRow_2[2];

			const Point3 &maxMatrixRow_3 = inMat.GetRow( 3 );
			retVal(3, 0) = maxMatrixRow_3[0];
			retVal(3, 1) = maxMatrixRow_3[1];
			retVal(3, 2) = maxMatrixRow_3[2];


			retVal(0, 3) = 0.0f;
			retVal(1, 3) = 0.0f;
			retVal(2, 3) = 0.0f;
			retVal(3, 3) = 1.0f;
#if defined(_DEBUG)
			for(int i=0; i < 4; ++i)
				for(int j=0; j < 4; ++j)
				{
					if( !_finite( retVal(i,j)) )
					{
						EXPLOG.WriteError( "found non-finite floats in a matrix");
					}
				}
#endif
			return retVal;
		}

		Matrix3 MaxM3( const maMatrix4x4 &i_Mat)
		{
			Matrix3 retMat;
			for( int i=0; i < 4; ++i )
			{			
				Point3 sm( i_Mat(i, 0), i_Mat(i,1), i_Mat(i, 2) );
				retMat.SetRow(i, sm  );
			}
			return retMat;
		}

		//max node name is converted 
		string NodeName(const MCHAR *szName)
		{
			return MbcsToUtf8( szName) ;
		}
		//instanced  node name is converted 
		string InstancedNodeName(const MCHAR *szName)
		{
			MSTR sName(szName );
			MSTR sInstanced("_instanced");
			sName = sName.Append(sInstanced );
			return MbcsToUtf8( sName.data() ) ;
		}
		// max material name is converted
		string MaterialName( Mtl *mtl )
		{
			string sMtlName;
			if( NULL != mtl)
			{
				const  MCHAR *szMtlName = mtl->GetName().data();
				sMtlName = MbcsToUtf8( szMtlName );
			}
			return sMtlName;
		}

		// max filename (may be backward slashes)
		// is converted to  filename with forward slash
		std::string FileName( const std::string &i_Fname)
		{
			string ret(i_Fname);
			std::replace( ret.begin(), ret.end(), '\\', '/');
			return ret;
		}
		// max Color is clamped and converted to maFloatRGBA
		maFloatRGBA ColorRGBA( const Color &col )
		{
			return maFloatRGBA( 
				Clamp( static_cast<float>(col[0]) ), 
				Clamp( static_cast<float>(col[1]) ), 
				Clamp( static_cast<float>(col[2]) ),  
				1.0f);
		}

		maVector2d UV(const Point3 &uv)
		{
			maVector2d vec( Vec2(uv ) );
			vec[1] = 1.0f - vec[1];
#if defined(_DEBUG)
			if( !_finite( vec[0]) || !_finite(  vec[1] ) )
			{
				EXPLOG.WriteError( "found non-finite floats in a UV coordinate");
			}
#endif
			return vec;
		}

	} //namespace  SgpuConvert


	//========================================================================
	// Get the pivot transform corresponding to a max node
	//========================================================================
	void GetPivotTransform( INode *i_pCurNode, Matrix3 &o_Tm )
	{
		Point3 opos = i_pCurNode->GetObjOffsetPos();
		Quat orot = i_pCurNode->GetObjOffsetRot();
		ScaleValue oscale = i_pCurNode->GetObjOffsetScale();

		ApplyScaling(o_Tm, oscale);
		RotateMatrix(o_Tm, orot);
		o_Tm.Translate(opos);

		o_Tm.ValidateFlags();
	}
	//========================================================================
	//return a copy of the input file name  with the new extension
	//========================================================================
	template<>
	string ChangeExtension< string >( const  string & i_FilePath, const string &i_NewExt )
	{

		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];

		strcpy( ext, i_NewExt.c_str() );
		_splitpath(i_FilePath.c_str(), drive, dir, fileName, ext);
		_makepath(newPath, drive, dir, fileName, i_NewExt.c_str());
		return string(newPath);

	}

	template<>
	wstring ChangeExtension< wstring >( const  wstring & i_FilePath, const wstring &i_NewExt )
	{

		wchar_t	newPath[ _MAX_DIR ];
		wchar_t	drive[ _MAX_DRIVE ];
		wchar_t	dir[ _MAX_DIR ];
		wchar_t	fileName[ _MAX_FNAME ];
		wchar_t	ext[ _MAX_EXT ];

		wcscpy( ext, i_NewExt.c_str() );
		_wsplitpath(i_FilePath.c_str(), drive, dir, fileName, ext);
		_wmakepath(newPath, drive, dir, fileName, i_NewExt.c_str());
		return wstring(newPath);

	}
//========================================================================
	//return a copy of the input file name  with the new extension
	//========================================================================
	template<>
	string ChangeExtensionCustom< string >( const  string & i_FilePath, const string &i_NewExt )
	{

		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];

		strcpy( ext, i_NewExt.c_str() );
		_splitpath(i_FilePath.c_str(), drive, dir, fileName, ext);
		strcat( fileName, "_" );
		char *ext_1 = ext;
		if ( strlen( ext ) > 0 && ext[0] == '.' )
		{
			ext_1 = ext + 1;
		}
		strcat( fileName, ext_1 );
		_makepath(newPath, drive, dir, fileName, i_NewExt.c_str());
		return string(newPath);

	}

	template<>
	wstring ChangeExtensionCustom< wstring >( const  wstring & i_FilePath, const wstring &i_NewExt )
	{

		wchar_t	newPath[ _MAX_DIR ];
		wchar_t	drive[ _MAX_DRIVE ];
		wchar_t	dir[ _MAX_DIR ];
		wchar_t	fileName[ _MAX_FNAME ];
		wchar_t	ext[ _MAX_EXT ];
		
		wcscpy( ext, i_NewExt.c_str() );
		_wsplitpath(i_FilePath.c_str(), drive, dir, fileName, ext);		
		wcscat( fileName, L"_" );
		wchar_t *ext_1 = ext;
		if ( wcslen( ext ) > 0 && ext[0] == L'.' )
		{
			ext_1 = ext + 1;
		}
		wcscat( fileName, ext_1 );
		_wmakepath(newPath, drive, dir, fileName, i_NewExt.c_str());
		return wstring(newPath);

	}

	//========================================================================
	//return the file name sans the ext
	//Eg: c:/users/kgeorge/documents/3dsmax/scenes/foo.max"
	//returns foo
	//========================================================================
	template<>
	std::string GetFileTitle< std::string > ( const  std::string & i_FilePath )
	{
		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];

		_splitpath(i_FilePath.c_str(), drive, dir, fileName, ext);
		return std::string( fileName );
	}

	template<>
	std::wstring GetFileTitle< std::wstring > ( const  std::wstring & i_FilePath )
	{

		wchar_t	newPath[ _MAX_DIR ];
		wchar_t	drive[ _MAX_DRIVE ];
		wchar_t	dir[ _MAX_DIR ];
		wchar_t	fileName[ _MAX_FNAME ];
		wchar_t	ext[ _MAX_EXT ];
		
		_wsplitpath(i_FilePath.c_str(), drive, dir, fileName, ext);		
		return std::wstring( fileName );

	}


	//=============================================================================
	//Find the correct submaterial of a multi-material,
	//given the id
	//i_pMtl = the input multi-material
	//i_nId = input id of the submaterial
	// KG: 6/10/09
	// This is no longer used for extracting the submaterials
	// When the submaterials didnt have consecutive ID-s starting from 1,
	// this routine gave incorrect submaterials. 
	// Using just pMuluLaterial->GetSubMtl( non_consecutive_id-1 )
	// gave the correct result.
	// Try to test this scenario with Box_mat_Test in ftp.studiogpu.com/MaxAssets
	//=============================================================================


	Mtl * GetSubMaterialById( Mtl *i_pMtl, SgpuExportOptions &i_Options,  int i_nId )
	{
		assert( NULL !=  i_pMtl );
		assert( i_pMtl->IsMultiMtl() );
		IParamBlock2* subMaterialParameters = (IParamBlock2*) i_pMtl->GetReference(0);
		if (NULL ==  subMaterialParameters )
		{
			return i_pMtl->GetSubMtl( i_nId );
		}
		int nSubMtls = i_pMtl->NumSubMtls();
		for( int j=0; j < nSubMtls; ++j)
		{
			int sid = subMaterialParameters->GetInt(  3, static_cast<TimeValue>( i_Options.m_StartTime ), j);
			if( sid == i_nId )
			{
				if( i_nId != j)
				{
					int breakHere =0;
				}
				return i_pMtl->GetSubMtl( j );
			}
		}
		return NULL;
	}

	


	//========================================================================
	// The whole extraction of version info
	// is done at the construction of this
	// structure and can throw exceptions
	// So the construction has to be tried within a try-catch block
	//========================================================================
	
	WinVersionInfo::WinVersionInfo( const string &modulename ):
	m_ModuleName( modulename),
		m_pVersionBuffer(NULL)
	{
		memset( m_Version, 0, sizeof(int) * 4);	
		m_ModuleFileName[0] = '\0';
		HMODULE hModule = GetModuleHandle( m_ModuleName.c_str() );
		if( NULL == hModule )			
		{
			throw runtime_error(string("cannot find handle for ") + m_ModuleName);
		}
		if (!GetModuleFileName (hModule, m_ModuleFileName, m_ModuleFileNameSize ))
		{
			throw runtime_error(string("cannot find filename for ") + m_ModuleName);
		}
		DWORD doomy;
		unsigned int versionInfoSize=GetFileVersionInfoSize (m_ModuleFileName, &doomy);
		if ( versionInfoSize == 0 )			
		{
			throw runtime_error(string("cannot find versionsize for ") + m_ModuleName);	
		}
		m_pVersionBuffer =new char[ versionInfoSize ];
		// Find the verion resource
		if (!GetFileVersionInfo(m_ModuleFileName, 0, versionInfoSize, m_pVersionBuffer))
		{
			delete [] m_pVersionBuffer;
			m_pVersionBuffer = NULL;
			throw runtime_error(string("cannot find versioninfo for ") + m_ModuleName);		
		}
		unsigned int *versionTab = NULL;
		unsigned int versionSize =0;
		if (!VerQueryValue (m_pVersionBuffer, "\\", (void**)&versionTab,  &versionSize))
		{
			delete [] m_pVersionBuffer;
			m_pVersionBuffer = NULL;
			throw runtime_error(string("cannot find versioninfo for ") + m_ModuleName);		
		}
		// Get the pointer on the structure
		VS_FIXEDFILEINFO *info=(VS_FIXEDFILEINFO*)versionTab;
		if (info)
		{
			m_Version[0] = info->dwFileVersionMS>>16, 
				m_Version[1] = info->dwFileVersionMS & 0xffff, 
				m_Version[2] = info->dwFileVersionLS>>16,  
				m_Version[3] = info->dwFileVersionLS & 0xffff;
			stringstream ss;
			ss << m_Version[0] << "." << m_Version[1] << "." << m_Version[2]  << "." << m_Version[3];
			m_VersionString = ss.str();

		}	
	}

	WinVersionInfo::~WinVersionInfo(){ 
		delete [] m_pVersionBuffer;
	}
	

	void StripTSTR( TSTR &tstr )
	{
		int k=0;
		for(; k < tstr.Length() && _istspace( tstr[k] ); ++k);
		int l=tstr.Length();
		for(; l >0 && _istspace( tstr[l-1] ); --l); 
		if( l < k)
		{
			l =k;
		}
		tstr = tstr.Substr(k, l-k);		
	}


//=============================================================================
// Convert an ansi string to microsoft unicode, based on the
// current codepage settings for file apis.
// 
// For smaller strings, use a static cache
// For larger strings, use a heap
//=============================================================================

std::string MbcsToUtf8(const char *i_zMbcsString )
{
  std::wstring wret;
  std::string sret;
  int nByte = MbcsToUnicode( i_zMbcsString, wret );
  if( nByte > 0)
  {
	sret = envString::WideCharToUTF8( wret );
  }
  return sret;
}



//=============================================================================
// Convert an ansi string to microsoft unicode, based on the
// current codepage settings for file apis.
// 
// For smaller strings, use a static cache
// For larger strings, use a heap
//=============================================================================

int MbcsToUnicode(const char *i_zMbcsString, std::wstring &o_WString )
{
  int nByte;
  static wchar_t zWcharFilenameStatic[ 1024];
  wchar_t *zWcharFilenameDynamic = NULL;
  wchar_t *zWcharFilename = &zWcharFilenameStatic[ 0 ];
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  nByte = MultiByteToWideChar(codepage, 0, i_zMbcsString, -1, NULL,0)*sizeof(wchar_t);
  if( nByte > 1024)
  {
	  zWcharFilenameDynamic = new wchar_t [ nByte  ];
	  if( zWcharFilenameDynamic==0 ){
		  return 0;
	  }
	  zWcharFilename = zWcharFilenameDynamic ;
  }
  nByte = MultiByteToWideChar(codepage, 0, i_zMbcsString, -1, zWcharFilename, nByte);
  if( nByte > 0 )
  {
	  o_WString = std::wstring( zWcharFilename );
  }
  delete [] zWcharFilenameDynamic;
  return nByte;
}

int UnicodeToMbcs(const std::wstring &i_WString, MSTR &o_MbcsString )
{
  int nByte;
  static char zMbcsFilenameStatic[ 1024];
  char *zMbcsFilenameDynamic = NULL;
  char *zMbcsFilename = &zMbcsFilenameStatic[ 0 ];
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  nByte = WideCharToMultiByte(codepage, 0, i_WString.c_str(), -1, NULL,0, NULL,NULL )*sizeof(char);
  if( nByte > 1024)
  {
	  zMbcsFilenameDynamic = new char [ nByte  ];
	  if( zMbcsFilenameDynamic==0 ){
		  return 0;
	  }
	  zMbcsFilename = zMbcsFilenameDynamic ;
  }
  BOOL bUsedDefaultChar=FALSE;
  nByte = WideCharToMultiByte(codepage, 0, i_WString.c_str(), -1, zMbcsFilename, nByte, NULL, &bUsedDefaultChar );
  if( nByte > 0 )
  {
	  o_MbcsString = zMbcsFilename;
  }
  delete [] zMbcsFilenameDynamic;
  return nByte;
}

//push_back all the node names starting from SceneRoot
//to i_pNode into o_Path
void GetPath( INode *i_pNode, std::deque<std::string> &o_Path )
{
	assert( NULL != i_pNode );
	MSTR path = i_pNode->GetName();
	INode *pCurNode = i_pNode;
	std::back_insert_iterator< std::deque<std::string> >  bit( o_Path );
	while( pCurNode != NULL )
	{
		MSTR curName = pCurNode->GetName();
		std::string sNodeName ( SgpuConvert::NodeName( curName ) );
		*bit++ = sNodeName;
		pCurNode = pCurNode->GetParentNode();
	}
	//reverse the order the path components 
    //so that ancestors appear earlier than descendants
	std::reverse( o_Path.begin(), o_Path.end() );
}

	LibXLTInitCleanupWrapper::LibXLTInitCleanupWrapper()
	{
		matShaderParser::Initialize();
	}

	LibXLTInitCleanupWrapper::~LibXLTInitCleanupWrapper()
	{
		matShaderParser::DeInitialize();
	}

void strip( std::wstring &io_str )
{
	std::wstring::size_type fpos = io_str.find_first_not_of(L" \t\r\n\v\f", 0 );
	std::wstring::size_type lpos = io_str.find_last_not_of(L" \t\r\n\v\f");
	if( fpos < lpos  )
	{
		if( lpos != std::wstring::npos )
		{
			io_str = io_str.substr( fpos, lpos - fpos + 1 );
		} else
		{
			io_str = io_str.substr( fpos );
		}
	} else if ( fpos == lpos )
	{
			if( lpos != std::wstring::npos )
			{
				io_str = io_str.substr( fpos, 1 );
			}
	}
}


} //namespace SgpuExp