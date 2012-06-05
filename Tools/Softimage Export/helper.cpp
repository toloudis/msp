//*****************************************************************************
/*!	\file helper.cpp
	\brief Helper classes for reading and writing mesh data.
*/
//*****************************************************************************

#include <xsi_string.h>
#include <xsi_value.h>
#include <xsi_status.h>

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_string.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_menu.h>
#include <xsi_model.h>
#include <xsi_parameter.h>
#include <xsi_x3dobject.h>
#include <xsi_selection.h>
#include <xsi_primitive.h>
#include <xsi_polygonmesh.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_geometryaccessor.h>
#include <xsi_longarray.h>
#include <xsi_floatarray.h>
#include <xsi_doublearray.h>
#include <xsi_bitarray.h>
#include <xsi_envelopeweight.h>
#include <xsi_customproperty.h>
#include <xsi_griddata.h>
#include <xsi_clusterproperty.h>
#include <xsi_material.h>
#include <xsi_ppglayout.h>
#include <xsi_userdatamap.h>
#include <xsi_imageclip2.h>
#include <xsi_source.h>
#include <xsi_texture.h>
#include <xsi_math.h>
#include <xsi_uitoolkit.h>

#include <xsi_vector3f.h>
#include <xsi_vector2f.h>
#include <xsi_oglmaterial.h>
#include <xsi_ogltexture.h>
#include <xsi_kinematics.h>
#include <xsi_triangle.h>
#include <xsi_trianglevertex.h>
#include <xsi_scene.h>
#include <xsi_project.h>
#include "sgpuString.hpp"

#include "helper.h"
#include "wchar.h"
#include "string.h"
//#include <wchar.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <exception>

#include "sgpuNode.hpp"
#include "sgpuString.hpp"
#include "sgpuMatrix.hpp"

using namespace std;
#define MAX_CHARS 200

int SafeLongToInt( LONG l )
{
	return static_cast< int > ( l );
}


// logger
CMeshFileWriter::CMeshFileWriter() : p (NULL) {}
CMeshFileWriter::~CMeshFileWriter()
{
	if (p)
	{
		fclose(p);
	}
}

XSI::CStatus CMeshFileWriter::Init(const XSI::CString& in_outFile )
{
	p = fopen( in_outFile.GetAsciiString(), "wt" );
	
	if (!p) return XSI::CStatus::InvalidArgument;
	
	return XSI::CStatus::OK;	
}

void CMeshFileWriter::Header( const XSI::CString& in_strText )
{	
	Write( L"\n" );
	XSI::CString str = in_strText;
	str += L"\n{\n";
	
	Write( str );
}

void CMeshFileWriter::Footer()
{
	Write( L"}\n\n" );
}

void CMeshFileWriter::Write( const XSI::CString& in_str )
{	
	const char* psz = in_str.GetAsciiString();
	size_t nLen = strlen(psz);
	fwrite( psz, sizeof(char), nLen, p );		 
}

void CMeshFileWriter::Write( const wchar_t* in_pstr )
{
	XSI::CString str(in_pstr);
	Write( str );
}

void CMeshFileWriter::Write( XSI::CValue in_val )
{
	Write( in_val.GetAsText().GetWideString() );
}

void CMeshFileWriter::EOL()
{
	Write( L"\n" );
}

CMeshFileReader::CMeshFileReader() : 
	p (NULL)
	,m_pszRow(NULL)
	,m_pLongs(NULL)
	,m_pDoubles(NULL)
{}

CMeshFileReader::~CMeshFileReader()
{
	if (p) fclose(p);

	if (m_pLongs) free(m_pLongs);
	if (m_pDoubles) free(m_pDoubles);
	if (m_pszRow) free(m_pszRow);	
	
}

XSI::CStatus CMeshFileReader::Init(const XSI::CString& in_inFile)
{
	p = fopen( in_inFile.GetAsciiString(), "rt" );	
	if (!p) return XSI::CStatus::InvalidArgument;
	
	m_pszRow = (char*)malloc( MAX_CHARS );
	return XSI::CStatus::OK;	
}

bool CMeshFileReader::IsEOF()
{
	if (!p) return true;
	return feof(p) == EOF;
}

