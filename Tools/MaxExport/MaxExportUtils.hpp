/*****************************************************************************
**  MaxExportUtils.hpp
**
**	Collection of all utility functions and classes, 
**	that glues Sgpu api and max api
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_MAXEXPORTERUTILS_HPP
#error MAXEXP_MAXEXPORTERUTILS_HPP multuply defined!!
#endif
#define MAXEXP_MAXEXPORTERUTILS_HPP


#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
#endif
#ifndef  MA_VECTOR2D_HPP
#include "Core/Ma/maVector2d.hpp"
#endif
#ifndef  MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#include "max.h"
#include <string>
#include <map>
#include <hash_set>
#include <exception>
#include <functional>
#include <tchar.h>
#include <ctime>
#include <list>
#include <iostream>
#include <deque>




//forward declarations
class INode;
class Matrix3;
class Animatable;
class fsXMLWriter;
class fsLocator;
class itString;
class mdlNodeInfo;
namespace MaxExp
{
	class ExportDoc;
}

class SgpuExportOptions;

namespace MaxExp
{

	//========================================================================
	// common exception to denote failure of export 
	// due to Sgpu specific logic
	//========================================================================

	class export_failure : public std::runtime_error
	{
	public:
		export_failure():runtime_error("max export failed at sgpu code"){}
		~export_failure(){}
	};

	class export_cancel : public std::runtime_error
	{
	public:
		export_cancel():runtime_error("max export cancelled by user"){}
		~export_cancel(){}
	};

	class export_ignore : public std::runtime_error
	{
	public:
		export_ignore():runtime_error(""){}
		~export_ignore(){}
	};

	//========================================================================
	// Simple Profiler class 
	// that writes the time elapsed to the logger upon destruction
	//========================================================================
	class SimpleProfile
	{
	public:
		SimpleProfile( const std::string &name );
		~SimpleProfile();
		const std::string m_Name;
		clock_t m_Start;
		clock_t m_Stop;
	};

	//========================================================================
	// Accepts an Mbcs string and returns a std::string or std::wstring.
	// The general version works for std::string
	// A specialized version is defined  for wstring
	//========================================================================

	template< class StrClass >
	StrClass FromMbcs(const char *szName )
	{
		return StrClass( szName );
	}

	template<>
	std::wstring FromMbcs< std::wstring > ( const char *szName );
	//========================================================================
	// An indenting class used for pretty printing
	//========================================================================
	struct Indent 
	{
		const std::string &data(){ return _data;}		
		void pop()
		{
			_data.erase( _data.length() -1);
		}
		void push() 
		{
			_data += " ";
		}
		std::string _data;
	};


	//========================================================================
	// A wrapper for the progress report interface
	//========================================================================
	// dummy function for progress bar
	inline DWORD WINAPI fn(LPVOID)
	{
		return 0;
	}

	class InterfaceProgressSP 
	{
	public:
		InterfaceProgressSP( Interface *i):_i(i)
		{
			assert( NULL != _i);
			_i->ProgressStart( _T("Sgpu Export..."), TRUE, fn, NULL);
		}
		~InterfaceProgressSP()
		{
			_i->ProgressEnd();
		}
	private:
		Interface *_i;
	};


	//========================================================================
	// This performs logging
	//
	// This is designed as a singleton class, (equivalent of a global variable)
	// so that we call call it from anywhere
	// For a single invocation of export, the singleton logger instance
	// should be properly 'Init'-ed at the beginning of the export
	// and 'CleanUp'-ed at the end of the export. To facilitate this
	// we provide an embedded wrapper Wrap.
	// See DoExport in SgpuExport.cpp for details

	// The write methods all accept var-args
	// Also someof the fsXmlWriter's write methods are
	// also exposed.

	//  To facilitate accurate pairing of WriteStartElement and 
	//  WriteEndElement, we provide another wrpapper class
	//  called WriteStartElementWrap


	// Please note the pre-processor macro EXPLOG
	// From anywhere in the code, we can write to the log 
	// as 'EXPLOG.WriteInfo( "blah blah %s %d\n", szName, id );
	//========================================================================

	class ExportLogger 
	{
	public:
		//see above description
		class Wrap{
		public:
			Wrap( const wchar_t *i_pszExportFilename);
			void Init( const wchar_t *i_pszExportFilename );
			~Wrap();
		};

		//see above description
		class WriteStartElementWrap{
		public:
			WriteStartElementWrap( const char *i_pzFmt, ... );
			~WriteStartElementWrap();
		private:

			const static int m_nMsgBufferLen = 1023;
			char m_MsgBuffer[ m_nMsgBufferLen+1  ];		
		};


		//higher level interface
		//write an info, warning or an error
		void WriteInfo( const char *fmt, ...);
		void WriteWarning( const char *fmt, ...);
		void WriteError( const char *fmt, ...);
		void WriteHypothesisTest(  const char *fmt, ...);

		//lower level interface
		void WriteElement( std::string & key, const std::string &val);  
		void WriteStartElement( std::string &selement);
		void WriteEndElement();

		//access to the static singleton access
		static ExportLogger &Get() { return m_TheLogger;}

	private:			
		void Init( const wchar_t *exportFilename);
		void CleanUp();

	private:
		ExportLogger( ):
		   m_pFsXMLWriter(NULL),
			   m_pFsLocator(NULL),
			   m_Info("info"),
			   m_Warning("warning"),
			   m_Error("error"),
			   m_HypothesisTest("hypo")
		   {}

		   ~ExportLogger();

	private:
		//locator and logwriter
		fsLocator   *m_pFsLocator;
		fsXMLWriter  *m_pFsXMLWriter;

		//filename of the log
		std::wstring m_LogFileName;
		//filename of export output
		std::wstring  m_ExportFileName; 


		const static int m_nMsgBufferLen = 1023;

		char m_MsgBuffer[ m_nMsgBufferLen+1  ];		



		std::string m_Warning;
		std::string m_Info;
		std::string m_Error;
		//warnings instituted in order to test a 
		//hypothesis. (ie You are not sure whether
		//certain tings happen under certain conditions.
		//If it happens give us a warning )
		std::string m_HypothesisTest;

		//static singleton instance
		static ExportLogger m_TheLogger;
	};


#define EXPLOG ExportLogger::Get()

	//========================================================================
	// Get the string description of the class from the Class_ID
	//
	// The ClassDescs is privately inherited from a map class
	// If 'class B' derives privately from 'class A', then it is syntactically
	// equal to 'class B has a class A'. 
	// http://www.parashift.com/c++-faq-lite/private-inheritance.html

	// In addition to this feature,
	// the ClassDescs is also a singleton
	//
	// Note: deriving off stl container classes is not usually recommended
	// since they dont have any virtual destructors.
	// But this doesnt apply here,
	// since private inheritance assures that we can never upcast 
	// a ClassDescs instance to a map< Class_ID, string> instance.
	// So the issue of memory leak, due to the lack of virtual destructors
	// never come up.
	//========================================================================
	class ClassDescs : private std::map< Class_ID, std::string>
	{
	public:
		//parent type
		typedef std::map< Class_ID, std::string>  ParentType; 
		//staitc accessor for the singleton instance
		static const ClassDescs &Get() { return _theClassDesc; }
		//the  parent's insert methids are public now
		//anything else is private to future derived classes 
		//or outside users
		using ParentType::insert; 
		//get the class name given a class id
		const std::string & ClassName( const Class_ID  &cid ) const
		{
			const_iterator cit =  find( cid );
			if( cit == end() )
			{ 
				//return the unknown string
				return m_Unknown;
			} 
			else
			{
				return cit->second;
			}
		}
	private:
		ClassDescs();
		~ClassDescs(){}
		static ClassDescs _theClassDesc;
		const static std::string m_Unknown;
	};

	//========================================================================
	// Sgpu specific classification of various max objects
	//========================================================================
	namespace MaxObjectType
	{
		typedef enum TypeVal {Unknown, Bone, Mesh, Camera, Light, Material, XrefObject, XrefMtl, Helper , SpaceWrap, Target};	
		//Get the TypeVal corresponding to the object referred to by
		//i_pCurNode.  If i_bSupportXref is false,
		//any xref nobject or material has to be dereferenced and 
		//re-evaluated
		TypeVal Get( INode *i_pCurNode, SgpuExportOptions &i_Options );
		//Another form of the above function
		void Get( INode *i_pCurNode, SgpuExportOptions &i_Options, std::string &o_StypeName, MaxObjectType::TypeVal &o_ObjType );

		//The workhorse function for the above  
		//Note that the relavant object  to by the node is passed in
		//THis is because the relavant object could be different
		//from the actual objct referred to by the node.
		TypeVal GetAux( INode *i_pCurNode, SgpuExportOptions &i_Options, Object *obj );
		//Get  the typename from the typeval
		const std::string &GetTypeName( TypeVal i_ObjType );
		bool IsXrefMaterial( Animatable *b );
		bool IsXrefObject( Animatable *b);
		bool IsSuperClassDerivedObj ( const SClass_ID &sid );
	} //namepace MaxObjectType




	//========================================================================
	// Get string names for unit enums rom max
	//========================================================================
	namespace  MaxUnit
	{

		//get the  name of the unit
		//unitType is a max enum
		const std::string & GetUnitName( int  unitType );
	}// namespace MaxUnit	



	//========================================================================
	// conversion from max classes to equivalent sgpu classes
	//========================================================================
	namespace SgpuConvert
	{
		//Clamps the value of 'f' to the closed interval [0.0f,1.0f]]
		inline  float Clamp( float f )
		{
			return (f >= 0) ?   ( ( f <= 1.0f ) ? f  : 1.0f ) : 0.0f;
		}
		//Clamps the 4 component floats 
		template < class Vec4 >
		Vec4 Clamp4( const Vec4 & vec ) 
		{
			return Vec4( Clamp(vec[0]), Clamp(vec[1]), Clamp(vec[2]), Clamp(vec[3]) );
		}

		//Clamps the 3 component floats 
		template < class Vec3 >
		Vec3 Clamp3( const Vec3 & vec ) 
		{
			return Vec3( Clamp(vec[0]), Clamp(vec[1]), Clamp(vec[2]) ) ;
		}

		template < typename T >
		T Clamp( T i_Val, T i_cMinVal,T i_cMaxVal )
		{
			return (i_Val < i_cMinVal ) ? i_cMinVal : ( ( i_Val > i_cMaxVal ) ? i_cMaxVal : i_Val   );
		}
		//convert the name of a Max Node
		std::string NodeName(const MCHAR *i_szName);

		//If this max node instanced,
		//we would end up putting a common mdlNodeInfo to stand in
		//for all instances.
		//That common node should have a name derived from the node name

		std::string InstancedNodeName( const MCHAR *i_szName );
		//max Vector is  converted to a maVector3d		
		template <  class T >
		maVector3d Vec3(T &i_Vec)
		{
#if defined(_DEBUG)
			if( !_finite( i_Vec[0] ) ||  !_finite( i_Vec[1] ) || !_finite( i_Vec[2]) )
			{
				EXPLOG.WriteError( "Non finite float in a 3d vector!" );
			}
#endif
			return maVector3d((float)i_Vec[0], (float)i_Vec[1], (float)i_Vec[2]);
		}
		//max Vector is converted to a maVector2d
		template<class T>
		maVector2d Vec2(T &i_Vec)
		{

#if defined(_DEBUG)
			if( !_finite( i_Vec[0] ) ||  !_finite( i_Vec[1] )  )
			{
				EXPLOG.WriteError( "Non finite float in a 2d vector!" );
			}
#endif
			return maVector2d((float)i_Vec[0], (float)i_Vec[1]);
		}
		// max MAtrix3 is converted to a maMatrix4x4	
		maMatrix4x4 Ma4x4(const Matrix3 &i_Mat );
		// a maMatrix4x4 is converted to max Matrix3
		Matrix3 MaxM3( const maMatrix4x4 &i_Mat);
		// max filename (may be backward slashes)
		// is converted to  filename with forward slash
		std::string FileName( const std::string &i_Fname); 
		// max Color is clamped and converted to maFloatRGBA
		maFloatRGBA ColorRGBA( const Color &i_Col );	
		//convert a max uv coordinate into sgpu uv coordinates 
		maVector2d UV(const Point3 &uv);
		// max material name is converted
		std::string MaterialName( Mtl *i_Mtl );
	}

	//Get the pivot transform of the current node
	void GetPivotTransform( INode *i_pCurNode, Matrix3 &o_Tm );

	//get the submaterial of the multi-mtl coresponding to the given id
	Mtl * GetSubMaterialById( Mtl *i_pMtl, SgpuExportOptions &i_Options, int i_nId ) ;

	//return the input fie name with the new extension
	template< class Str >
	Str ChangeExtension( const  Str & i_FilePath, const Str &i_NewExt );
	template<>
	std::string ChangeExtension< std::string > ( const  std::string & i_FilePath, const std::string  &i_NewExt );
	template<>
	std::wstring ChangeExtension< std::wstring > ( const  std::wstring & i_FilePath, const std::wstring  &i_NewExt );

	//If the input file is <file path dir>/<file name>.<ext>,
	//return the string <file path dir>/<file name>_<ext>.<new ext>
	template< class Str >
	Str ChangeExtensionCustom( const  Str & i_FilePath, const Str &i_NewExt );
	template<>
	std::string ChangeExtensionCustom< std::string > ( const  std::string & i_FilePath, const std::string  &i_NewExt );
	template<>
	std::wstring ChangeExtensionCustom< std::wstring > ( const  std::wstring & i_FilePath, const std::wstring  &i_NewExt );

	//return the file name sans the ext
	//Eg: c:/users/kgeorge/documents/3dsmax/scenes/foo.max"
	//returns foo
	template< class Str >
	Str GetFileTitle( const  Str & i_FilePath );
	template<>
	std::string GetFileTitle< std::string > ( const  std::string & i_FilePath );
	template<>
	std::wstring GetFileTitle< std::wstring > ( const  std::wstring & i_FilePath );

#define SGPU_BOOL_STRING( a ) ( (a ) ? "true" : "false" )


	//========================================================================
	// Generic visitor of an mdl hierarchy
	// io_MdlNode, the node that is visited
	// visit, the work that need be done upon the node 
	// visit should have the following structure
	// struct visit
	// {
	//   typedef <visit specific stack type> stack_type;
	//		//stack_type should be copy_constructible and
	//		//assignable-
	//      //Eg: typedef maTransform4x4 stack_type;
	//
	//	 //This is called prior to visting the children
	//	 void Pre( shared_ptr<mdlNodeInfo> &node);
	//	  
	//   //This is called after to visting the children
	//	 void Post( shared_ptr<mdlNodeInfo> &node);
	//	 stack_type m_Stack;
	// };
	//========================================================================

	//Example visit struct
	struct PrintNodeDepthVisit
	{
		typedef int stack_type;
		PrintNodeDepthVisit():m_Stack(0){}
		void Pre( shared_ptr<mdlNodeInfo> io_MdlNode )
		{
			int curDepth = 1 + m_Stack;
			//The following line is commented out because this is just
			//an example and 'cout' would require iostream
			//
			//cout << io_MdlNode->m_NodeName.c_str() << " depth: " << curDepth;
			m_Stack = curDepth;
		}
		void Post ( shared_ptr<mdlNodeInfo> io_MdlNode ){}
		stack_type m_Stack;
	};




	template< class Visit >
	void VisitMdlHierarchyRec(shared_ptr<mdlNodeInfo> io_MdlNode, Visit &visit )
	{		
		//Does the stuff that need
		//be done prior to visiting children
		visit.Pre(io_MdlNode);	

		std::vector< shared_ptr<mdlNodeInfo> >::iterator chit;
		//make a copy of the current stack
		Visit::stack_type visitStackCopy( visit.m_Stack );
		for( chit = io_MdlNode->m_Children.begin(); chit != io_MdlNode->m_Children.end(); ++chit )
		{
			shared_ptr<mdlNodeInfo> ch = *chit;		
			//visit the child
			VisitMdlHierarchyRec( ch, visit );
			//restore the stack
			visit.m_Stack =  visitStackCopy;
		}
		//does the stuff that need be done
		//after visiting children
		visit.Post(io_MdlNode);
	}


	template< class Visit >
	void VisitINodeHierarchyRec(INode *i_pCurNode, Visit &io_Visit )
	{		
		//Does the stuff that need
		//be done prior to visiting children
		io_Visit.Pre(i_pCurNode);	

		//make a copy of the current stack
		Visit::stack_type visitStackCopy( io_Visit.m_Stack );
		for( int j = 0;  j < i_pCurNode->NumberOfChildren(); ++j )
		{
			INode *pch  = i_pCurNode->GetChildNode( j );		
			//visit the child
			VisitINodeHierarchyRec( pch, io_Visit );
			//restore the stack
			io_Visit.m_Stack =  visitStackCopy;
		}
		//does the stuff that need be done
		//after visiting children
		io_Visit.Post(i_pCurNode);
	}

	//========================================================================
	// Getting the collection of user defined properties of  this node
	// Each node can have a user defined property string
	// This user defined property string is parsed as a sequence of
	// '# key = val'
	// resulting in  key-value pairs, which are inserted in the container
	// Eg:
	// 'this part wont be parsed
	//  haha haha
	//  end of part that wont be parsed
	//  # self illumination = 1.0f
	//  # double-sided = true
	//  # master node = sphere 1'
	//
	// The resulting associative container will contain the 
	// following key-value pairs
	// 'self illumination':'1.0f', 'double-sided':'true', 'master-node':'sphere 1'

	// We derive UserDefinedProperties privately from
	// std::map< std::string, std::string>
	// This is because private derivation is equivalent to composition
	// http://www.parashift.com/c++-faq-lite/private-inheritance.html
	// Also to do, make this unicode supported. 
	//========================================================================

	template< typename C >
	struct less_i : std::binary_function< std::basic_string<C>, std::basic_string<C>, bool>
	{
		template< typename C2>
		struct Traits : public std::unary_function< C2, C2>
		{
			C2 operator()( const C2 & arg ) const
			{
				return arg;
			}
		};

		template<>
		struct Traits<char>: public std::unary_function< char, char >
		{
			char operator()( const char & arg ) const
			{
				return tolower(arg);
			}
		};

		template<>
		struct Traits<wchar_t>: public std::unary_function< wchar_t, wchar_t >
		{
			wchar_t operator()( const wchar_t & arg ) const
			{
				return towlower(arg);
			}
		};


		typedef std::basic_string<C> bstring;
		bool operator()( const bstring &left, const bstring &right ) const
		{
			bstring lleft(left);
			bstring lright(right);
			std::transform( lleft.begin(), lleft.end(), lleft.begin(), Traits<C>());
			std::transform( lright.begin(), lright.end(), lright.begin(), Traits<C>());
			return lleft < lright;
		}
	};


	//========================================================================
	// Extracting Version Info from dll-s
	// 
	// The whole extractio is done at the construction of this
	// structure and can throw exceptions
	// So the construction has to be tried within a try-catch block
	//========================================================================
	struct WinVersionInfo
	{
		WinVersionInfo(const std::string &moduleName);
		~WinVersionInfo();

		std::string m_ModuleName; //input moduloe name (eg: "foo.dll")	
		int m_Version[4]; //a quad of numbers which specify the version
		// 1.0.1.11			
		std::string m_VersionString;//the quad of version numbers expressed as a string
	private:
		char * m_pVersionBuffer; //tempoarary buffer which holds version info
		const static int m_ModuleFileNameSize = 1024;	
		char m_ModuleFileName[m_ModuleFileNameSize]; //temporary buffer for filename
	};


	//========================================================================
	// A less operator for INode
	// 
	//  returns true if parg1 is a proper ancestor of  parg2
	//========================================================================

	struct LessINode :  public std::binary_function< const INode*, const INode*, bool >
	{

		bool IsArg1AncestorOfArg2( const INode *parg1, const INode *parg2 ) const
		{			
			assert( NULL != parg1 && NULL != parg2 );			
			const MCHAR *sz1 = const_cast< INode*> (parg1)->GetName();
			const MCHAR *sz2 = const_cast< INode*> (parg2)->GetName();
			INode *ancestor = NULL;
			INode *p = const_cast< INode * > ( parg2 );
			bool isLess = false;
			const int  limit = 1000000;
			int iiter = 0;
			//we are assuming that
			//every inode tree is roted at the GetCOREInterface()->GetRootNode()
			// the parent of a root node is NULL
			bool retVal = false;
			while( ( ancestor =  p->GetParentNode()  ) != NULL )
			{
				const MCHAR * szAncestor = ancestor->GetName();
				if( ancestor == parg1 )
				{
					retVal =  true;
					break;
				}
				if( iiter++ > limit )
				{
					EXPLOG.WriteError( "INode hierarchy depth > 1000000" );
					throw export_failure();
				}
				p = ancestor;
			}
			return retVal;
		}


		bool operator()( const INode *parg1, const INode *parg2 ) const
		{			
			if( NULL == parg1 && NULL == parg2  )
			{
				return false;
			}
			if( NULL == parg2 )
			{
				return false;

			}
			if( parg1 == parg2 )
			{	
				return false;
			}
			assert( NULL != parg1 && NULL != parg2 );
			return IsArg1AncestorOfArg2( 	parg1, parg2 );
		}
	};

	struct DbgPrintINodeName: std::unary_function< INode *, INode * >
	{
		INode *operator()( INode * i_pCurNode) const
		{
			MCHAR *szname = i_pCurNode->GetName();
			_RPT1( _CRT_WARN, "%s\n", szname );
			return i_pCurNode;
		}
	};


	struct TransformRTVec3d: std::unary_function< maVector3d, maVector3d >
	{
		TransformRTVec3d( const maMatrix4x4 &rt):
	m_RT( rt )
	{
	}

	maVector3d &operator()( maVector3d &i_Vec3d ) const
	{	
		m_RT.TransformDir( i_Vec3d );
		return i_Vec3d;			
	}

	const maMatrix4x4 &m_RT;
	};

	//compute a hash value of the nodes' name
	struct HashINodes: std::unary_function< INode *, INode * >
	{
		HashINodes(): m_HashVal(0){}
		INode *operator()( INode * i_pCurNode) 
		{
			MCHAR *szname = i_pCurNode->GetName();
			size_t curVal = stdext::hash_value( szname );
			m_HashVal =  16777619U * m_HashVal ^ curVal ;
			return i_pCurNode;
		}
		size_t m_HashVal;
	};


	template< class I >
	bool IsAncestorPresentInContainer(INode *i_pCurNode, I &b, I &e ) 
	{
		assert( NULL != i_pCurNode );
		INode *p = i_pCurNode->GetParentNode();
		while( NULL != p)
		{
			if( std::find(b, e, p ) != e )
			{
				return true;
			}
			p = p->GetParentNode();
		}
		return false;
	}

	typedef stdext::hash_set< INode * , stdext::hash_compare< INode *,   LessINode >  > OrderedINodeHS;
	typedef std::vector< INode * > INodeVector;
	typedef stdext::hash_set< INode *   > INodeHS;

	inline bool EpsilonEqualZero( float a , float eps = maConstants::c_fEpsilon ) { return fabsf(a) < eps; }
	inline bool EpsilonEqualZero( const maVector2d &v , float eps =  maConstants::c_fEpsilon ) { return EpsilonEqualZero( v[0], eps ) && EpsilonEqualZero( v[1], eps ) ; }
	inline bool EpsilonEqualZero( const maVector3d &v,  float eps =  maConstants::c_fEpsilon ) { return EpsilonEqualZero( v[0], eps ) && EpsilonEqualZero( v[1], eps ) &&   EpsilonEqualZero( v[2], eps ) ; }
	inline bool EpsilonEqualZero( const maVector4d &v,  float eps =  maConstants::c_fEpsilon ) { return EpsilonEqualZero( v[0], eps ) && EpsilonEqualZero( v[1], eps ) &&   EpsilonEqualZero( v[2], eps ) && EpsilonEqualZero( v[3], eps ); }


	std::string MbcsToUtf8(const char *i_pSzString );

	int MbcsToUnicode(const char *i_pSzString, std::wstring &o_WString );

	int UnicodeToMbcs(const std::wstring &i_WString, MSTR &o_mbcsString );

#define MBCSTOLPCSTR( i_pSzString ) ( MbcsToUtf8( i_pSzString ).c_str() )


	//push_back all the node names starting from SceneRoot
	//to i_pNode into path
	void GetPath( INode *i_pNode, std::deque<std::string> &path );

	struct HiddenNodeFilter
	{
		bool operator()(  INode * i_pCurNode ) const
		{
			return  ( i_pCurNode->IsHidden() ) ? true : false;
		}
	};


	/**
	// Does the Initialization of LibXLT packages on construction,
	// Does cleanup, on destruction
	//
	*/
	struct LibXLTInitCleanupWrapper
	{
		LibXLTInitCleanupWrapper();

		~LibXLTInitCleanupWrapper();
	};



	template< typename C, int NStaticSize >
	struct DynamicBuffer
	{
	public:
		DynamicBuffer(int nSize):
		  m_pDynamicBuffer( NULL ),
			  m_Size( NStaticSize )
		  {
			  if( nSize > NStaticSize )
			  {
				  m_pDynamicBuffer = new C [ nSize ];
				  m_Size = nSize;
			  }
		  }
		  ~DynamicBuffer()
		  {
			  if( m_pDynamicBuffer )
			  {
				  delete [] m_pDynamicBuffer;
			  }
		  }

		  C *GetBuffer()
		  {
			  return ( m_pDynamicBuffer ) ? m_pDynamicBuffer : &m_StaticBuffer[0];
		  }
		  const C *GetBuffer() const
		  {
			  return ( m_pDynamicBuffer ).? m_pDynamicBuffer : &m_StaticBuffer[0];
		  }
		  int GetSize() const 
		  {
			  return m_Size;
		  }
		  C		m_StaticBuffer[ NStaticSize ];
		  C		*m_pDynamicBuffer;
		  int	m_Size;
	};

	//========================================================================
	// template parameter S should be std::string or std::wstring
	// Splits the given input string into a vector of strings
	// Any character in the i_Delims would be used fo splitting.
	//========================================================================
	template <typename S>
	void tokenize( const S & i_Input, const S &i_Delims ,  std::vector< S >  &o_Output )
	{
		S::size_type lpos = 0;
		S::size_type fposNotOf=0;

		while ( lpos != S::npos && lpos >= fposNotOf )
		{
			fposNotOf = i_Input.find_first_not_of(i_Delims, lpos);
			lpos = i_Input.find_first_of( i_Delims, fposNotOf );

			if( lpos != S::npos )
			{
				o_Output.push_back( i_Input.substr( fposNotOf, lpos-fposNotOf ) );
			} else if( lpos > fposNotOf )
			{
				o_Output.push_back( i_Input.substr( fposNotOf, lpos-fposNotOf ) );
			}
		}
	}


	//========================================================================
	//Strips inplcace, the given string off the whitespace characters
	//========================================================================
	void strip( std::wstring &io_str );

} //namespace MaxExp