/****************************************************************************\
**  sgpuConstructor.hpp
**
**	sgpuConstructor.hpp is a base class for the helper class for constructing an
**	sgpuMesh or sgpuSubdiv
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_CONSTRUCTOR_HPP
#define SGPU_CONSTRUCTOR_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"
#include "sgpuMesh.hpp"
#include "sgpuMaterial.hpp"


//============================================================================
//**	sgpuConstructor.hpp is a base class for the helper class for constructing 
//**	an sgpuMesh or sgpuSubdiv. See the derivations of this class as in
//**	sgpuMeshConstructor and sgpuSubdivConstructor
//============================================================================
struct sgpuMeshImpl;
class sgpuNode;
class sgpuModelExportScene;
struct sgpuConstructorImpl;
class sgpuPropertyValue;
class sgpuString;

#pragma warning( disable: 4251 )
//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuConstructor
{
public:
	//An embedded struct used to specify the corner vertex of a face.
	struct FaceVertex
	{
		int m_VertexId;
		int m_NormalId;
		int m_UVId;
		FaceVertex()
		{
			m_VertexId = -1;
			m_NormalId = -1;
			m_UVId = -1;
		}
		void Set( int i_VIdx, int i_NIdx, int i_UVIdx )
		{
			m_VertexId = i_VIdx;
			m_UVId = i_UVIdx;
			m_NormalId = i_NIdx;
		}
	};
public:
	//i_Scene = the sgpuModelExportScene for which we are constructing the sgpuMesh or sgpuSubdiv
	//i_Node = the node for shich the sgpuMesh or sgpuSubdiv is goimg to be the node content
	sgpuConstructor( sgpuModelExportScene &i_Scene, sgpuNode & i_Node );	
	virtual ~sgpuConstructor();
	//assign properties to the sgpuMesh or sgpuSubdiv that is constructed
	virtual bool GetProperty(const sgpuString &i_PropertyName, sgpuPropertyValue &o_Property )=0;
	virtual void SetProperty(const sgpuString &i_PropertyName, const sgpuPropertyValue & i_PropertyValue)=0;
protected:
	//only one sgpuConstructor needed for constructing a mesh
	sgpuConstructor( const sgpuConstructor &other );
	//no need to do assignment 
	sgpuConstructor &operator=( const sgpuConstructor & i_Other);
protected:
		 sgpuNode &m_Node;
		 sgpuModelExportScene & m_Scene;
};


#endif // #ifndef SGPU_CONSTRUCTOR_HPP