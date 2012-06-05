/****************************************************************************\
**  sgpuExportLib.cpp
**
**      sgpuExportLib.cpp defines boost-python bindings for the
**		classes exposed in the include directory of the sgpuExportLib
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/errors.hpp>
#include <iostream>
#include <string>

#include "sgpuModelExportScene.hpp"
#include "sgpuBaseScene.hpp"
#include "sgpuMesh.hpp"
#include "sgpuNode.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuVector.hpp"
#include "sgpuMatrix.hpp"

//========================================================================
//	anonymous namespace for some helpful wrappers
//========================================================================	
namespace
{

	//========================================================================
	//	wrapper for supporting the '[]' operator in python
	//========================================================================	
	inline float getSgpuVector3( const sgpuVector3 &vec3, int idx )
	{
		if( idx >= 3 )
		{
			PyErr_SetString(PyExc_IndexError, "index out of range");
			boost::python::throw_error_already_set();
		}
		assert( idx < 3 );
		return vec3( idx );
	}
	
	//========================================================================
	//	wrapper for supporting the '[]' operator in python
	//========================================================================	
	inline void setSgpuVector3( sgpuVector3 &vec3, int idx, float fVal )
	{

		if( idx >= 3 )
		{
			PyErr_SetString(PyExc_IndexError, "index out of range");
			boost::python::throw_error_already_set();
		}

		assert( idx < 3 );
		vec3.m_Data[ idx ] = fVal;

	}
	
	//========================================================================
	//	wrapper for supporting the '[]' operator in python
	//========================================================================	
	inline sgpuVector3 &getSgpuMatrix( sgpuMatrix &mat, int idx )
	{
		if( idx >= 4 )
		{
			PyErr_SetString(PyExc_IndexError, "index out of range");
			boost::python::throw_error_already_set();
		}

		assert( idx < 4 );
		return mat.m_Vec[ idx ];
	}
	
	//========================================================================
	//	wrapper for supporting the '[]' operator in python
	//========================================================================	
	inline void setSgpuMatrix( sgpuMatrix &mat, int idx, const sgpuVector3& vecVal )
	{
		if( idx >= 4 )
		{
			PyErr_SetString(PyExc_IndexError, "index out of range");
			boost::python::throw_error_already_set();
		}

		assert( idx < 4 );
		mat.m_Vec[ idx ] = vecVal;
		mat.m_Vec[ idx ].m_Data[3] = 0.0f;
		if( idx == 3 )
		{
			mat.m_Vec[ idx ].m_Data[ 3 ] = 1.0f;
		}
	}
	
	//========================================================================
	//  Boost Python EDSL (embedded domain specific language) 
	//  doesnt work well with 'char *' arguments.
	//	So whereever, there is a char * argument,
	//  we make a wrapper wrapper which accepts an std::string argument
	//========================================================================	
	bool WriteScene( sgpuScene & i_Scene, const std::string & i_Filename,
		const std::string & i_VersionString)
	{
		return i_Scene.WriteScene( i_Filename.c_str(), i_VersionString.c_str() );
	}
	
	//========================================================================
	//  Boost Python EDSL (embedded domain specific language) 
	//  doesnt work well with 'char *' arguments.
	//	So whereever, there is a char * argument,
	//  we make a wrapper wrapper which accepts an std::string argument
	//========================================================================	

	sgpuMaterial CreateMaterial( sgpuScene &i_Scene, const std::string &i_MaterialName)
	{
		return i_Scene.CreateMaterial( i_MaterialName.c_str() );
	}
	//========================================================================
	//  Boost Python EDSL (embedded domain specific language) 
	//  doesnt work well with 'char *' arguments.
	//	So whereever, there is a char * argument,
	//  we make a wrapper wrapper which accepts an std::string argument
	//========================================================================	

	void SetNodeName( sgpuNode &i_Node, const std::string & i_Name)
	{
		i_Node.SetNodeName( i_Name.c_str() );
	}
	//========================================================================
	//  Boost Python EDSL (embedded domain specific language) 
	//  doesnt work well with 'char *' arguments.
	//	So whereever, there is a char * argument,
	//  we make a wrapper wrapper which accepts an std::string argument
	//========================================================================	

	void SetMeshName( sgpuMesh &i_Mesh, const std::string &i_Name)
	{
		i_Mesh.SetMeshName( i_Name.c_str() );
	}
	//========================================================================
	//  Boost Python EDSL (embedded domain specific language) 
	//  doesnt work well with 'char *' arguments.
	//	So whereever, there is a char * argument,
	//  we make a wrapper wrapper which accepts an std::string argument
	//========================================================================	

	void SetDiffuseTexture( sgpuMaterial &i_Mat, const std::string& i_TextureFilename)
	{
		i_Mat.SetDiffuseTexture( i_TextureFilename.c_str() );
	}


	//========================================================================
	//  A utility class that encapsulates a character buffer 
	//  
	//========================================================================	
	template< int NStaticSize , typename C >
	struct CharacterBuffer
	{
	public:
		CharacterBuffer(int nSize):
		  m_pDynamicBuffer( NULL ),
			  m_Size( NStaticSize )
		  {
			  if( nSize > NStaticSize )
			  {
				  m_pDynamicBuffer = new C [ nSize ];
				  m_Size = nSize;
			  }
		  }
		  ~CharacterBuffer()
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
		  int m_Size;
	};

	
	//========================================================================
	//Wrapper for the sgpuScene::Describe function
	//========================================================================	
	std::string Describe( sgpuScene &i_Scene )
	{
		sgpuString desc;
		std::string retVal;
		i_Scene.Describe( desc );
		int nBytes = desc.GetNumBytes_UTF8();
		if( nBytes > 0 )
		{
			CharacterBuffer< 1024, char > buffer( nBytes );
			nBytes = desc.GetData_UTF8( buffer.GetBuffer(), nBytes );
			if( nBytes > 0 ) retVal = std::string( buffer.GetBuffer() );
		}
		return retVal;
	}

}



BOOST_PYTHON_MODULE(sgpuExportLib)
{
    using namespace boost::python;

	class_<sgpuVector3> ( "sgpuVector3" ) //default init
		.def( init< float, float, float >() ) //init with 3 floats
		.def( self + self ) //+  operator
		.def( self - self ) //-  operator
		.def( self * self ) //dot product operator
		.def( self * float() ) //scaling operator
		.def( self == self ) //equality operator
		.def( "EpsilonEqual", &sgpuVector3::EpsilonEqual ) //approximate equal
				//[] operator, returns an internal reference
		.def( "__getitem__", getSgpuVector3 )					
		.def( "__setitem__", setSgpuVector3 ) //[] operator
		;

	
	
	class_<sgpuMatrix>( "sgpuMatrix" ) //default init
		.def(init< const sgpuVector3 &, const sgpuVector3 &, const sgpuVector3 & , const sgpuVector3 &>() )  //init with 4 sgpuVector3 
		.def("MakeTranslate", &sgpuMatrix::MakeTranslate ) 
		.def("MakeRotate", &sgpuMatrix::MakeRotate )
		.def("MakeScale", &sgpuMatrix::MakeScale )
		.def( self * self ) // * operator, matrix multiplication
		.def( self == self ) //equality operator
		.def( "EpsilonEqual", &sgpuMatrix::EpsilonEqual ) //approximation operator
		.def( "__getitem__", getSgpuMatrix, return_internal_reference<>() ) //returns an internal reference to the i'th vector,
		.def( "__setitem__", setSgpuMatrix )
		.def( "Identity", &sgpuMatrix::Identity ) //makes an identity matrix
		;



	class_<sgpuMaterial>("sgpuMaterial", no_init)
		.def("SetDiffuseColor", &sgpuMaterial::SetDiffuseColor )
		.def("SetOpacity", &sgpuMaterial::SetOpacity )
		.def("SetSpecularColor", &sgpuMaterial::SetSpecularColor )
		.def("SetShininess", &sgpuMaterial::SetShininess )
		.def("SetDiffuseTexture", SetDiffuseTexture )
		;


	class_<sgpuMesh>("sgpuMesh", no_init)
		.def("SetMeshName", SetMeshName )
		.def("SetNumVertices", &sgpuMesh::SetNumVertices )
		.def("SetPosition", &sgpuMesh::SetPosition )
		.def("SetNormal", &sgpuMesh::SetNormal )
		.def("SetTexCoord", &sgpuMesh::SetTexCoord )
		.def("SetNumIndices", &sgpuMesh::SetNumIndices )
		.def("SetIndex", &sgpuMesh::SetIndex )
		.def("AssignMaterial", &sgpuMesh::AssignMaterial )
		;



	class_<sgpuNode>("sgpuNode", no_init)
		.def("SetNodeName", SetNodeName )
		.def("SetTransformationMatrix", &sgpuNode::SetTransformationMatrix )
		.def("AddChildNode", &sgpuNode::AddChildNode )
		.def("CreateTriangleMesh", &sgpuNode::CreateTriangleMesh )
		;


	class_<sgpuBaseScene, boost::noncopyable>("sgpuBaseScene", no_init)
		;

	class_<sgpuScene, bases< sgpuBaseScene > , boost::noncopyable>("sgpuScene")
		.def("WriteScene", WriteScene )
		.def("GetRootNode", &sgpuScene::GetRootNode )
		.def("CreateMaterial", CreateMaterial )
		.def("Describe", Describe )
		;



}