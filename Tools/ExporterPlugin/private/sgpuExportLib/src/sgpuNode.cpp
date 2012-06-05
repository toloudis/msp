/****************************************************************************\
**  sgpuNode.cpp
**
**      sgpuNode.hpp defines the sgpuNode class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuNode.hpp"
#include "sgpuMatrix.hpp"
#include "sgpuMesh.hpp"
#include "sgpuMeshImpl.hpp"
#include "sgpuSubdiv.hpp"
#include "sgpuSubdivImpl.hpp"
#include "sgpuNodeImpl.hpp"
#include "sgpuMeshConstructor.hpp"
#include "sgpuNodeContent.hpp"
#include "sgpuPathReference.hpp"
#include "sgpuPathReferenceImpl.hpp"
#include "sgpuModelExportScene.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"

#include "Graphics/smdl/smdlCharacterSkin.hpp"


#include "Core/It/itStringUtil.hpp"

#include <sstream>
#include <vector>
sgpuNode::sgpuNode()
: m_pImpl( new sgpuNodeImpl )	
{
	m_pImpl->m_Node.reset( new mdlNodeInfo() );
} 
sgpuNode::sgpuNode(const sgpuNode &i_Node):
	m_pImpl( new sgpuNodeImpl( *i_Node.m_pImpl ) )
	{}
sgpuNode::~sgpuNode()
{ 
	delete m_pImpl;
}
sgpuNode& sgpuNode::operator=(const sgpuNode& i_CopyFrom)
{
	if (&i_CopyFrom != this)
	{
		delete m_pImpl;
		m_pImpl = new sgpuNodeImpl(*i_CopyFrom.m_pImpl);
	}
	return *this;
}
//========================================================================
// Set name of the node in the scene graph
//========================================================================

void sgpuNode::SetName(const sgpuString& i_Name)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" )
	m_pImpl->m_Node->m_NodeName = i_Name.m_pImpl->m_Data;
}

sgpuString sgpuNode::GetName() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" )
	return sgpuString( m_pImpl->m_Node->m_NodeName.c_str() );
}


//========================================================================
// Set the transformation for this scene graph node
//========================================================================
void sgpuNode::SetTransformationMatrix(const sgpuMatrix& i_Matrix)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" )
	// Have to copy over each component of matrix because the
	// classes are different.
	for (int i=0; i<4; ++i)
		for( int j=0; j < 4 ; ++j)
		{
			m_pImpl->m_Node->m_Transform.m_Mat[i*4 + j] = i_Matrix(i,j);
		}
}

sgpuMatrix sgpuNode::GetTransformationMatrix() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	// Have to copy over each component of matrix because the
	// classes are different.
	sgpuMatrix mat;
	for (int i=0; i<4; ++i)
		for( int j=0; j < 4 ; ++j)
		{
			mat(i, j) = m_pImpl->m_Node->m_Transform.m_Mat[i*4 + j];
		}
	return mat;
}

bool sgpuNode::operator==( const sgpuNode &i_Other)const
{
	return *m_pImpl == *i_Other.m_pImpl;
}


//========================================================================
// Add new child node to this node, returning handle.
//========================================================================
sgpuNode sgpuNode::AddChildNode()
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	sgpuNode child_node;
	//making sure that the number of children is always < int limit
	if (static_cast<int>(m_pImpl->m_Node->m_Children.size()+2 >= 0 ) )
	{
		child_node.m_pImpl->m_Node.reset( new mdlNodeInfo );
		m_pImpl->m_Node->m_Children.push_back( child_node.m_pImpl->m_Node );
	} 
	return child_node;
}

void sgpuNode::DetachChildNode( sgpuNode &i_Child)
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	std::vector< shared_ptr<mdlNodeInfo> >::iterator vit = find( m_pImpl->m_Node->m_Children.begin(), m_pImpl->m_Node->m_Children.end(), i_Child.m_pImpl->m_Node );
	m_pImpl->m_Node->m_Children.erase( vit );
}

int sgpuNode::GetNumChildren() const
{
	int retVal = 0;
	if (m_pImpl->m_Node)
	{
		
		retVal = static_cast< int > ( m_pImpl->m_Node->m_Children.size() );
	}
	return retVal;
}

sgpuNode sgpuNode::GetChild( int i_ChildIdx)
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	INVALID_RANGE_EXCEPTION( i_ChildIdx, m_pImpl->m_Node->m_Children.size(), "sgpuNode" )
	sgpuNode child_node;
	child_node.m_pImpl->m_Node = m_pImpl->m_Node->m_Children[ i_ChildIdx ];
	return child_node;
}


//========================================================================
// Create an indexed triangle mesh at this node in the scene graph.
// Each node can only contain one mesh, so if this method is
// called a second time, it will return a handle to the same mesh
// created in the first call.
//========================================================================
sgpuMesh sgpuNode::CreateNodeContent_TriangleMesh(bool i_bVertexAnimation )
{
	sgpuMesh mesh;
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	// Create mesh info structure only if not already created
	if (!m_pImpl->m_Node->m_MeshInfo)
	{
		m_pImpl->m_Node->m_MeshInfo.reset( new mdlFragInfo );
	}
	if ( i_bVertexAnimation )
	{
		m_pImpl->m_Node->m_SkinInfo.reset( new smdlCharacterSkin() );
	}
	m_pImpl->m_Node->m_MeshInfo->m_Flags.m_bVertexAnimation = true;
	mesh.m_pImpl->m_Mesh = m_pImpl->m_Node->m_MeshInfo;
	return mesh;
}

sgpuSubdiv sgpuNode::CreateNodeContent_SubdivisionSurface( bool i_bVertexAnimation )
{
	sgpuSubdiv subd;
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	// Create mesh info structure only if not already created
	if (!m_pImpl->m_Node->m_SubdivInfo)
	{
		m_pImpl->m_Node->m_SkinInfo.reset( new smdlCharacterSkin() );
	}
	if ( i_bVertexAnimation )
	{
		m_pImpl->m_Node->m_SubdivInfo.reset( new mdlSubdivInfo );
		
	}
	subd.m_pImpl->m_bVertexAnim = i_bVertexAnimation;
	subd.m_pImpl->m_Subdiv = m_pImpl->m_Node->m_SubdivInfo;
	return subd;
}

sgpuPathReference sgpuNode::CreateNodeContent_PathReference(const sgpuNode &i_RootNode, const sgpuNode &i_DesiredDescendant)
{
	sgpuPathReference pathRef;
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	// Create mesh info structure only if not already created
	if (!m_pImpl->m_Node->m_InstanceInfo)
	{
		m_pImpl->m_Node->m_InstanceInfo.reset( new mdlNodeInfoProxy );
	}
	std::vector< shared_ptr< mdlNodeInfo > > tempPath;
	bool bRet = sgpuNodeImpl::GetPathToChildRec( i_DesiredDescendant.m_pImpl, i_RootNode.m_pImpl->m_Node, tempPath );
	std::reverse( tempPath.begin(), tempPath.end() );
	std::vector< shared_ptr< mdlNodeInfo > >::const_iterator cit;
	m_pImpl->m_Node->m_InstanceInfo->m_Path.clear();
	for( cit = tempPath.begin(); cit != tempPath.end(); ++cit)
	{
		shared_ptr< mdlNodeInfo > node = *cit;
		const std::string &sNodeName = node->m_NodeName;
		m_pImpl->m_Node->m_InstanceInfo->m_Path.push_back( node->m_NodeName );
	}
	pathRef.m_pImpl->m_InstanceInfo = m_pImpl->m_Node->m_InstanceInfo;
	return pathRef;
}

void sgpuNode::ResetNodeContent()
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	// Create mesh info structure only if not already created
	if ( m_pImpl->m_Node->m_SubdivInfo )
	{
		m_pImpl->m_Node->m_SubdivInfo.reset();
	} else if (  m_pImpl->m_Node->m_MeshInfo )
	{
		m_pImpl->m_Node->m_MeshInfo.reset();
	} else if(m_pImpl->m_Node->m_InstanceInfo )
	{
		m_pImpl->m_Node->m_InstanceInfo.reset();
	}
	if( m_pImpl->m_Node->m_SkinInfo )
	{
		m_pImpl->m_Node->m_SkinInfo.reset();
	}
}

sgpuNodeContent::ESgpuNodeContentType   sgpuNode::GetNodeContentType() const
{
	sgpuNodeContent::ESgpuNodeContentType trivial = sgpuNodeContent::eNone;
	if( m_pImpl->m_Node )
	{
		if( m_pImpl->m_Node->m_SubdivInfo.get() )
		{
			return sgpuNodeContent::eSubdiv;
		} else if ( m_pImpl->m_Node->m_MeshInfo.get() )
		{
			return sgpuNodeContent::eMesh;
		} else if ( m_pImpl->m_Node->m_InstanceInfo.get() )
		{
			return sgpuNodeContent::ePathReference;
		}
	
	}
	return trivial;
}
sgpuSubdiv sgpuNode::GetNodeContentSubdiv()
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	NO_IMPL_EXCEPTION( m_pImpl->m_Node->m_SubdivInfo, "sgpuNode::subdivInfo" );
	sgpuSubdiv subdiv;
	subdiv.m_pImpl->m_Subdiv = m_pImpl->m_Node->m_SubdivInfo;
	return subdiv;
}

sgpuMesh sgpuNode::GetNodeContentMesh()
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	NO_IMPL_EXCEPTION( m_pImpl->m_Node->m_MeshInfo, "sgpuNode::mesh" );
	sgpuMesh mesh;
	mesh.m_pImpl->m_Mesh = m_pImpl->m_Node->m_MeshInfo;
	return mesh;
}


sgpuPathReference sgpuNode::GetNodeContentPathReference()
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Node, "sgpuNode" );
	NO_IMPL_EXCEPTION( m_pImpl->m_Node->m_InstanceInfo, "sgpuNode::pathReference" );
	sgpuPathReference pathRef;
	pathRef.m_pImpl->m_InstanceInfo = m_pImpl->m_Node->m_InstanceInfo;
	return pathRef;
}

#if defined( SGPU_SUPPORT_1200)
void sgpuNode::SetNodeName(const char* i_Name)
{
	SetName( sgpuString( i_Name ) );
}
void sgpuNode::SetNodeName(const wchar_t* i_Name)
{
	SetName( sgpuString( i_Name ) );
}
sgpuMesh sgpuNode::CreateTriangleMesh()
{
	return CreateNodeContent_TriangleMesh();
}
#endif //SGPU_SUPPORT_1200
