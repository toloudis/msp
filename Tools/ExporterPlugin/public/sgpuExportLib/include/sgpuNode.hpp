/****************************************************************************\
**  sgpuNode.hpp
**
**      sgpuNode.hpp defines class for handle to a transformation node within
**	a scene graph within a sgpuModelExportScene.
**
**	sgpuNode is a handle to an internal node representation
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_NODE_HPP
#define SGPU_NODE_HPP

#include "sgpuExportLib.hpp"
#include "sgpuNodeContent.hpp"
#include "sgpuMesh.hpp"


//============================================================================
//============================================================================
class sgpuMatrix;
class sgpuMesh;
class sgpuSubdiv;
class sgpuModelExportScene;
struct sgpuNodeImpl;
class sgpuMeshConstructor;
class sgpuSubdivConstructor;
class sgpuPathReference;



//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuNode
{
public:
	//========================================================================
	// Constructors, Destructors of sgpuNode
	//========================================================================	
	sgpuNode();
	sgpuNode( const sgpuNode & i_Other );
	~sgpuNode();
	sgpuNode & operator=( const sgpuNode & i_Other );
	//========================================================================
	// Get/Set name of the node in the scene graph
	//========================================================================

	void SetName(const sgpuString &i_Name );
	sgpuString GetName() const;

	//========================================================================
	// Get/Set the transformation for this scene graph node
	//========================================================================
	void SetTransformationMatrix(const sgpuMatrix& i_Matrix);
	sgpuMatrix GetTransformationMatrix() const;
	//========================================================================
	// Add new child node to this node, returning handle.
	//========================================================================
	sgpuNode AddChildNode( );
	//If the curent node has a chid node that is equal to the
	//i_Child then relinquish parenthood nof that child
	void DetachChildNode( sgpuNode &child );
	//Get the number of children parented by this node
	int GetNumChildren() const;
	//Get the ith child
	sgpuNode GetChild( int i);
	

	//returns true if this node and i_Other points to the same 
	//internal node representation
	bool operator==( const sgpuNode &i_Other)const;
	//Resets any Node content this node may be having
	void ResetNodeContent();
	//returns the type of node content that this node contain.
	sgpuNodeContent::ESgpuNodeContentType   GetNodeContentType() const;
	//If the node content is a sgpuSubdiv node
	//return the sgpuSubdiv, otherwise this throws an sgpuException
	sgpuSubdiv GetNodeContentSubdiv();
	//If the node content is a sgpuMesh node
	//return the sgpuMesh, otherwise this throws an sgpuException
	sgpuMesh GetNodeContentMesh();
	//If the node content is a sgpuPathReference node
	//return the sgpuPathReference, otherwise this throws an sgpuException
	sgpuPathReference GetNodeContentPathReference();
	//========================================================================
	// Create an indexed triangle mesh at this node in the scene graph.
	// Each node can only contain one mesh, so if this method is
	// called a second time, it will return a handle to the same mesh
	// created in the first call.
	//========================================================================
	sgpuMesh CreateNodeContent_TriangleMesh( bool i_bVertexAnimation = false );
	//========================================================================
	// Create a subdivision mesh at this node in the scene graph.
	// Each node can only contain one mesh, so if this method is
	// called a second time, it will return a handle to the same mesh
	// created in the first call.
	//========================================================================
	sgpuSubdiv CreateNodeContent_SubdivisionSurface( bool i_bVertexAnimation = false );
	//========================================================================
	// Create a refernce to another node at this node in the scene graph.
	// THis can be used for instancing.
	// Each node can only contain one path reference, so if this method is
	// called a second time, it will return a handle to the same mesh
	// created in the first call.
	//========================================================================
	sgpuPathReference CreateNodeContent_PathReference(const sgpuNode &i_RootNode, const sgpuNode &i_DesiredDescendant);
	//obsolete calls
#if defined( SGPU_SUPPORT_1200)
	void SetNodeName(const char* i_Name);	
	void SetNodeName(const wchar_t* i_Name);	
	sgpuMesh CreateTriangleMesh();
#endif	

	friend sgpuModelExportScene;
	friend sgpuMeshConstructor;
	friend sgpuSubdivConstructor;

private:
	sgpuNodeImpl *m_pImpl;

};

#endif // #ifndef SGPU_NODE_HPP