XSI::CStatus CMeshFileReader::GotoSection( const XSI::CString& in_header )
{	
	// go to beginning of file
	fseek(p,0L,SEEK_SET);
	
	while (!feof(p))
	{
		GetRow();
		
		// remove ending CR if necessary
		size_t nLen = strlen(m_pszRow);
		if (m_pszRow[nLen-1] == '\n')		
			m_pszRow[nLen-1] = NULL;
		
		XSI::CString str;
		str.PutAsciiString(m_pszRow);
		bool bFound = in_header.IsEqualNoCase(str);
		if (bFound)
		{
			// go to first row of values
			if ( fgetc(p) == '{' && fgetc(p) == '\n')
			{				
				return XSI::CStatus::OK;
			}
			// wrong format
			return XSI::CStatus::Fail;
		}			
	}	
	return XSI::CStatus::OK;
}

bool CMeshFileReader::EndSection()
{
	int ch = fgetc(p);
	if ( '}' == ch || EOF == ch)
	{
		return true;
	}
	
	ungetc(ch,p);
	return false;
}

const char* CMeshFileReader::GetRow()
{
	return fgets( m_pszRow, MAX_CHARS, p );
}

const char* CMeshFileReader::GetStringValues()
{	
	const char* pszRow = GetRow();
	if (!pszRow) return NULL;
	
	size_t nLen = strlen(pszRow);	
	for (size_t i = 0; i<nLen; i++ )
	{
		if (pszRow[i] == ':') 
			// skip first blank in the row
			// e.g. "1 2 3\n"
			return &pszRow[i+2 < nLen ? i+2 : i];
	}
	
	return NULL;	
}

LONG CMeshFileReader::GetLongValues(LONG** out_pVals)
{		
	if (m_pLongs)
	{	
		free(m_pLongs);
		m_pLongs = NULL;
	}

	*out_pVals = NULL;
	
	const char* pszVals = GetStringValues();
	if (!pszVals) return 0;

	size_t nLen = strlen(pszVals);
	LONG nCount = 0; // n of values
	LONG start = 0;
	for ( size_t i = 0; i<nLen ; i++ )	
	{	
		if (pszVals[i] == ' ' || pszVals[i] == '\n') 
		{
			// add one more value
			m_pLongs = (LONG*)realloc( m_pLongs, sizeof(LONG)*(++nCount) );
			
			// get string value 
			char strChunk[100];
			strncpy(strChunk,&pszVals[start],(i-start)+1);
			strChunk[i-start] = NULL;
			 
			// store in output buffer
			char* pStopScan;
			m_pLongs[nCount-1] = strtol( strChunk, &pStopScan, 10 );
			
			// set new start index by skipping the blank separator
			start = (LONG)(i+1);
		}
	}
	*out_pVals = m_pLongs;
	return nCount;
}

LONG CMeshFileReader::GetDoubleValues(double** out_pVals)
{		
	if (m_pDoubles)
	{	
		free(m_pDoubles);
		m_pDoubles = NULL;
	}

	*out_pVals = NULL;	
	
	const char* pszVals = GetStringValues();
	if (!pszVals) return 0;

	size_t nLen = strlen(pszVals);
	LONG nCount = 0; // n of values
	LONG start = 0;
	for ( size_t i = 0; i<nLen ; i++ )	
	{			
		if (pszVals[i] == ' ' || pszVals[i] == '\n') 
		{
			// add one more value
			m_pDoubles = (double*)realloc( m_pDoubles, sizeof(double)*(++nCount) );
			
			// get string value 
			char strChunk[100];
			strncpy(strChunk,&pszVals[start],(i-start)+1);
			strChunk[i-start] = NULL;
			 
			// store in output buffer
			char* pStopScan;
			m_pDoubles[nCount-1] = strtod( strChunk, &pStopScan );
			
			// set new start index by skipping the blank separator
			start = (LONG)(i+1);
		}
	}

	*out_pVals = m_pDoubles;
	
	return nCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR* _tstr(const XSI::CString & xstr)
{
#ifdef UNICODE
	return xstr.GetWideString();
#else
	return xstr.GetAsciiString();
#endif
}

		//extract the string data from sgpuString and storte it in the argument 
		//outWString as an std::wstring.
		//Upon error returns 0
		//otherwise return the numberofBytes needed to store the wstring
		//No exception is thrown, No asserts are checked
		int GetWString( std::wstring &outWString, const sgpuString &sgpuInput )
		{
			int nBytesNeeded = sgpuInput.GetNumBytes_WChart();
			CharacterBuffer< 256, wchar_t> cbuf(nBytesNeeded);
			wchar_t *pzbuf = cbuf.GetBuffer();
			nBytesNeeded = sgpuInput.GetData_WChart( pzbuf, nBytesNeeded );
			outWString = pzbuf;
			return nBytesNeeded;
		}
		
		//extract the string data from sgpuString and storte it in the argument 
		//outWString as an std::wstring.
		//Upon error throws an std::rutime_error exception
		//otherwise return the numberofBytes needed to store the wstring
		int GetWStringSafe( std::wstring &outWString, const sgpuString &sgpuInput )
		{
			int nBytesNeeded = GetWString( outWString, sgpuInput );
			assert( nBytesNeeded > 0 );
			if( nBytesNeeded <= 0)
			{
				throw std::runtime_error("error converting from an sgpuString");
			}
			return nBytesNeeded;
		}


		XSI::CString GetSceneFilepath()
		{	
			XSI::Application app;
			XSI::Scene scn = app.GetActiveProject().GetActiveScene();
			XSI::CParameterRefArray params = scn.GetParameters();
			XSI::Parameter p = params.GetItem( L"Filename" );
			XSI::CString sRealName = p.GetValue( double(1) );
			const TCHAR *pzRealName = _tstr( sRealName );
			XSI::Parameter q = params.GetItem( L"Name" );
			XSI::CString sScriptingName = q.GetValue( double(1) );
			const TCHAR *pzScriptingName = _tstr( sScriptingName );
			return sRealName;
		}


		bool GetRelativePath( const XSI::CString &absParentpath, const XSI::CString & absArgpath, Locator &result )
		{
			Locator absArgLocator( _tstr( absArgpath ) );	
			Locator absParentLocator ( _tstr( absParentpath ) );
			Locator absArgLocatorCopy( absArgLocator );
			Locator absParentLocatorCopy ( absParentLocator );
			if( absArgLocator.GetNumNames() <= 0)
			{
				result = absArgLocator;
				return false;
			}
			absArgLocator.Pop();
			assert( absParentLocator.GetNumNames() >= 3);
			absParentLocator.Pop();
			int smallerNumber = min( absArgLocator.GetNumNames(), absParentLocator.GetNumNames() );
			result.Clear();
			result.Push( L"." );
			int i;
			for( i=0 ; i < smallerNumber; ++i )
			{
				const wstring &absArgPart =  absArgLocator.GetName( i );
				const wstring &absParentPart = absParentLocator.GetName( i );
				Locator f1( absArgPart ), f2( absParentPart );
				if ( f1 == f2  )
				{
					continue;
				} else
				{
					if( i == 0)
					{
						return false;
					}
					int numParentDirs = absParentLocator.GetNumNames() - i;
					for( int j=0; j < numParentDirs; ++j )
					{
						result.Push(L"..");
					}
					break;
				}
			}
			for( int j = i; j < absArgLocator.GetNumNames(); ++j )
			{
				result.Push( absArgLocator.GetName( j ) );
			}
			result.Push( absArgLocatorCopy.GetLastName() );
			return true;
		}

		Locator::Locator( const std::wstring &init )
		{
			if( init.length() > 0)
			{
				std::wstring::size_type offset=0;
				std::wstring::size_type tpos;
				std::wstring component;
				while( ( tpos = init.find_first_of( L"/\\" , offset ) ) != std::wstring::npos )
				{
					component = init.substr( offset, tpos - offset );
					push_back( component );
					offset = init.find_first_not_of( L"/\\", tpos );
				}
				component = init.substr( offset, std::wstring::npos );
				push_back( component );
			}
		}
		

		bool Locator::operator==( const Locator &other ) const
		{

			std::wstring wstr1, wstr2;
			wstr1 = ToPath();
			wstr2 = other.ToPath();
			std::transform(wstr1.begin(), wstr1.end(), wstr1.begin(), towlower);
			std::transform(wstr2.begin(), wstr2.end(), wstr2.begin(), towlower);
			return (wstr1 == wstr2);
		}

		const std::wstring & Locator::GetName( int i )const
		{
			assert( i <= static_cast< int > ( size() ) );
			return operator[]( i);
		}
		std::wstring Locator::ToPath() const
		{
			const_iterator cit = begin();
			std::wstring result ( *cit );
			for( ; cit != end(); ++ cit )
			{
				result += L"/";
				result += *cit;
			}
			return result;
		}
		int Locator::GetNumNames() const
		{
			return static_cast< int > ( size() );
		}


		void CopyFile(const XSI::CString& src, const XSI::CString& dst)
		{
			std::wifstream in(_tstr(src), std::ios::in | std::ios::binary);
			std::wofstream out(_tstr(dst), std::ios::out | std::ios::binary | std::ios::trunc);

			if (!in || !out)
			{

				std::wstringstream ss;
				ss << L"cant copy textures " << _tstr(src) << " to " << _tstr(dst );
				throw std::runtime_error( "cant copy textures" );
			}

			wchar_t tmpBuf[2048];

			while (!in.eof())
			{
				in.read(tmpBuf, 2048);

				std::streamsize c = in.gcount();

				out.write(tmpBuf, c);

			}

			in.close();
			out.close();
		}

bool HasExportableGeometry( const XSI::X3DObject &xobj )
{
	
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh xmesh = prim.GetGeometry();
	XSI::NurbsSurfaceMesh xnurbsmesh = prim.GetGeometry();
	return xmesh.IsValid() || xnurbsmesh.IsValid();
}

bool GetMasterInstance( const XSI::X3DObject & possibleInstance, XSI::CRef &ret )
{
		XSI::Application app;
		XSI::CValueArray args;
		XSI::CValue result;
		bool bVal = false;
		XSI::X3DObject xobj_copy( possibleInstance );
		args.Resize(1);
		args[0] = xobj_copy;
		
		app.ExecuteCommand( L"GetMaster", args, result ); 
		if ( result.m_t == XSI::CValue::siRef )
		{
			XSI::Model masterModel = result;
			if ( masterModel.IsValid() )
			{	
				ret = masterModel;
				bVal = true;
			}
		}
		return bVal;
}

bool IsUsefulSgpuNode( const sgpuNode & node )
{
	return node.GetNodeContentType() != sgpuNodeContent::eNone  || node.GetNumChildren() > 0; 
}

sgpuNode FindNodeRec( sgpuNode & node, const XSI::CString & nameTobeMatched )
{
	sgpuNode retVal;
	int nChildren = node.GetNumChildren();
	const TCHAR *pzName = _tstr(nameTobeMatched);
	sgpuString sgpuNameTobeMatched ( pzName );
	sgpuString sgpuNameOfNode = node.GetName();
	std::wstring wNameTobeMatched;
	std::wstring wNameOfNode;
	int n1 = GetWString( wNameTobeMatched, sgpuNameTobeMatched );
	int n2 = GetWString( wNameOfNode, sgpuNameOfNode );
	if( n1 > 0 && n2 > 0 && wNameTobeMatched == wNameOfNode )
	{
		retVal = node;
		return retVal;
	}
	for( int i=0; i < nChildren; ++i )
	{
		sgpuNode child( node.GetChild( i ) );
		retVal = FindNodeRec( child, nameTobeMatched );
		if( IsUsefulSgpuNode( retVal ) )
		{
			return retVal;
		}
	}
	return retVal;
}


void TObjectsTobeExported::AddToArrayBasedOnSortIndex( XSI::CRefArray &srcArray, XSI::CRefArray &dstArray, TObjectsTobeExported &itemsInOrder )
{
	
	//construct and array of sortindices of each item of the source array
	vector< LONG > sortIndices;
	LONG curArrayCount = srcArray.GetCount();
	for(LONG i=0, j=0; i < curArrayCount; ++i )
	{
		XSI::CRef &item = srcArray[ i ];
		LONG indexInArray = itemsInOrder.GetIndex( item );
		//if this src item is not in the items to be exported
		//then dont add its sort index to the indices array
		if( indexInArray >= 0)
		{
			sortIndices.push_back( indexInArray );
		}
	}
	//some  checking
	assert( sortIndices.size() <= srcArray.GetCount() );
	
	//sort the sortIndices array
	std::sort( sortIndices.begin(), sortIndices.end() );
	std::vector< LONG > :: const_iterator vit;
	//If the src array is unsique and if the sortIndices array
	//has more than one items, then the sortIndices array
	//is guaranteed to be unique
	if( sortIndices.size() > 1 )
	{
		for( vit = sortIndices.begin(); (vit + 1) != sortIndices.end(); ++vit )
		{
			assert( *vit < *(vit+1) );
		}
	}
	//now for each index in the indices array
	for( vit = sortIndices.begin(); vit != sortIndices.end(); ++vit )
	{
		LONG indexInArray = -1;
		//find the item in the srcArray with that index
		//and add that item to the dstArray
		//This will ensure that the items in the dst  array
		//are in the same order as their indices in the index array
		for(LONG j=0; j < curArrayCount; ++ j )
		{
			XSI::CRef &item = srcArray[ j ];
			indexInArray = itemsInOrder.GetIndex( item );
			if( indexInArray == *vit )
			{
				dstArray.Add( item );
				break;
			}
		}
		assert( indexInArray >= 0);
	}
}



void TransformStack::PushStackWithCurrentObjectTm( const XSI::X3DObject &xobj )
{
	//get the total transform
	XSI::Kinematics kin = xobj.GetKinematics().EvaluateAt( static_cast< double > ( m_curFrame ) );
	XSI::MATH::CMatrix4 gM = kin.GetGlobal().GetTransform().GetMatrix4();		
	//store the total transform to be used by exportMesh
	m_sTransform.push_back( gM );
}
void TransformStack::ApplyTmToSgpuNode ( sgpuNode & node )
{

	if( !m_bExportAnimation )
	{	// get object local transform
		const XSI::MATH::CMatrix4 &gM = m_sTransform.back();
		assert( m_sTransform.size() > 1);
		//get the next up transform in the stack
		XSI::MATH::CMatrix4 ipM = m_sTransform[ m_sTransform.size() - 2 ];
		ipM.InvertInPlace();    
		XSI::MATH::CMatrix4 tM;
		tM.Mul(gM, ipM);
		sgpuMatrix m(
			(float)(tM.GetValue(0,0)), (float)(tM.GetValue(0,1)), (float)(tM.GetValue(0,2)), (float)(tM.GetValue(0,3)),
			(float)(tM.GetValue(1,0)), (float)(tM.GetValue(1,1)), (float)(tM.GetValue(1,2)), (float)(tM.GetValue(1,3)),
			(float)(tM.GetValue(2,0)), (float)(tM.GetValue(2,1)), (float)(tM.GetValue(2,2)), (float)(tM.GetValue(2,3)),
			(float)(tM.GetValue(3,0)), (float)(tM.GetValue(3,1)), (float)(tM.GetValue(3,2)), (float)(tM.GetValue(3,3))
			); 
		node.SetTransformationMatrix(m);
	}
}



TransformStackForDeInstancing::TransformStackForDeInstancing(const XSI::CRef &referencingObjRef,const XSI::CRef &referencedObjRef, bool bExportAnimation, double curFrame ):
TransformStack( bExportAnimation, curFrame )
{
	XSI::X3DObject referencingObj ( referencingObjRef );
	XSI::Kinematics kin = referencingObj.GetKinematics().EvaluateAt(  m_curFrame  );
	XSI::MATH::CMatrix4 globalTmOfReferencingObj = kin.GetGlobal().GetTransform().GetMatrix4();		

	XSI::X3DObject referencedObj ( referencedObjRef );
	XSI::Kinematics kin2 = referencedObj.GetKinematics().EvaluateAt( static_cast< double > ( m_curFrame ) );
	XSI::MATH::CMatrix4 globalTmOfReferencedObj = kin2.GetGlobal().GetTransform().GetMatrix4();		

	XSI::MATH::CMatrix4 invOfGlobalTmOfReferencedObj( globalTmOfReferencedObj);
	invOfGlobalTmOfReferencedObj.InvertInPlace();
	m_reReferencingModulatorTm.Mul( invOfGlobalTmOfReferencedObj, globalTmOfReferencingObj );
}



void TransformStackForDeInstancing::PushStackWithCurrentObjectTm( const XSI::X3DObject &xobj )
{
	//get the total transform
	XSI::Kinematics kin = xobj.GetKinematics().EvaluateAt( static_cast< double > ( m_curFrame ) );
	XSI::MATH::CMatrix4 gM = kin.GetGlobal().GetTransform().GetMatrix4();		
	gM.MulInPlace( m_reReferencingModulatorTm );

	//store the total transform to be used by exportMesh
	m_sTransform.push_back( gM );
}
